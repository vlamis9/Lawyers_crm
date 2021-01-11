#include "yurclient.h"
#include "enums.h"
#include "popup.h"
#include <QSqlTableModel>
#include <QSqlRecord>
#include <QSqlQuery>

yurClient::yurClient(QSqlTableModel* modelYur, QSqlTableModel* modelMix, bool newCl, QObject* parent):
    QObject(parent),
    m_modelYur(modelYur),
    m_modelMix(modelMix),
    m_newCl(newCl)
{

}

void yurClient::saveData()
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
            record.setValue("CLIENT_TYPE", static_cast<int>(Category::YUR));
            m_modelMix->insertRecord(-1, record);
            //save data to yurClient
            record = m_modelYur->record();
            //set ID_FIZ
            QString strID = "SELECT ID_MIX "
                            "FROM mixClients "
                            "WHERE ROWID = last_insert_rowid() ";
            QSqlQuery query(strID);            
            query.exec();
            query.first();
            int id = query.value(0).toInt();
            record.setValue("ID_YUR", id);

            for (const auto& key : infoFromUser.keys())
            {
                record.setValue(key, infoFromUser.value(key));
            }
            m_modelYur->insertRecord(-1, record);

            PopUp* pop = new PopUp();
            pop->setAttribute(Qt::WA_DeleteOnClose);
            pop->setPopupText(QObject::tr("Клиент добавлен: ") +
                              infoFromUser.value("OPF") + " " +
                              infoFromUser.value("COMPANY")    + " " +
                              infoFromUser.value("OGRN")  + " " +
                              infoFromUser.value("INN"));
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
    }
    emit closeView();
}

void yurClient::giveInfoFromUser(QMap<QString, QString> info)
{
    infoFromUser = info;
}

bool yurClient::checkForDuplicates()
{
    //check OGRN or INN because it's unique numbers
    QString str;
    QSqlQuery query;

    if(infoFromUser.value("OGRN").isEmpty())
    {
        str = "INN = :inn";
        query.prepare("SELECT ID_YUR FROM yurClient "
                      "WHERE " + str);
        query.bindValue(":inn", infoFromUser.value("INN"));
    }
    else if(infoFromUser.value("INN").isEmpty())
    {
        str = "OGRN = :ogrn";
        query.prepare("SELECT ID_YUR FROM yurClient "
                      "WHERE " + str);
        query.bindValue(":ogrn", infoFromUser.value("OGRN"));
    }
    else
    {
        str = "OGRN = :ogrn OR INN = :inn";
        query.prepare("SELECT ID_YUR FROM yurClient "
                      "WHERE " + str);
        query.bindValue(":ogrn", infoFromUser.value("OGRN"));
        query.bindValue(":inn",  infoFromUser.value("INN"));
    }
    if(!query.exec())
    {
        return false;
    }
    else
    {
        return query.first();
    }
    return false;
}


