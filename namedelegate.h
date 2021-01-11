#ifndef NAMEDELEGATE_H
#define NAMEDELEGATE_H

#include <QObject>
#include <QStyledItemDelegate>
#include "casesmainview.h"

class nameDelegate : public QStyledItemDelegate
{
    Q_OBJECT
public:
    explicit nameDelegate(QObject* parent = nullptr): QStyledItemDelegate(parent) {}

    virtual QString displayText(const QVariant& value, const QLocale& locale) const //override
     {
        Q_UNUSED(locale);
        casesMainView* view = qobject_cast<casesMainView*>(this->parent());
        QString name = view->giveNameToDelegate(value.toInt());
        return name;
     }
};

#endif // NAMEDELEGATE_H
