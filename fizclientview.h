#ifndef FIZCLIENTVIEW_H
#define FIZCLIENTVIEW_H

#include "fizclient.h"
#include <QDialog>

class QDataWidgetMapper;
class QTableView;

namespace Ui {
class fizClientView;
}

class fizClientView : public QDialog
{
    Q_OBJECT

public:
    explicit fizClientView(QSqlTableModel*, QSqlTableModel*, QTableView*, bool, QWidget* parent = nullptr);
    ~fizClientView();

signals:
    void saveData(int);
    void sendInfoFromUser(QMap<QString, QVariant>);

public slots:
    void errMsgDuplicate();
    void submitChanges();
    void updateFiz();

private slots:
    void on_buttonSaveFiz_clicked();
    void on_buttonCancelFiz_clicked();

private:
    void setMapper(QSqlTableModel*);

    Ui::fizClientView* ui;
    fizClient* fClientBE; //pointer to class of logic
    bool m_newCl; //new or existing client
    QDataWidgetMapper* m_mapper;
    QTableView* m_fizView;
};

#endif // FIZCLIENTVIEW_H
