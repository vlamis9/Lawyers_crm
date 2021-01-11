#include "yurclientview.h"
#include "ui_yurclientview.h"
#include "clientsformview.h"
#include <QDataWidgetMapper>
#include <QSqlTableModel>
#include <QTableView>
#include <QMessageBox>

yurClientView::yurClientView(QSqlTableModel* modelYur, QSqlTableModel* modelMix, QTableView* yurView, bool newCl, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::yurClientView),
    m_newCl(newCl),
    m_mapper(new QDataWidgetMapper(this)),
    m_yurView(yurView)
{
    ui->setupUi(this);

    ui->lineCompany->setFocus(); //set focus to Company name field first

    yClientBE = new yurClient(modelYur, modelMix, m_newCl, this);

    //"signal-slot" connection for save data in database by class of logic
    connect(this, &yurClientView::saveData, yClientBE, &yurClient::saveData);

    //"signal-slot" connection for sending-giving User's input data and check it
    connect(this, &yurClientView::sendInfoFromUser, yClientBE, &yurClient::giveInfoFromUser);

    //"signal-slot" connection for create error meassage when duplicates is found
    connect(yClientBE, &yurClient::duplicatesFound, this, &yurClientView::errMsgDuplicate);

    //"signal-slot" connection for close View window after DB was changed
    connect(yClientBE, &yurClient::closeView, this, &QDialog::close);

    //"signal-slot" connection for submit changes in DB
    connect(yClientBE, &yurClient::submitChanges, this, &yurClientView::submitChanges);

    //"signal-slot" connection for update fizClient View
    connect(yClientBE, &yurClient::updateYur, this, &yurClientView::updateYur);

    setMapper(modelYur);
}

yurClientView::~yurClientView()
{
    delete ui;
}

void yurClientView::errMsgDuplicate()
{
    QMessageBox::warning(nullptr, QObject::tr("Ошибка"),
                         QObject::tr("Клиент уже существует в базе."
                                     "<BR><BR>"
                                     "Если вы уверены в необходимости добавления клиента, "
                                     "отредактируйте информацию в записи "
                                     "содержащей совпадение данных"
                                     "<BR><BR>"
                                     "Вы можете воспользоваться кнопкой"
                                     "<b> \"Поиск\"</b>"));
}

void yurClientView::submitChanges()
{
    m_mapper->submit();
}

void yurClientView::updateYur()
{
    clientsFormView* form = qobject_cast<clientsFormView*>(this->parent());
    emit form->startUpdateYurView();
}

void yurClientView::on_buttonSaveYur_clicked()
{
    //check if "ogrn" or "inn" not set
    if (checkIfEmpty())
    {
        QMessageBox::warning(nullptr, QObject::tr("Ошибка"),
                             QObject::tr("Необходимо указать один из реквизитов:"
                                         "<BR><BR>"
                                         "<b>\"ОГРН\"</b>"
                                         "<BR><BR>или"
                                         "<BR><BR>"
                                         "<b>\"ИНН\"</b>"));
        return;
    }
    else
    {
        emit sendInfoFromUser(QMap<QString, QString> {
                              {"OPF",      ui->lineOpf->text()},
                              {"COMPANY",  ui->lineCompany->text()},
                              {"OGRN",     ui->lineOgrn->text()},
                              {"INN",      ui->lineInn->text()},
                              {"TEL",      ui->lineTel->text()},
                              {"EMAIL",    ui->lineEmail->text()},
                              {"DELEGATE", ui->lineDel->text()},
                              {"SURDEL",   ui->lineSurdel->text()},
                              {"NAMEDEL",  ui->lineNamedel->text()},
                              {"MIDDEL",   ui->lineMiddel->text()},
                              {"IND",      ui->lineInd->text()},
                              {"CITY",     ui->lineCity->text()},
                              {"STREET",   ui->lineStreet->text()},
                              {"BLD",      ui->lineBld->text()},
                              {"CORP",     ui->lineCorp->text()},
                              {"ROOM",     ui->lineRoom->text()},
                              {"DOC",      ui->lineDoc->text()},
                              {"REQS",     ui->lineReqs->text()},
                              {"BSURNAME", ui->lineBsur->text()},
                              {"BNAME",    ui->lineBname->text()},
                              {"BMIDDLE",  ui->lineBmid->text()},
                              {"NOTES",    ui->textNotes->toPlainText()}
                             });
        emit saveData();
    }
}

void yurClientView::on_buttonCancelYur_clicked()
{
    close();
}

void yurClientView::setMapper(QSqlTableModel* modelYur)
{
    m_mapper->setModel(modelYur);
    m_mapper->setSubmitPolicy(QDataWidgetMapper::ManualSubmit);

    m_mapper->addMapping(ui->lineOpf,     static_cast<int>(YUR::OPF));
    m_mapper->addMapping(ui->lineCompany, static_cast<int>(YUR::COMPANY));
    m_mapper->addMapping(ui->lineOgrn,    static_cast<int>(YUR::OGRN));
    m_mapper->addMapping(ui->lineInn,     static_cast<int>(YUR::INN));
    m_mapper->addMapping(ui->lineTel,     static_cast<int>(YUR::TEL));
    m_mapper->addMapping(ui->lineEmail,   static_cast<int>(YUR::EMAIL));
    m_mapper->addMapping(ui->lineDel,     static_cast<int>(YUR::DELEGATE));
    m_mapper->addMapping(ui->lineSurdel,  static_cast<int>(YUR::SURDEL));
    m_mapper->addMapping(ui->lineNamedel, static_cast<int>(YUR::NAMEDEL));
    m_mapper->addMapping(ui->lineMiddel,  static_cast<int>(YUR::MIDDEL));
    m_mapper->addMapping(ui->lineInd,     static_cast<int>(YUR::IND));
    m_mapper->addMapping(ui->lineCity,    static_cast<int>(YUR::CITY));
    m_mapper->addMapping(ui->lineStreet,  static_cast<int>(YUR::STREET));
    m_mapper->addMapping(ui->lineBld,     static_cast<int>(YUR::BLD));
    m_mapper->addMapping(ui->lineCorp,    static_cast<int>(YUR::CORP));
    m_mapper->addMapping(ui->lineRoom,    static_cast<int>(YUR::ROOM));
    m_mapper->addMapping(ui->lineDoc,     static_cast<int>(YUR::DOC));
    m_mapper->addMapping(ui->lineReqs,    static_cast<int>(YUR::REQS));
    m_mapper->addMapping(ui->lineBsur,    static_cast<int>(YUR::BSURNAME));
    m_mapper->addMapping(ui->lineBname,   static_cast<int>(YUR::BNAME));
    m_mapper->addMapping(ui->lineBmid,    static_cast<int>(YUR::BMIDDLE));
    m_mapper->addMapping(ui->textNotes,   static_cast<int>(YUR::NOTES));

    if (m_newCl)
    {
        setWindowTitle(QObject::tr("Добавление нового клиента"));
    }
    else
    {
        setWindowTitle(QObject::tr("Просмотр и редактирование информации о клиенте"));
        m_mapper->setCurrentIndex(m_yurView->currentIndex().row());
    }
}

bool yurClientView::checkIfEmpty()
{
    bool ogrn = ui->lineOgrn->text().isEmpty();
    bool inn  = ui->lineInn->text().isEmpty();
    return ogrn ? inn ? true : false: false;
}
