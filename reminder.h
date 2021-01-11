#ifndef REMINDER_H
#define REMINDER_H

#include <QObject>

class QSqlTableModel;
class QTimer;

class REMINDER : public QObject
{
    Q_OBJECT
public:
    explicit REMINDER(QObject* parent = nullptr);

private slots:
    void checkEvents();

private:
    QTimer* reminderTimer;
};

#endif // REMINDER_H
