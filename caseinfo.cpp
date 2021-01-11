#include "caseinfo.h"
#include "enums.h"
#include "popup.h"

#include <QSqlRecord>
#include <QSqlQuery>
#include <QSqlTableModel>
#include <QDate>

caseInfo::caseInfo(QSqlTableModel* modelCase, bool newCase, QObject* parent):
    QObject(parent),
    m_modelCase(modelCase),
    m_newCase(newCase)
{

}

QMap<int, QString> caseInfo::getTableConcat()
{
    QSqlQuery query;
    query.prepare("SELECT (ID_FIZ || ' ' || SURNAME || ' ' || NAME || ' ' || MIDDLE || ' ' || COALESCE(BIRTHDAY, '') ) "
                  "FROM fizClient "
                  "UNION "
                  "SELECT (ID_YUR || ' ' || OPF || ' ' || COMPANY || ' ' || OGRN || ' ' || INN) "
                  "FROM yurClient;");
    query.exec();

    QMap<int, QString> mapInfo;
    while (query.next())
    {
        QString strIdToDelete = query.value(0).toString().section(" ", 0, 0, QString::SectionSkipEmpty);
        QVariant strDate = query.value(0).toString().right(10);
        QDate d = strDate.toDate();
        QString strFormattedDate;

        if (d.isValid())
        {           
            strFormattedDate = d.toString(Qt::SystemLocaleShortDate);
            int index = strIdToDelete.toInt();
            mapInfo[index] = query.value(0).toString().remove(0, strIdToDelete.size());
            mapInfo[index] = mapInfo[index].trimmed();
            //change date in map
            mapInfo[index].remove(query.value(0).toString().right(10));
            mapInfo[index].append(strFormattedDate);
        }
        else
        {            
            int index = strIdToDelete.toInt();
            mapInfo[index] = query.value(0).toString().remove(0, strIdToDelete.size());
            mapInfo[index] = mapInfo[index].trimmed();
        }
    }
    return mapInfo;
}

void caseInfo::saveData(int curRowIndex)
{
    if(m_newCase)
    {
        QSqlRecord record = m_modelCase->record();
        for (const auto& key : infoFromUser.keys())
        {
            if (key == "CLIENTNAME") continue;
            record.setValue(key, infoFromUser.value(key));
        }
        m_modelCase->insertRecord(-1, record);

        PopUp* pop = new PopUp();
        pop->setAttribute(Qt::WA_DeleteOnClose);
        pop->setPopupText(QObject::tr("Дело успешно создано\n Клиент: ") +
                          infoFromUser.value("CLIENTNAME").toString());
        pop->show();
    }
    else
    {
        emit submitChanges();
        //set case category and client status
        int caseCatCol = static_cast<int>(CASE::CASECAT);
        m_modelCase->setData(m_modelCase->index(curRowIndex, caseCatCol), infoFromUser.value("CASECAT"));

        int clStCol = static_cast<int>(CASE::CLIENTSTATUS);
        m_modelCase->setData(m_modelCase->index(curRowIndex, clStCol), infoFromUser.value("CLIENTSTATUS"));
        m_modelCase->submitAll();
    }
    emit closeView();
}

void caseInfo::giveInfoFromUser(QMap<QString, QVariant> info)
{
    infoFromUser = info;
}

int caseInfo::getLabelClient(int ind)
{
    QSqlQuery query;
    query.prepare("SELECT CLIENT_TYPE FROM mixClients WHERE ID_MIX = :id");
    query.bindValue(":id" , ind);
    query.exec();
    query.first();
    return query.value(0).toInt();
}
