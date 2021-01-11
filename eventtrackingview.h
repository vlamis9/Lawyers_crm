#ifndef EVENTTRACKINGVIEW_H
#define EVENTTRACKINGVIEW_H

#include <QDialog>
#include "eventtracking.h"

namespace Ui {
class eventTrackingView;
}

class eventTrackingView : public QDialog
{
    Q_OBJECT

public:
    explicit eventTrackingView(QWidget* parent = nullptr);
    ~eventTrackingView();

    QVector<QDate> getDatesForCalendar();

signals:
    void startUpdateEventView(int, QString, bool);

private slots:
    void openNewEvent(const QDate&); // opens dialog to add new event on selected date
    void finishUpdateEventView(QSqlTableModel*, int);
    //update events on View when month or year was changed
    void updateMonthYearView(int, int);
    void on_buttonAllEvents_clicked();
    void on_buttonToEventDate_clicked();
    void on_buttonExit_clicked();
    void on_buttonCurDate_clicked();
    void on_buttonSearchEvent_clicked();

private:
    Ui::eventTrackingView* ui;
    eventTracking* eMainBE; //pointer to class of logic
    QVector<QDate> vecDates; //store all saved dates with events
    bool isAllEventsToShow {false}; //to show all events
};

#endif // EVENTTRACKINGVIEW_H
