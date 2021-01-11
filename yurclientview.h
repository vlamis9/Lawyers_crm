#ifndef YURCLIENTVIEW_H
#define YURCLIENTVIEW_H

#include "yurclient.h"
#include <QDialog>

class QDataWidgetMapper;
class QTableView;

namespace Ui {
class yurClientView;
}

class yurClientView : public QDialog
{
    Q_OBJECT

public:
    explicit yurClientView(QSqlTableModel*, QSqlTableModel*, QTableView*, bool, QWidget* parent = nullptr);
    ~yurClientView();

signals:
    void saveData();
    void sendInfoFromUser(QMap<QString, QString>);

public slots:
    void errMsgDuplicate();
    void submitChanges();
    void updateYur();

private slots:
    void on_buttonSaveYur_clicked();
    void on_buttonCancelYur_clicked();

private:
    void setMapper(QSqlTableModel*);
    bool checkIfEmpty();

    Ui::yurClientView* ui;
    yurClient* yClientBE; //pointer to class of logic
    bool m_newCl; //new or existing client
    QDataWidgetMapper* m_mapper;
    QTableView* m_yurView;
};

#endif // YURCLIENTVIEW_H
