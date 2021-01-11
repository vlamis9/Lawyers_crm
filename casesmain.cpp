#include "casesmain.h"

#include <QSqlTableModel>
#include <QSqlQuery>
#include <QSqlRecord>
#include <QDate>

casesMain::casesMain(QObject* parent):
    QObject(parent),
    m_modelCase(new QSqlTableModel(this)),
    m_modelPay(new QSqlTableModel(this))
{

}

QSqlTableModel* casesMain::getModelCase()
{
    return m_modelCase;
}

QSqlTableModel* casesMain::getModelPay(int idCase)
{
    //set table to model
    m_modelPay->setTable("payments");

    QStringList payHeaders {QObject::tr("Дата платежа"), QObject::tr("Сумма платежа")};

    int count = 2;
    for (const auto& header : payHeaders)
    {
        m_modelPay->setHeaderData(count, Qt::Horizontal, qPrintable(header));
        count++;
    }

    m_modelPay->select();

    //select records from table by idCase
    m_modelPay->setFilter(QString("ID_CASE = %1").arg(idCase));

    return m_modelPay;
}

void casesMain::delPayment(QModelIndex idIndex, int idCase)
{
    m_modelPay = getModelPay(idCase);
    m_modelPay->removeRow(idIndex.row());
}

QString casesMain::giveNameToDelegate(int id)
{
    QSqlQuery query;
    query.prepare("SELECT CLIENT_TYPE FROM mixClients WHERE ID_MIX = :id");
    query.bindValue(":id", id);
    query.exec();
    query.first();
    int cat = query.value(0).toInt();

    if (cat) //if 1 -> yur client
    {
        query.prepare("SELECT (OPF || ' ' || COMPANY || ' ' || OGRN || ' ' || INN) "
                      "FROM yurClient WHERE ID_YUR = :id");
    }
    else
    {
        query.prepare("SELECT (SURNAME || ' ' || NAME || ' ' || MIDDLE || ' ' || COALESCE(BIRTHDAY, ' ') )"
                      "FROM fizClient WHERE ID_FIZ = :id");
    }
    query.bindValue(":id", id);
    query.exec();
    query.first();
    return query.value(0).toString();
}

bool casesMain::checkClientExist()
{
    QSqlQuery query;
    query.prepare("SELECT count(*) FROM mixClients");
    query.exec();
    query.first();
    return query.value(0).toBool();
}

void casesMain::deleteCase(int selInd)
{
    //get case's ID
    int idToDelCase = m_modelCase->record(selInd).value("ID_CASE").toInt();

    //delete all case's documents
    QSqlQuery query;
    query.prepare("DELETE FROM documents "
                  "WHERE ID_DOC IN "
                  "(SELECT ID_DOC FROM caseDocs WHERE ID_CASE = :id)");
    query.bindValue(":id", idToDelCase);
    query.exec();

    //delete from caseTable
    m_modelCase->removeRow(selInd);
    m_modelCase->select();
}

void casesMain::searchCase(QString strUserInput)
{
    QString strSearch;
    strSearch  = "MAINPOINT      LIKE '%" + strUserInput + "%' ";
    strSearch += "OR RULESOFLAW  LIKE '%" + strUserInput + "%' ";
    strSearch += "OR CASEPARTS   LIKE '%" + strUserInput + "%' ";
    strSearch += "OR CONTRACTNUM LIKE '%" + strUserInput + "%' ";
    strSearch += "OR CONTRACTSUM LIKE '%" + strUserInput + "%' ";
    strSearch += "OR CASEDATE    LIKE '%" + strUserInput + "%' ";
    strSearch += "OR NOTES       LIKE '%" + strUserInput + "%' ";

    m_modelCase->setFilter(strSearch);
}

void casesMain::addPayment(int idCase, int sum, QDate date)
{
    QSqlQuery query;
    query.prepare("INSERT INTO payments (DATE_PAY, ID_CASE, SUM) VALUES (:date, :id, :sum);");
    query.bindValue(":date", date);
    query.bindValue(":id",   idCase);
    query.bindValue(":sum",  sum);
    query.exec();
}

void casesMain::startUpdateCaseModel()
{
    //set table to model
    m_modelCase->setTable("casesTable");

    QStringList caseHeaders { QObject::tr("Клиент"),         QObject::tr("Статус кл."),
                              QObject::tr("Кат. дела"),      QObject::tr("Дата дела"),
                              QObject::tr("Номер Договора"), QObject::tr("Сумма дог."),
                              QObject::tr("Суть дела"),      QObject::tr("Участники дела"),
                            };

    count = 1;
    for (const auto& header : caseHeaders)
    {
        m_modelCase->setHeaderData(count, Qt::Horizontal, qPrintable(header));
        count++;
    }

    //select records from table
    m_modelCase->select();

    emit finishUpdateCaseModel(m_modelCase, count);
}

void casesMain::filterModelWithClient(int idClient)
{
    m_modelCase->setFilter("ID_CLIENT = " + QString::number(idClient));

    //emit signal to update View
    emit finishUpdateCaseModel(m_modelCase, count);
}

