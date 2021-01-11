#include "customcalendarwidget.h"
#include "eventtrackingview.h"
#include <QPainter>

customCalendarWidget::customCalendarWidget(QWidget* parent):
    QCalendarWidget(parent)
{

}

void customCalendarWidget::paintCell(QPainter* painter, const QRect& rect, const QDate& date) const
{
    QVector<QDate> vecDates;
    eventTrackingView* view = qobject_cast<eventTrackingView*>(this->parent());
    vecDates = view->getDatesForCalendar();

    if (date == QDate::currentDate())
    {
        painter->save();
        painter->setRenderHint(QPainter::Antialiasing, true);
        painter->drawEllipse(rect);
        painter->drawText(rect, Qt::TextSingleLine | Qt::AlignCenter, QString::number(date.day()));
        painter->restore();
    }
    else if (vecDates.contains(date))
    {
        painter->drawText(rect, Qt::AlignCenter, QString::number(date.day()));
        painter->fillRect(rect, QColor(225, 65, 110, 80));
    }
    else
    {
        QCalendarWidget::paintCell(painter, rect, date);
    }
}
