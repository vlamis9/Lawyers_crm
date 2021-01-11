#include "reminder.h"
#include <QSqlQuery>
#include <QVariant>
#include <QDate>
#include <QMessageBox>
#include <QTimer>

REMINDER::REMINDER(QObject* parent):
    QObject(parent)
{    
    checkEvents();

    reminderTimer = new QTimer(this);
    connect(reminderTimer, &QTimer::timeout, this, &REMINDER::checkEvents);
    reminderTimer->setInterval(1000 * 60 * 60); //hour
    reminderTimer->setSingleShot(false);
    reminderTimer->start();
}

void REMINDER::checkEvents()
{
    QVector<QPair<QString, QString>> vecInfoToRemind;
    QSqlQuery query;
    query.prepare("SELECT DATE_E, NUMD, ENAME FROM events");
    query.exec();
    while (query.next())
    {
        int days = QDate::currentDate().daysTo(query.value(0).toDate());
        //если событие прошло
        if (days < 0) continue;
        //если осталось меньше или равно дней как указано в "уведомить за..."
        if (days <= query.value(1).toInt())
        {
            vecInfoToRemind.append({query.value(0).toDate().toString(Qt::SystemLocaleShortDate),
                                    query.value(2).toString()});
        }
    }
    if(!vecInfoToRemind.isEmpty())
    {
        QString str;
        for (const auto& pair : vecInfoToRemind)
        {
            str.append(pair.first + " " + pair.second + "\n\n");
        }

        QMessageBox::warning(0,"Важное сообщение!", "Уведомление о предстоящих событиях \n"
                             "Состоятся следующие события \n\n" + str);
    }
}

