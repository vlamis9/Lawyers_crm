#ifndef CLSTATUSDELEGATE_H
#define CLSTATUSDELEGATE_H

#include <QObject>
#include <QStyledItemDelegate>
#include "casesmainview.h"

class clStatusDelegate : public QStyledItemDelegate
{
    Q_OBJECT
public:
    explicit clStatusDelegate(QObject* parent = nullptr): QStyledItemDelegate(parent) {}

    virtual QString displayText(const QVariant& value, const QLocale& locale) const //override
     {
        Q_UNUSED(locale);
        casesMainView* view = qobject_cast<casesMainView*>(this->parent());
        QString clStatus = view->giveClStatusToDelegate(value.toInt());
        return clStatus;
     }
};

#endif // CLSTATUSDELEGATE_H
