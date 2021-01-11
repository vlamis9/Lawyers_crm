#ifndef CASECATDELEGATE_H
#define CASECATDELEGATE_H

#include <QObject>
#include <QStyledItemDelegate>
#include "casesmainview.h"

class caseCatDelegate : public QStyledItemDelegate
{
    Q_OBJECT
public:
    explicit caseCatDelegate(QObject* parent = nullptr): QStyledItemDelegate(parent) {}

    virtual QString displayText(const QVariant& value, const QLocale& locale) const //override
    {
        Q_UNUSED(locale);
        casesMainView* view = qobject_cast<casesMainView*>(this->parent());
        QString category = view->giveCaseCatToDelegate(value.toInt());
        return category;
     }
};

#endif // CASECATDELEGATE_H
