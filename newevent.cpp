#include "newevent.h"
#include "enums.h"
#include <QSqlTableModel>
#include <QSqlQuery>
#include <QSqlRecord>
#include <QDate>

newEvent::newEvent(QObject* parent):
    QObject(parent),
    m_modelEvent(new QSqlTableModel(this))
{
    m_modelEvent->setTable("events");
}

QMap<int, QString> newEvent::getCasesForView()
{
    QSqlQuery query;
    query.prepare("SELECT (ID_CASE || ' ' || CASECAT || ' ' || MAINPOINT || ' ' "
                  "|| CONTRACTNUM  || ' ' || COALESCE(CASEDATE, '') ) "
                  "FROM casesTable ");
    query.exec();
    QMap<int, QString> mapInfo;
    while (query.next())
    {
        QString strIdToDelete = query.value(0).toString().section(" ", 0, 0, QString::SectionSkipEmpty);

        int index = strIdToDelete.toInt();
        //remove only ID, not other same strings
        mapInfo[index] = query.value(0).toString().remove(0, strIdToDelete.size());
        mapInfo[index] = mapInfo[index].trimmed();
        //get date
        QVariant strDate = query.value(0).toString().right(10);
        QDate d = strDate.toDate();
        //set date to ok format
        QString strFormattedDate = d.toString(Qt::SystemLocaleShortDate);
        //change date in map
        mapInfo[index].remove(query.value(0).toString().right(10));
        mapInfo[index].append(strFormattedDate);
        //set case category to string format
        strIdToDelete = mapInfo[index].section(" ", 0, 0, QString::SectionSkipEmpty);
        mapInfo[index].replace(0, strIdToDelete.size(), caseCatArr[strIdToDelete.toInt()]);
    }
    return mapInfo;
}

QSqlTableModel *newEvent::getEventModel()
{
    return  m_modelEvent;
}

void newEvent::deleteEvent(QModelIndex ind)
{
    //delete from events with cascade delete from other link tables
    m_modelEvent->removeRow(ind.row());
}

int newEvent::getIdCase(int idEve)
{
    QSqlQuery query;
    query.prepare("SELECT ID_CASE FROM caseEvents WHERE ID_EVE = :id");
    query.bindValue(":id", idEve);
    query.exec();
    query.first();
    return query.value(0).toInt();
}

void newEvent::startUpdateEventModel(const QDate& date)
{
    m_modelEvent->setTable("events");

    QStringList caseHeaders { QObject::tr("Категория"), QObject::tr("Название"),
                              QObject::tr("Детали")
                            };
    count = static_cast<int>(Events::CAT_EVE);
    for (const auto& header : caseHeaders)
    {
        m_modelEvent->setHeaderData(count, Qt::Horizontal, qPrintable(header));
        count++;
    }
    m_modelEvent->select();

    //set filter event for selected date
    QString d = date.toString("yyyy-MM-dd");
    m_modelEvent->setFilter("DATE_E = '" + d + "' ");
    emit finishUpdateEventModel(m_modelEvent, count);
}

void newEvent::saveData(QMap<QString, QVariant> infoFromUser, bool isUser)
{
    m_modelEvent->setTable("events");
    QSqlTableModel* modelCategory = new QSqlTableModel(this);
    isUser ? modelCategory->setTable("userEvents") : modelCategory->setTable("caseEvents");
    int owner;
    isUser ? owner = static_cast<int>(OwnerCat::USER) : owner = static_cast<int>(OwnerCat::CASE);

    QSqlRecord record = m_modelEvent->record();
    for (const auto& key : infoFromUser.keys())
    {
        record.setValue(key, infoFromUser.value(key));
    }
    record.setValue("CAT_OWNER", owner);
    m_modelEvent->insertRecord(-1, record);

    //to owner's table
    record = modelCategory->record();
    QString strID = "SELECT ID_EVE "
                    "FROM events "
                    "WHERE ROWID = last_insert_rowid() ";
    QSqlQuery query(strID);
    query.exec();
    query.first();
    int id = query.value(0).toInt();
    record.setValue("ID_EVE", id);
    if (!isUser)
    {
        record.setValue("ID_CASE", infoFromUser.value("ID_CASE"));
    }
    modelCategory->insertRecord(-1, record);
    startUpdateEventModel(infoFromUser.value("DATE_E").toDate());
}

void newEvent::modelSubmit(int curRowInd, int catEve)
{
    int catEveCol = static_cast<int>(Events::CAT_EVE);
    m_modelEvent->setData(m_modelEvent->index(curRowInd, catEveCol), catEve);
    m_modelEvent->submitAll();
    emit finishUpdateEventModel(m_modelEvent, count);
}
