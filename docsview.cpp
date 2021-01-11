#include "docsview.h"
#include "ui_docsview.h"
#include "docs.h"
#include "imageviewer.h"
#include "scanerview.h"
#include <QSqlTableModel>
#include <QMessageBox>
#include <QBuffer>
#include <QMediaPlayer>
#include <QLineEdit>
#include <QTime>
#include <QPrintDialog>
#include <QPrinter>
#include <QPainter>

docsView::docsView(bool user, QWidget* parent, int idCase, int idClient):
    QDialog(parent),
    ui(new Ui::docsView),
    m_user(user),
    m_idCase(idCase),
    m_idClient(idClient),
    player(new QMediaPlayer(this)),
    audioStream(new QBuffer(this))
{
    ui->setupUi(this);
    this->setWindowTitle(QObject::tr("Документы"));

    QSplitterHandle* handle = ui->splitterFoto->handle(1);
    handle->setEnabled(false);
    handle = ui->splitterAudio->handle(1);
    handle->setEnabled(false);

    ui->widgetDocs->setTabText(0, QObject::tr("Фото документы"));
    ui->widgetDocs->setTabText(1, QObject::tr("Аудио документы"));
    ui->widgetDocs->setTabText(2, QObject::tr("Текст документы"));

    ui->widgetDocs->setCurrentIndex(0); //for set Tab Foto at first

    docsBE = new Docs(this);

    if(m_user)
    {
        ui->checkBoxUser->toggle();
    }
    setComboBoxes();

    //"signal-slot" connections for updating Models and Views
    connect(this, &docsView::startUpdateView,  docsBE, &Docs::startUpdateModel);
    connect(docsBE, &Docs::finishUpdateModel, this, &docsView::finishUpdateView);

    //"signal-slot" connections for adding Docs
    connect(this, &docsView::addDocs,  docsBE, &Docs::addDocs);

    connect(this, &docsView::addScanImage,  docsBE, &Docs::addScanOrText);

    //preview image
    connect(ui->viewFoto, &QTableView::clicked, this, &docsView::previewFoto);

    //connections for play audio
    connect(ui->viewAudio, &QTableView::clicked, this, [this](){isLoaded = false;});

    connect(this, &docsView::audioPosTime, ui->sliderAudioDur, &QSlider::setValue);
    connect(player, &QMediaPlayer::positionChanged, this, [this](qint64 time)
                    { QTime displayTime(0, (time / 60000) % 60, (time / 1000) % 60);
                      ui->lcdTime->display(displayTime.toString("mm:ss"));
                      if(player->duration()!=0) emit audioPosTime(time); });
    connect(ui->sliderAudioDur, &QSlider::sliderMoved, this, [this](int time)
                                                       {player->setPosition(time);});
    connect(player, &QMediaPlayer::durationChanged, this, [this](qint64 val)
                                                          {ui->sliderAudioDur->setMaximum(val);});

    connect(docsBE, &Docs::startMessageTextWarning, this, &docsView::startMessageTextWarning);
    connect(docsBE, &Docs::endMessageTextWarning, this, &docsView::endMessageTextWarning);

    arrFunc[1] = &docsView::on_comboBoxClient_activated;
    arrFunc[2] = &docsView::on_comboBoxCase_activated;

    allCombos  << nullptr << ui->comboBoxClient << ui->comboBoxCase;

    m_dv = DocViews::FOTO;
    emit startUpdateView(m_dv, true, 0, 0);

    if (m_idClient)
    {
        ui->checkBoxClient->toggle();
        int ind = indexesClientsCombo.key(m_idClient);
        ui->comboBoxClient->setCurrentIndex(ind);
        ui->comboBoxClient->activated(ind);
    }
    if (m_idCase)
    {
        ui->checkBoxCase->toggle();
        int ind = indexesCasesCombo.key(m_idCase);
        ui->comboBoxCase->setCurrentIndex(ind);
        ui->comboBoxCase->activated(ind);
    }
}

docsView::~docsView()
{
    delete ui;
}

void docsView::previewFoto(const QModelIndex& ind)
{
    if (static_cast<bool>(m_dv)) return;
    image.loadFromData(docsBE->getFile(ind));
    QPixmap fromImage = QPixmap::fromImage(image, Qt::AutoColor);
    ui->labelPreview->setPixmap(fromImage.scaled(QSize(ui->labelPreview->size()), Qt::KeepAspectRatio));
}

void docsView::on_checkBoxUser_toggled(bool checked)
{
    if (checked)
    {
        ui->comboBoxCase->setEnabled(false);
        ui->comboBoxClient->setEnabled(false);
        ui->checkBoxCase->setChecked(false);
        ui->checkBoxClient->setChecked(false);

        ui->labelOwner->setText(QObject::tr("Документы владельца: Пользователь"));
        //clear preview label
        ui->labelPreview->clear();
        comboChanged = 0;
        emit startUpdateView(m_dv, true, 0, 0);
    }
}

void docsView::on_checkBoxClient_toggled(bool checked)
{
    if (checked)
    {
        ui->comboBoxCase->setEnabled(false);
        ui->comboBoxClient->setEnabled(true);
        ui->checkBoxCase->setChecked(false);
        ui->checkBoxUser->setChecked(false);
        ui->comboBoxClient->activated(ui->comboBoxClient->currentIndex());
    }
    else
    {
        ui->comboBoxClient->setEnabled(false);
    }
}

void docsView::on_checkBoxCase_toggled(bool checked)
{
    if (checked)
    {
        ui->comboBoxCase->setEnabled(true);
        ui->comboBoxClient->setEnabled(false);
        ui->checkBoxClient->setChecked(false);
        ui->checkBoxUser->setChecked(false);

        ui->comboBoxCase->activated(ui->comboBoxCase->currentIndex());
    }
    else
    {
        ui->comboBoxCase->setEnabled(false);        
    }
}

void docsView::finishUpdateView(QSqlTableModel* model, DocViews dv)
{
    //set model to the view
    QList<QTableView*> allViews {ui->viewFoto, ui->viewAudio, ui->viewText};
    m_dv = dv;
    int ind = static_cast<int>(dv);
    allViews[ind]->setModel(model);
    allViews[ind]->setSelectionBehavior(QAbstractItemView::SelectRows);
    int i = 0;
    //hide other than viewable headers
    for(; i < 3; i++)
    {
        allViews[ind]->setColumnHidden(i, true);
    }
    allViews[ind]->setSortingEnabled(true);
    allViews[ind]->sortByColumn(4, Qt::AscendingOrder); //DATE
    allViews[ind]->setEditTriggers(QAbstractItemView::NoEditTriggers);

    allViews[ind]->resizeColumnsToContents();

    //can move by keys
    connect(allViews[ind]->selectionModel(), &QItemSelectionModel::currentRowChanged, this, &docsView::previewFoto);
}

void docsView::setComboBoxes()
{
    //set clients combo box
    QMap<int, QString> mapInfo = docsBE->getClientsForView();
    int indCombo = 0;
    for (const auto& key : mapInfo.keys())
    {
        ui->comboBoxClient->addItem(mapInfo.value(key));        
        indexesClientsCombo[indCombo] = key;
        indCombo++;
    }

    //set cases combo box
    mapInfo = docsBE->getCasesForView();
    indCombo = 0;
    for (const auto& key : mapInfo.keys())
    {
        ui->comboBoxCase->addItem(mapInfo.value(key));
        indexesCasesCombo[indCombo] = key;
        indCombo++;
    }
}

void docsView::on_comboBoxClient_activated(int index)
{
    int idClient = indexesClientsCombo.value(index);
    comboChanged = 1;
    emit startUpdateView(m_dv, false, 0, idClient);
    ui->labelOwner->setText(QObject::tr("Документы владельца: Клиент ")
                            + ui->comboBoxClient->currentText());
    //clear preview label
    ui->labelPreview->clear();
}

void docsView::on_comboBoxCase_activated(int index)
{
    int idCase = indexesCasesCombo.value(index);
    comboChanged = 2;
    emit startUpdateView(m_dv, false, idCase, 0);
    ui->labelOwner->setText(QObject::tr("Документы владельца: Дело ")
                            + ui->comboBoxCase->currentText());
    //clear preview label
    ui->labelPreview->clear();
}

void docsView::on_widgetDocs_tabBarClicked(int index)
{
    Q_UNUSED(index);
    if (m_dv == static_cast<DocViews>(index))
    {
        return;
    }
    else
    {
        m_dv = static_cast<DocViews>(index);
    }

    if (comboChanged)
    {
        (this->*arrFunc[comboChanged])(allCombos[comboChanged]->currentIndex());
    }
    else
    {
        on_checkBoxUser_toggled(true);
    }
}

void docsView::on_buttonShowIm_clicked()
{
    QModelIndex ind = ui->viewFoto->currentIndex();
    if (!ind.isValid())
    {
        QMessageBox::warning(nullptr, QObject::tr("Внимание!"),
                             QObject::tr("Не выбрана строка с файлом"));
        return;
    }
    image.loadFromData(docsBE->getFile(ind));
    imageViewer* viewIm = new imageViewer(&image, this);
    viewIm->exec();
}

void docsView::on_buttonPlayAudio_clicked()
{    
    QModelIndex ind = ui->viewAudio->currentIndex();
    if (!ind.isValid())
    {
        QMessageBox::warning(nullptr, QObject::tr("Внимание!"),
                             QObject::tr("Не выбрана строка с файлом"));
        return;
    }
    if (!isLoaded)
    {
        player->setMedia(QMediaContent());
        audioStream->close();
        arrAudio = docsBE->getFile(ind);
        audioStream->setData(arrAudio);
        audioStream->open(QIODevice::ReadOnly);
        player->setMedia(QMediaContent(), audioStream);
        int volume = 70;
        player->setVolume(volume);
        ui->volumeSlider->setValue(volume);
        isLoaded = true;
        ui->viewAudio->selectRow(ind.row());
        player->play();
    }
    else
    {
        if (player->state() == QMediaPlayer::PlayingState)
        {
            player->pause();
        }
        else
        {
            player->play();
        }
    }
}

void docsView::getImageFromScan(QImage scanImage)
{
    image = scanImage;
}

void docsView::on_buttonScan_clicked()
{
    image = QImage();
    scanerView* sView = new scanerView(this);

    connect(sView, &scanerView::giveImageToSave, this, &docsView::getImageFromScan);
    sView->exec();
    if (!image.isNull())
    {
        QByteArray imageArr;
        QBuffer buffer(&imageArr);
        buffer.open(QIODevice::WriteOnly);
        image.save(&buffer, "jpg");

        if (comboChanged == 0)
        {
            emit addScanImage(imageArr, m_dv, true);
        }
        else if (comboChanged == 1)
        {
            int idClient = indexesClientsCombo.value(allCombos[comboChanged]->currentIndex());
            emit addScanImage(imageArr, m_dv, false, 0, idClient);
        }
        else
        {
            int idCase = indexesCasesCombo.value(allCombos[comboChanged]->currentIndex());
            emit addScanImage(imageArr, m_dv, false, idCase, 0);
        }
    }
}

void docsView::on_volumeSlider_valueChanged(int value)
{
    player->setVolume(value);
}

void docsView::on_buttonEdittextDoc_clicked()
{
    QModelIndex ind = ui->viewText->currentIndex();
    if (!ind.isValid())
    {
        QMessageBox::warning(nullptr, QObject::tr("Внимание!"),
                             QObject::tr("Не выбрана строка с файлом"));
        return;
    }
    if (comboChanged == 0)
    {
        docsBE->openTextDoc(ind, true);
    }
    else if (comboChanged == 1)
    {
        int idClient = indexesClientsCombo.value(allCombos[comboChanged]->currentIndex());
        docsBE->openTextDoc(ind, false, 0, idClient);
    }
    else
    {
        int idCase = indexesCasesCombo.value(allCombos[comboChanged]->currentIndex());
        docsBE->openTextDoc(ind, false, idCase, 0);
    }
}

void docsView::on_buttonPrintFoto_clicked()
{
    QModelIndex ind = ui->viewFoto->currentIndex();
    if (!ind.isValid())
    {
        QMessageBox::warning(nullptr, QObject::tr("Внимание!"),
                             QObject::tr("Не выбрана строка с файлом"));
        return;
    }
    image.loadFromData(docsBE->getFile(ind));
    if (!image.isNull())
    {
        QPrinter printer;
        QPrintDialog* dlg = new QPrintDialog(&printer,0);
        if(dlg->exec() == QDialog::Accepted)
        {
            QPainter painter(&printer);
            QRect rect = painter.viewport();
            painter.drawImage(rect, image);
            painter.end();
        }
    }
}

void docsView::on_buttonExit_clicked()
{
    close();
}

void docsView::addAnyDocs()
{
    if (comboChanged == 0)
    {
        emit addDocs(m_dv, true);
    }
    else if (comboChanged == 1)
    {
        int idClient = indexesClientsCombo.value(allCombos[comboChanged]->currentIndex());
        emit addDocs(m_dv, false, 0, idClient);
    }
    else
    {
        int idCase = indexesCasesCombo.value(allCombos[comboChanged]->currentIndex());
        emit addDocs(m_dv, false, idCase, 0);
    }
}

void docsView::on_buttonAddFoto_clicked()
{
    addAnyDocs();
}

void docsView::on_buttonAddAudio_clicked()
{
    addAnyDocs();
}

void docsView::on_buttonAddText_clicked()
{
    addAnyDocs();
}

void docsView::saveAnyDocs(QTableView* view)
{
    QModelIndexList indexes = view->selectionModel()->selectedRows();
    if (indexes.isEmpty())
    {
        QMessageBox::warning(nullptr, QObject::tr("Внимание!"),
                             QObject::tr("Не выбрана строка с файлом"));
        return;
    }
    if (comboChanged == 0)
    {
        docsBE->saveDocs(indexes, m_dv, true);
    }
    else if (comboChanged == 1)
    {
        int idClient = indexesClientsCombo.value(allCombos[comboChanged]->currentIndex());
        docsBE->saveDocs(indexes, m_dv, false, 0, idClient);
    }
    else
    {
        int idCase = indexesCasesCombo.value(allCombos[comboChanged]->currentIndex());
        docsBE->saveDocs(indexes, m_dv, false, idCase, 0);
    }
}

void docsView::on_buttonSaveText_clicked()
{
    saveAnyDocs(ui->viewText);
}

void docsView::on_buttonSaveFoto_clicked()
{
    saveAnyDocs(ui->viewFoto);
}

void docsView::on_buttonSaveAudio_clicked()
{
    saveAnyDocs(ui->viewAudio);
}

void docsView::renameAnyDocs(QTableView* view)
{
    QModelIndex ind = view->currentIndex();
    if (!ind.isValid())
    {
        QMessageBox::warning(nullptr, QObject::tr("Внимание!"),
                             QObject::tr("Не выбрана строка с файлом"));
        return;
    }
    QDialog renDialog;
    QLineEdit renEdit(&renDialog);
    QPushButton buttonRen(&renDialog);
    buttonRen.setText(QObject::tr("Переименовать"));
    buttonRen.setSizePolicy(QSizePolicy
                               (QSizePolicy::Maximum,
                                QSizePolicy::Fixed,
                                QSizePolicy::ToolButton));
    QVBoxLayout vbox;
    vbox.addWidget(&renEdit);
    vbox.addWidget(&buttonRen);

    renDialog.setLayout(&vbox);
    renDialog.setFixedWidth(500);
    renDialog.setWindowTitle(QObject::tr("Введите новое имя файла без расширения..."));

    QString strInput;
    connect(&buttonRen, &QPushButton::clicked, this, [&](){strInput = renEdit.text();});
    connect(&buttonRen, &QPushButton::clicked, &renDialog, &QDialog::close);
    renDialog.exec();

    if (comboChanged == 0)
    {
        docsBE->renameDocs(ind, strInput, m_dv, true);
    }
    else if (comboChanged == 1)
    {
        int idClient = indexesClientsCombo.value(allCombos[comboChanged]->currentIndex());
        docsBE->renameDocs(ind, strInput, m_dv, false, 0, idClient);
    }
    else
    {
        int idCase = indexesCasesCombo.value(allCombos[comboChanged]->currentIndex());
        docsBE->renameDocs(ind, strInput, m_dv, false, idCase, 0);
    }
}

void docsView::on_buttonRenameFoto_clicked()
{
    renameAnyDocs(ui->viewFoto);
}

void docsView::on_buttonRenameAudio_clicked()
{
    renameAnyDocs(ui->viewAudio);
}

void docsView::on_buttonRenameText_clicked()
{
    renameAnyDocs(ui->viewText);
}

void docsView::deleteAnyDocs(QTableView* view)
{
    QModelIndexList indexes = view->selectionModel()->selectedRows();
    if (indexes.isEmpty())
    {
        QMessageBox::warning(nullptr, QObject::tr("Внимание!"),
                             QObject::tr("Не выбрана строка с файлом"));
        return;
    }
    else
    {
        QMessageBox msgBox;
        msgBox.setWindowTitle(QObject::tr("Внимание"));
        msgBox.setInformativeText(QObject::tr("Вы собираетесь удалить документ. Вы уверены?\n"
                                  "Вы можете потерять важную информацию\n"
                                  "Сохраните все нужные файлы!"));
        msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
        msgBox.setDefaultButton(  QMessageBox::No);
        msgBox.setIcon(QMessageBox::Warning);

        int decision = msgBox.exec();
        if (decision == QMessageBox::Yes)
        {
            if (comboChanged == 0)
            {
                docsBE->delDocs(indexes, m_dv, true);
            }
            else if (comboChanged == 1)
            {
                int idClient = indexesClientsCombo.value(allCombos[comboChanged]->currentIndex());
                docsBE->delDocs(indexes, m_dv, false, 0, idClient);
            }
            else
            {
                int idCase = indexesCasesCombo.value(allCombos[comboChanged]->currentIndex());
                docsBE->delDocs(indexes, m_dv, false, idCase, 0);
            }
        }
    }
}

void docsView::on_buttonDelFoto_clicked()
{
    deleteAnyDocs(ui->viewFoto);
}

void docsView::on_buttonDelAudio_clicked()
{
    deleteAnyDocs(ui->viewAudio);
}

void docsView::on_buttonDeleteText_clicked()
{
    deleteAnyDocs(ui->viewText);
}

void docsView::on_buttonSelectAllFoto_clicked()
{
    ui->viewFoto->selectAll();
}

void docsView::on_buttonSelectAllAudio_clicked()
{
    ui->viewAudio->selectAll();
}

void docsView::on_buttonSelectAllText_clicked()
{
    ui->viewText->selectAll();
}

void docsView::on_buttonNewText_clicked()
{
    if (comboChanged == 0)
    {
        docsBE->newTextDoc(true);
    }
    else if (comboChanged == 1)
    {
        int idClient = indexesClientsCombo.value(allCombos[comboChanged]->currentIndex());
        docsBE->newTextDoc(false, 0, idClient);
    }
    else
    {
        int idCase = indexesCasesCombo.value(allCombos[comboChanged]->currentIndex());
        docsBE->newTextDoc(false, idCase, 0);
    }
}

void docsView::startMessageTextWarning()
{
    QMessageBox::warning(nullptr, QObject::tr("Внимание!"),
                         QObject::tr("По завершению редактирования документа сохраните "
                                     "его в том же формате "
                                     "(или в формате Microsoft Word document (.docx) если "
                                     "создается новый файл) в ту же директорию. "
                                     "<b> Не переименовывайте файл и не перемещайте файл!</b> "
                                     "Это можно будет сделать позже."
                                     ));
}

void docsView::endMessageTextWarning()
{
    QMessageBox::warning(nullptr, QObject::tr("Внимание!"),
                         QObject::tr("Нажмите на кнопку <b> OK </b> после завершения редактирования "
                                     "и его сохранения текстовым редактором"
                                     ));
}
