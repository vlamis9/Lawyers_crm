#ifndef DATEFORMATDELEGATE_H
#define DATEFORMATDELEGATE_H

#include <QObject>
#include <QStyledItemDelegate>
#include <QDate>

class dateFormatDelegate : public QStyledItemDelegate
{
    Q_OBJECT
public:
    explicit dateFormatDelegate(QObject* parent = nullptr): QStyledItemDelegate(parent) {}
    virtual QString displayText(const QVariant& value, const QLocale& locale ) const
    {
        Q_UNUSED(locale);
        return value.toDate().toString(Qt::SystemLocaleShortDate);
    }
};

#endif // DATEFORMATDELEGATE_H
