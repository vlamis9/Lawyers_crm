#ifndef CASESMAIN_H
#define CASESMAIN_H

#include <QObject>

class QSqlTableModel;

class casesMain : public QObject
{
    Q_OBJECT
public:
    explicit casesMain(QObject* parent = nullptr);

    QSqlTableModel* getModelCase();
    QSqlTableModel* getModelPay(int);
    void delPayment(QModelIndex, int);
    QString giveNameToDelegate(int);
    bool checkClientExist();
    void deleteCase(int);
    void searchCase(QString);
    void addPayment(int, int, QDate);

signals:
    void finishUpdateCaseModel(QSqlTableModel*, int);

public slots:
    void startUpdateCaseModel();
    void filterModelWithClient(int);

private:
    QSqlTableModel* m_modelCase;
    QSqlTableModel* m_modelPay;
    int count; // counter visible headers
};

#endif // CASESMAIN_H
