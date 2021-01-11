#include "customdateedit.h"
#include <QLineEdit>

customDateEdit::customDateEdit(QWidget* parent):
    QDateEdit(parent)
{
    this->setButtonSymbols(QAbstractSpinBox::NoButtons);
    this->setCalendarPopup(true);
    this->setDate(QDate::currentDate());
    connect(this, &QDateEdit::userDateChanged, [this](){isNullField = false;});
}

void customDateEdit::paintEvent(QPaintEvent* event)
{
    if (isNullField)
    {
        lineEdit()->setText(QObject::tr("Выберите дату"));
    }
    QDateEdit::paintEvent(event);
}

void customDateEdit::setDate(const QDate& date)
{
    QDateEdit::setDate(date);
}

QDate customDateEdit::date() const
{
    return isNullField ? QDate() : QDateEdit::date();
}






