#include "neweventview.h"
#include "ui_neweventview.h"
#include "eventtrackingview.h"
#include "eventcatdelegate.h"
#include <QDate>
#include <QDataWidgetMapper>
#include <QSqlTableModel>
#include <QMessageBox>

newEventView::newEventView(const QDate& date, QWidget* parent):
    QDialog(parent),
    ui(new Ui::newEventView),
    m_date(date),    
    m_mapper(new QDataWidgetMapper(this)),
    m_isUser(true)
{
    ui->setupUi(this);
    this->setWindowTitle(QObject::tr("Событие"));

    ui->labelSelDate->setText(QDate(date).toString());

    neBE = new newEvent(this);
    initInfo();

    //"signal-slot" connections for updating Model and View for selected date
    connect(this, &newEventView::startUpdateDateEvView,  neBE, &newEvent::startUpdateEventModel);
    connect(neBE, &newEvent::finishUpdateEventModel, this, &newEventView::finishUpdateDateEvView);

    //"signal-slot" connection for save data in database by class of logic
    connect(this, &newEventView::saveData, neBE, &newEvent::saveData);

    connect(ui->dateEventView, &QTableView::clicked, this, &newEventView::setMapperIndex);

    //"signal-slot" connection for close View window after DB was changed
    connect(neBE, &newEvent::closeView, this, &QDialog::close);

    emit startUpdateDateEvView(m_date);
    setMapper(neBE->getEventModel());
    ui->buttonChange->setEnabled(false);
}

newEventView::~newEventView()
{
    delete ui;
}

void newEventView::initInfo()
{
    for (const QString& str : eventCatArr)
    {
        ui->comboCatEvent->addItem(str);
    }
    //set cases combo box
    QMap<int, QString> mapInfo = neBE->getCasesForView();
    int indCombo = 0;
    for (const auto& key : mapInfo.keys())
    {
        ui->comboCases->addItem(mapInfo.value(key));
        indexesCasesCombo[indCombo] = key;
        indCombo++;
    }
    ui->checkBoxUser->setChecked(true);
    if (indexesCasesCombo.isEmpty())
    {
        ui->checkBoxCase->setEnabled(false);
    }
}

void newEventView::setMapper(QSqlTableModel* modelEvents)
{
    m_mapper->setModel(modelEvents);
    m_mapper->setSubmitPolicy(QDataWidgetMapper::ManualSubmit);
    m_mapper->addMapping(ui->editNameEvent,    static_cast<int>(Events::ENAME));
    m_mapper->addMapping(ui->editDetailEvent,  static_cast<int>(Events::EDET));
    m_mapper->addMapping(ui->comboCatEvent,    static_cast<int>(Events::CAT_EVE));
    m_mapper->addMapping(ui->spinNumDays,      static_cast<int>(Events::NUMD));
}

bool newEventView::checkSpinValue()
{
    int days = QDate::currentDate().daysTo(m_date);
    if (days < 0) days = - days;
    return (days >= ui->spinNumDays->value());
}

void newEventView::on_checkBoxUser_toggled(bool checked)
{
    if (checked)
    {
        ui->comboCases->setEnabled(false);
        ui->checkBoxCase->setChecked(false);
        m_isUser = true;
    }
}

void newEventView::on_checkBoxCase_toggled(bool checked)
{
    if (checked)
    {
        ui->comboCases->setEnabled(true);
        ui->checkBoxUser->setChecked(false);
        m_isUser = false;
    }
}

void newEventView::finishUpdateDateEvView(QSqlTableModel* modelEvent, int count)
{
    ui->dateEventView->setModel(modelEvent);

    ui->dateEventView->setSelectionMode(QAbstractItemView::SingleSelection);
    ui->dateEventView->setSelectionBehavior(QAbstractItemView::SelectRows);

    //set hidden column's
    ui->dateEventView->setColumnHidden(static_cast<int>(Events::ID_EVE), true);
    ui->dateEventView->setColumnHidden(static_cast<int>(Events::DATE_E), true);
    //hide other than viewable headers
    for(int i = count; i < modelEvent->columnCount(); i++)
    {
        ui->dateEventView->setColumnHidden(i, true);
    }

    //allow to sort items (by "Date"), forbid to edit items
    ui->dateEventView->setSortingEnabled(true);
    ui->dateEventView->sortByColumn(1, Qt::AscendingOrder);
    ui->dateEventView->setEditTriggers(QAbstractItemView::NoEditTriggers);

    //set delegate to show event' category
    int evCat = static_cast<int>(Events::CAT_EVE);
    ui->dateEventView->setItemDelegateForColumn(evCat, new eventCatDelegate(this));

    //this allows you to see the data for the entire width of the column (and the item, and header)
    ui->dateEventView->resizeColumnsToContents();

    //update eventTrackingView main View
    eventTrackingView* view = qobject_cast<eventTrackingView*>(this->parent());
    int year = m_date.year();
    QString month = m_date.toString("MM");
    view->startUpdateEventView(year, month, false);
}

void newEventView::on_buttonSave_clicked()
{
    if (!checkSpinValue())
    {
        QMessageBox::warning(0,"Ошибка!", "Событие состоится через "
                             + QString::number(QDate::currentDate().daysTo(m_date)) +
                             " дней, однако выбрано уведомить через "
                             + QString::number(ui->spinNumDays->value()) +
                             " дней. Укажите верное число.");
        return;
    }
    QMap<QString, QVariant> infoFromUser{
                                        {"DATE_E",   m_date},
                                        {"CAT_EVE",  ui->comboCatEvent->currentIndex()},
                                        {"ENAME",    ui->editNameEvent->text()},
                                        {"EDET",     ui->editDetailEvent->toPlainText()},
                                        {"DATE_CR",  QDate::currentDate()},
                                        {"NUMD",     ui->spinNumDays->value()},
                                        {"ID_CASE",  indexesCasesCombo.value(ui->comboCases->currentIndex())}
                                        };
    emit saveData(infoFromUser, m_isUser);
    //clear all forms
    on_buttonCancel_clicked();
}

void newEventView::setMapperIndex(const QModelIndex& qInd)
{
    ui->buttonChange->setEnabled(true);
    ui->buttonSave->setEnabled(false);
    m_mapper->setCurrentIndex(qInd.row());
    //get case' category
    int eventCat = qInd.sibling(qInd.row(), static_cast<int>(Events::CAT_EVE)).data().toInt();
    //set combo box with case categories
    ui->comboCatEvent->setCurrentIndex(eventCat);

    int catOwner = qInd.sibling(qInd.row(), static_cast<int>(Events::CAT_OWNER)).data().toInt();
    if (catOwner == static_cast<int>(OwnerCat::CASE))
    {
        int idEve = qInd.sibling(qInd.row(), static_cast<int>(Events::ID_EVE)).data().toInt();
        int idCase = neBE->getIdCase(idEve);
        ui->checkBoxCase->setChecked(true);
        ui->comboCases->setCurrentIndex(indexesCasesCombo.key(idCase));
        ui->comboCases->setEnabled(false);
        ui->checkBoxUser->setEnabled(false);
    }
}

void newEventView::on_buttonChange_clicked()
{
    if (!checkSpinValue())
    {
        QMessageBox::warning(0,"Ошибка!", "Событие состоится через "
                             + QString::number(QDate::currentDate().daysTo(m_date)) +
                             " дней, однако выбрано уведомить через "
                             + QString::number(ui->spinNumDays->value()) +
                             " дней. Укажите верное число.");
        return;
    }

    m_mapper->submit();
    connect(this, &newEventView::modelSubmit, neBE, &newEvent::modelSubmit);
    int curRowInd = ui->dateEventView->currentIndex().row();
    int catEve = ui->comboCatEvent->currentIndex();
    emit modelSubmit(curRowInd, catEve);
    //clear all forms
    on_buttonCancel_clicked();
}

void newEventView::on_buttonCancel_clicked()
{
    //clear TableView selection and all edit forms
    ui->dateEventView->clearSelection();
    ui->checkBoxUser->toggled(true);
    ui->comboCases->setEnabled(false);
    ui->comboCatEvent->setCurrentIndex(0);
    ui->editNameEvent->clear();
    ui->editDetailEvent->clear();
    ui->spinNumDays->setValue(0);
    ui->buttonChange->setEnabled(false);
    ui->buttonSave->setEnabled(true);
}

void newEventView::on_buttonDelEvent_clicked()
{
    if (!(ui->dateEventView->selectionModel()->isSelected(ui->dateEventView->currentIndex())))
    {
        QMessageBox::warning(nullptr, QObject::tr("Не выбрано событие. "),
                                      QObject::tr("Необходимо выбрать строку с событием "));
        return;
    }
    else
    {
        QMessageBox msgBox;
        msgBox.setWindowTitle(QObject::tr("Внимание"));
        msgBox.setInformativeText(QObject::tr("Вы собираетесь удалить событие. Вы уверены?\n"));
        msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
        msgBox.setDefaultButton(  QMessageBox::No);
        msgBox.setIcon(QMessageBox::Warning);
        int decision = msgBox.exec();
        if (decision == QMessageBox::Yes)
        {
            neBE->deleteEvent(ui->dateEventView->currentIndex());
            emit startUpdateDateEvView(m_date);
        }
    }
}
