#include "eventtracking.h"
#include <QSqlTableModel>
#include <QSqlQuery>
#include <QDate>

eventTracking::eventTracking(QObject* parent):
    QObject(parent),
    m_modelEvent(new QSqlTableModel(this))
{

}

void eventTracking::startUpdateEventModel(int year, QString month, bool isAllEventsToShow)
{
    m_modelEvent->setTable("events");
    QStringList caseHeaders { QObject::tr("Дата события"),   QObject::tr("Категория"),
                              QObject::tr("Название"),       QObject::tr("Детали")
                            };
    count = 1;
    for (const auto& header : caseHeaders)
    {
        m_modelEvent->setHeaderData(count, Qt::Horizontal, qPrintable(header));
        count++;
    }
    m_modelEvent->select();
    if (!isAllEventsToShow)
    {
        QString strFilter = QString("(strftime('%Y', DATE_E) = '%1') AND (strftime('%m', DATE_E) = '%2')")
                            .arg(year).arg(month);
        m_modelEvent->setFilter(strFilter);
    }
    emit finishUpdateEventModel(m_modelEvent, count);
}

QSqlTableModel* eventTracking::getModelEvent()
{
    return m_modelEvent;
}

QVector<QDate> eventTracking::getDatesWithEvents()
{
    QVector<QDate> vecDates;
    QSqlQuery query;
    query.prepare("SELECT DATE_E FROM events ");
    query.exec();
    while (query.next())
    {
        vecDates.append(query.value(0).toDate());
    }
    return vecDates;
}

void eventTracking::searchEvent(QString strUserInput)
{
    QString strSearch;
    strSearch  = "ENAME    LIKE '%" + strUserInput + "%' ";
    strSearch += "OR EDET  LIKE '%" + strUserInput + "%' ";
    m_modelEvent->setFilter(strSearch);
}
