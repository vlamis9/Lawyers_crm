#ifndef NEWEVENT_H
#define NEWEVENT_H

#include <QObject>
#include <QMap>
#include <QVariant>

class QSqlTableModel;

class newEvent : public QObject
{
    Q_OBJECT
public:
    explicit newEvent(QObject* parent = nullptr);

    QMap<int, QString> getCasesForView();
    QSqlTableModel* getEventModel();
    void deleteEvent(QModelIndex ind);
    int getIdCase(int);

signals:
    void finishUpdateEventModel(QSqlTableModel*, int);
    void closeView();

public slots:
    void startUpdateEventModel(const QDate&);
    void saveData(QMap<QString, QVariant>, bool);
    void modelSubmit(int, int);

private:
    QSqlTableModel* m_modelEvent;
    int count;
};

#endif // NEWEVENT_H
