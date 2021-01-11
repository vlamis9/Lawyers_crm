#ifndef CUSTOMDATEEDIT_H
#define CUSTOMDATEEDIT_H

#include <QDateEdit>

class customDateEdit : public QDateEdit
{
    Q_OBJECT
public:
    customDateEdit(QWidget* parent = nullptr);
    virtual void paintEvent(QPaintEvent* event) override;

public slots:
    void setDate (const QDate& date);
    QDate date() const;

private:
    bool isNullField = true;
};


#endif // CUSTOMDATEEDIT_H
