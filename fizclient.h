#ifndef FIZCLIENT_H
#define FIZCLIENT_H

#include <QObject>
#include <QMap>
#include <QVariant>

class QSqlTableModel;

class fizClient : public QObject
{
    Q_OBJECT
public:
    explicit fizClient(QSqlTableModel*, QSqlTableModel*, bool, QObject* parent = nullptr);

signals:
    void duplicatesFound();
    void closeView();
    void submitChanges();
    void updateFiz();

public slots:
    void saveData(int);
    void giveInfoFromUser(QMap<QString, QVariant>);

private:
    bool checkForDuplicates();

    QSqlTableModel* m_modelFiz;
    QSqlTableModel* m_modelMix;
    bool m_newCl; //new or existing client
    QMap<QString, QVariant> infoFromUser;
};

#endif // FIZCLIENT_H
