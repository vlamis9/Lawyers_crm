#ifndef CASEINFO_H
#define CASEINFO_H

#include <QObject>
#include <QMap>
#include <QVariant>

class QSqlTableModel;

class caseInfo : public QObject
{
    Q_OBJECT
public:
    explicit caseInfo(QSqlTableModel*, bool, QObject* parent = nullptr);

    QMap<int, QString> getTableConcat();

signals:
    void submitChanges();
    void closeView();
    void updateCase();

public slots:
    void saveData(int);
    void giveInfoFromUser(QMap<QString, QVariant>); //to set all info from user's input
    int getLabelClient(int);

private:
    QSqlTableModel* m_modelCase;
    bool m_newCase; //new or existing case
    QMap<QString, QVariant> infoFromUser;

};

#endif // CASEINFO_H
