#ifndef EVENTTRACKING_H
#define EVENTTRACKING_H

#include <QObject>

class QSqlTableModel;

class eventTracking : public QObject
{
    Q_OBJECT
public:
    explicit eventTracking(QObject* parent = nullptr);

    QSqlTableModel* getModelEvent();
    QVector<QDate> getDatesWithEvents();
    void searchEvent(QString);

signals:
    void finishUpdateEventModel(QSqlTableModel*, int);

public slots:
    void startUpdateEventModel(int, QString, bool);

private:
    QSqlTableModel* m_modelEvent;
    int count; // counter visible headers
};

#endif // EVENTTRACKING_H
