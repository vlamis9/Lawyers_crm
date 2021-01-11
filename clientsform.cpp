#include "clientsform.h"
#include <QSqlTableModel>
#include <QSqlQuery>
#include <QSqlRecord>

clientsForm::clientsForm(QObject* parent):
    QObject(parent),
    m_modelFiz(new QSqlTableModel(this)),
    m_modelYur(new QSqlTableModel(this)),
    m_modelMix(new QSqlTableModel(this))
{
    m_modelMix->setTable("mixClients");
}

void clientsForm::deleteClient(int selIndex, Category cat)
{
    int idToDelMix;
    if (cat == Category::FIZ)
    {
        //Client's ID
        idToDelMix = m_modelFiz->record(selIndex).value("ID_FIZ").toInt();
        //delete from table fizClient
        m_modelFiz->removeRow(selIndex);
        m_modelFiz->select();
    }
    else
    {
        //Client's ID
        idToDelMix = m_modelYur->record(selIndex).value("ID_YUR").toInt();
        //delete from table yurClient
        m_modelYur->removeRow(selIndex);
        m_modelYur->select();
    }
    //delete client's documents
    QSqlQuery query;
    query.prepare("DELETE FROM documents "
                  "WHERE ID_DOC IN "
                  "(SELECT ID_DOC FROM clientDocs WHERE ID_CLIENT = :id)");
    query.bindValue(":id", idToDelMix);
    query.exec();

    //delete client's cases and case's documents
    query.prepare("DELETE FROM documents "
                  "WHERE ID_DOC IN "
                  "(SELECT ID_DOC FROM caseDocs WHERE ID_CASE IN "
                  "(SELECT ID_CASE FROM casesTable WHERE ID_CLIENT = :id))");
    query.bindValue(":id", idToDelMix);
    query.exec();

    //delete from table mixClients
    query.prepare("DELETE FROM mixClients "
                  "WHERE ID_MIX = :id");
    query.bindValue(":id", idToDelMix);
    query.exec();
}

QSqlTableModel* clientsForm::getModelFiz()
{
    return m_modelFiz;
}

QSqlTableModel* clientsForm::getModelYur()
{
    return m_modelYur;
}

QSqlTableModel* clientsForm::getModelMix()
{
    return m_modelMix;
}

void clientsForm::searchClientsFiz(QString strUserInput)
{
    QString strSearch;
    strSearch  = "SURNAME     LIKE '%" + strUserInput + "%' ";
    strSearch += "OR NAME     LIKE '%" + strUserInput + "%' ";
    strSearch += "OR MIDDLE   LIKE '%" + strUserInput + "%' ";
    strSearch += "OR BIRTHDAY LIKE '%" + strUserInput + "%' ";
    strSearch += "OR TELM     LIKE '%" + strUserInput + "%' ";
    strSearch += "OR EMAIL    LIKE '%" + strUserInput + "%' ";
    strSearch += "OR STREET   LIKE '%" + strUserInput + "%' ";
    strSearch += "OR NOTES    LIKE '%" + strUserInput + "%' ";

    m_modelFiz->setFilter(strSearch);
}

void clientsForm::searchClientsYur(QString strUserInput)
{
    QString strSearch;
    strSearch  = "OPF         LIKE '%" + strUserInput + "%' ";
    strSearch += "OR COMPANY  LIKE '%" + strUserInput + "%' ";
    strSearch += "OR OGRN     LIKE '%" + strUserInput + "%' ";
    strSearch += "OR INN      LIKE '%" + strUserInput + "%' ";
    strSearch += "OR TEL      LIKE '%" + strUserInput + "%' ";
    strSearch += "OR EMAIL    LIKE '%" + strUserInput + "%' ";
    strSearch += "OR STREET   LIKE '%" + strUserInput + "%' ";
    strSearch += "OR NOTES    LIKE '%" + strUserInput + "%' ";
    m_modelYur->setFilter(strSearch);
}

void clientsForm::startUpdateFizModel()
{
    //set table to model
    m_modelFiz->setTable("fizClient");
    //set viewable headers
    QStringList fizHeaders { QObject::tr("Фамилия"),  QObject::tr("Имя"),
                             QObject::tr("Отчество"), QObject::tr("Дата рожд."),
                             QObject::tr("Телефон"),  QObject::tr("Эл. почта") };
    int ind = 1;
    for (const auto& header : fizHeaders)
    {
        m_modelFiz->setHeaderData(ind, Qt::Horizontal, qPrintable(header));
        ind++;
    }
    //select records from table
    m_modelFiz->select();
    //emit signal to update View
    emit finishUpdateFizModel(m_modelFiz, ind);
}

void clientsForm::startUpdateYurModel()
{
    //set table to model
    m_modelYur->setTable("yurClient");
    //set viewable headers
    QStringList yurHeaders { QObject::tr("ОПФ"),       QObject::tr("Наименование"),
                             QObject::tr("ОГРН"),      QObject::tr("ИНН"),
                             QObject::tr("Телефон"),   QObject::tr("Эл. почта"),
                             QObject::tr("Должность"), QObject::tr("Фамилия"),
                             QObject::tr("Имя"),       QObject::tr("Отчество") };
    int ind = 1;
    for (const auto& header : yurHeaders)
    {
        m_modelYur->setHeaderData(ind, Qt::Horizontal, qPrintable(header));
        ind++;
    }
    //select records from table
    m_modelYur->select();
    //emit signal to update View
    emit finishUpdateYurModel(m_modelYur, ind);
}

