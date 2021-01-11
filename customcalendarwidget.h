#ifndef CUSTOMCALENDARWIDGET_H
#define CUSTOMCALENDARWIDGET_H

#include <QObject>
#include <QCalendarWidget>

class customCalendarWidget : public QCalendarWidget
{
    Q_OBJECT
public:
    explicit customCalendarWidget(QWidget* parent = nullptr);

protected:
    void paintCell(QPainter*, const QRect&, const QDate&) const;
};

#endif // CUSTOMCALENDARWIDGET_H
