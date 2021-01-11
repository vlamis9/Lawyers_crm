#ifndef NEWEVENTVIEW_H
#define NEWEVENTVIEW_H

#include "newevent.h"
#include <QDialog>

class QDataWidgetMapper;

namespace Ui {
class newEventView;
}

class newEventView : public QDialog
{
    Q_OBJECT

public:
    explicit newEventView(const QDate&, QWidget* parent = nullptr);
    ~newEventView();

signals:
    void startUpdateDateEvView(const QDate&);
    void saveData(QMap<QString, QVariant>, bool);
    void modelSubmit(int, int);

private slots:
    void on_checkBoxUser_toggled(bool);
    void on_checkBoxCase_toggled(bool);
    void finishUpdateDateEvView(QSqlTableModel*, int);
    void on_buttonSave_clicked();
    void setMapperIndex(const QModelIndex&);
    void on_buttonChange_clicked();
    void on_buttonCancel_clicked();
    void on_buttonDelEvent_clicked();

private:
    void initInfo();
    void setMapper(QSqlTableModel*);

    //check spin box value, if value > (Selected date - Current Date) then message
    bool checkSpinValue();

    Ui::newEventView *ui;
    const QDate& m_date;
    QMap<int, int> indexesCasesCombo;
    newEvent* neBE; //pointer to class of logic
    QDataWidgetMapper* m_mapper;
    bool m_isUser; //to check event' owner
};

#endif // NEWEVENTVIEW_H
