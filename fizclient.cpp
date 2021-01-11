#include "fizclient.h"
#include "enums.h"
#include "popup.h"
#include <QSqlTableModel>
#include <QSqlRecord>
#include <QSqlQuery>
#include <QDate>

fizClient::fizClient(QSqlTableModel* modelFiz, QSqlTableModel* modelMix, bool newCl, QObject* parent):
    QObject(parent),
    m_modelFiz(modelFiz),
    m_modelMix(modelMix),
    m_newCl(newCl)
{

}

void fizClient::saveData(int curRowIndex)
{
    if(m_newCl)
    {
        if (checkForDuplicates())
        {
            emit duplicatesFound();
            return;
        }
        else
        {
            //save data to mixClients
            QSqlRecord record = m_modelMix->record();
            record.setValue("CLIENT_TYPE", static_cast<int>(Category::FIZ));
            m_modelMix->insertRecord(-1, record);
            //save data to fizClient            
            record = m_modelFiz->record();
            //set ID_FIZ
            QString strID = "SELECT ID_MIX "
                            "FROM mixClients "
                            "WHERE ROWID = last_insert_rowid() ";
            QSqlQuery query(strID);
            query.exec();
            query.first();
            int id = query.value(0).toInt();
            record.setValue("ID_FIZ", id);
            for (const auto& key : infoFromUser.keys())
            {
                record.setValue(key, infoFromUser.value(key));
            }
            m_modelFiz->insertRecord(-1, record);

            PopUp* pop = new PopUp();
            pop->setAttribute(Qt::WA_DeleteOnClose);
            pop->setPopupText(QObject::tr("Клиент добавлен: ") +
                              infoFromUser.value("SURNAME").toString() + " " +
                              infoFromUser.value("NAME").toString()    + " " +
                              infoFromUser.value("MIDDLE").toString()  + " " +
                              infoFromUser.value("BIRTHDAY").toDate().toString(Qt::SystemLocaleShortDate));
            pop->show();
        }
    }
    else
    {
        if (checkForDuplicates())
        {
            emit duplicatesFound();
            return;
        }
        emit submitChanges();

        //set right info from date fields
        m_modelFiz->setData(m_modelFiz->index(curRowIndex, static_cast<int>(FIZ::BIRTHDAY)),
                                              infoFromUser.value("BIRTHDAY"));
        m_modelFiz->setData(m_modelFiz->index(curRowIndex, static_cast<int>(FIZ::WHENIS)),
                                              infoFromUser.value("WHENIS"));
        m_modelFiz->submitAll(); //submit changes
    }
    emit closeView();
}

void fizClient::giveInfoFromUser(QMap<QString, QVariant> info)
{
    infoFromUser = info;
}

bool fizClient::checkForDuplicates()
{    
    //if date was not set by user
    bool bdIsNull = infoFromUser.value("BIRTHDAY").isNull();
    QString strBD;
    if (bdIsNull)
    {
        strBD += "AND BIRTHDAY IS NULL";
    }
    else
        strBD += "AND BIRTHDAY = :birthday";

    QSqlQuery query;
    query.prepare("SELECT * FROM fizClient "
                  "WHERE SURNAME  = :surname "
                  "AND   NAME     = :name "
                  "AND   MIDDLE   = :middle "
                  "AND   PASSER   = :passer "
                  "AND   PASNUM   = :pasnum " +
                  strBD);

    query.bindValue(":surname",  infoFromUser.value("SURNAME"));
    query.bindValue(":name",     infoFromUser.value("NAME"));
    query.bindValue(":middle",   infoFromUser.value("MIDDLE"));
    query.bindValue(":passer",   infoFromUser.value("PASSER"));
    query.bindValue(":pasnum",   infoFromUser.value("PASNUM"));
    if (!bdIsNull) query.bindValue(":birthday", infoFromUser.value("BIRTHDAY"));

    if(!query.exec())
    {
        return false;
    }
    else
    {        
        return query.first(); //false if no duplicates, true if duplicate was found
    }
    return false;
}
