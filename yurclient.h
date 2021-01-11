#ifndef YURCLIENT_H
#define YURCLIENT_H

#include <QObject>
#include <QMap>

class QSqlTableModel;

class yurClient : public QObject
{
    Q_OBJECT
public:
    explicit yurClient(QSqlTableModel*, QSqlTableModel*, bool, QObject* parent = nullptr);

signals:
    void duplicatesFound();
    void closeView();
    void submitChanges();
    void updateYur();

public slots:
    void saveData();
    void giveInfoFromUser(QMap<QString, QString>);

private:
    bool checkForDuplicates();

    QSqlTableModel* m_modelYur;
    QSqlTableModel* m_modelMix;
    bool m_newCl; //new or existing client
    QMap<QString, QString> infoFromUser;
};

#endif // YURCLIENT_H
