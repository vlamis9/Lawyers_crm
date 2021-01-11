#include "eventtrackingview.h"
#include "ui_eventtrackingview.h"
#include "neweventview.h"
#include <QSqlTableModel>
#include "dateformatdelegate.h"
#include "eventcatdelegate.h"
#include <QMessageBox>

eventTrackingView::eventTrackingView(QWidget* parent) :
    QDialog(parent),
    ui(new Ui::eventTrackingView)
{
    ui->setupUi(this);
    this->setWindowTitle(QObject::tr("События"));

    ui->calendarWidget->setVerticalHeaderFormat(QCalendarWidget::NoVerticalHeader);
    ui->calendarWidget->setGridVisible(true);

    eMainBE = new eventTracking(this); //class of logic

    //"signal-slot" connection to add new event on selected date
    connect(ui->calendarWidget, &QCalendarWidget::clicked, this, &eventTrackingView::openNewEvent);

    //"signal-slot" connections for updating Model and View
    connect(this, &eventTrackingView::startUpdateEventView,  eMainBE, &eventTracking::startUpdateEventModel);
    connect(eMainBE, &eventTracking::finishUpdateEventModel, this, &eventTrackingView::finishUpdateEventView);

    connect(ui->calendarWidget, &QCalendarWidget::currentPageChanged, this, &eventTrackingView::updateMonthYearView);

    int year = QDate::currentDate().year();
    QString month = QDate::currentDate().toString("MM");
    emit startUpdateEventView(year, month, isAllEventsToShow);
}

eventTrackingView::~eventTrackingView()
{
    delete ui;
}

void eventTrackingView::openNewEvent(const QDate& date)
{
    newEventView view(date, this);
    view.exec();
}

void eventTrackingView::finishUpdateEventView(QSqlTableModel* modelEvent, int count)
{
    ui->eventsView->setModel(modelEvent);
    ui->eventsView->setSelectionMode(QAbstractItemView::SingleSelection);
    ui->eventsView->setSelectionBehavior(QAbstractItemView::SelectRows);
    //set hidden column "id"
    ui->eventsView->setColumnHidden(0, true);
    //hide other than viewable headers
    for(int i = count; i < modelEvent->columnCount(); i++)
    {
        ui->eventsView->setColumnHidden(i, true);
    }
    //allow to sort items (by "Date"), forbid to edit items
    ui->eventsView->setSortingEnabled(true);
    ui->eventsView->sortByColumn(static_cast<int>(Events::DATE_E), Qt::AscendingOrder);
    ui->eventsView->setEditTriggers(QAbstractItemView::NoEditTriggers);

    //set delegate to show event' category
    int evCat = static_cast<int>(Events::CAT_EVE);
    ui->eventsView->setItemDelegateForColumn(evCat, new eventCatDelegate(this));

    //set delegate to show date column in like-look format
    int caseDate = static_cast<int>(Events::DATE_E);
    ui->eventsView->setItemDelegateForColumn(caseDate, new dateFormatDelegate(this));

    //this allows you to see the data for the entire width of the column (and the item, and header)
    ui->eventsView->resizeColumnsToContents();

    vecDates = eMainBE->getDatesWithEvents();
}

void eventTrackingView::updateMonthYearView(int year, int month)
{
    isAllEventsToShow = false;
    QDate d(year, month, 1);
    QString strMonth = d.toString("MM");
    emit startUpdateEventView(year, strMonth, isAllEventsToShow);
    ui->labelHeaderEvents->setText(QObject::tr("События текущего месяца"));
}

QVector<QDate> eventTrackingView::getDatesForCalendar()
{
    return vecDates;
}

void eventTrackingView::on_buttonAllEvents_clicked()
{
    isAllEventsToShow = true;
    emit startUpdateEventView(0, QString(""), isAllEventsToShow);
    ui->labelHeaderEvents->setText(QObject::tr("Все события"));
}

void eventTrackingView::on_buttonToEventDate_clicked()
{
    if (!(ui->eventsView->selectionModel()->isSelected(ui->eventsView->currentIndex())))
    {
        QMessageBox::warning(nullptr, QObject::tr("Не выбрано событие. "),
                                      QObject::tr("Необходимо выбрать строку с событием "
                                                  ));
        return;
    }
    QDate selEventDate = ui->eventsView->currentIndex().sibling
                            (ui->eventsView->currentIndex().row(), static_cast<int>(Events::DATE_E))
                            .data().toDate();
    ui->calendarWidget->setSelectedDate(selEventDate);
}

void eventTrackingView::on_buttonExit_clicked()
{
    close();
}

void eventTrackingView::on_buttonCurDate_clicked()
{
    ui->calendarWidget->setSelectedDate(QDate::currentDate());
    emit startUpdateEventView(QDate::currentDate().year(), QDate::currentDate().toString("MM"), isAllEventsToShow);
}

void eventTrackingView::on_buttonSearchEvent_clicked()
{
    eMainBE->searchEvent(ui->editSearch->text());
    ui->editSearch->clear();
}
