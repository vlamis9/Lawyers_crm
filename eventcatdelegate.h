#ifndef EVENTCATDELEGATE_H
#define EVENTCATDELEGATE_H

#include <QObject>
#include <QStyledItemDelegate>
#include "enums.h"

class eventCatDelegate : public QStyledItemDelegate
{
    Q_OBJECT
public:
    explicit eventCatDelegate(QObject* parent = nullptr): QStyledItemDelegate(parent) {}

    virtual QString displayText(const QVariant& value, const QLocale& locale) const //override
     {
        Q_UNUSED(locale);
        return eventCatArr[value.toInt()];
     }
};

#endif // EVENTCATDELEGATE_H
