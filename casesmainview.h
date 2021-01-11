#ifndef CASESMAINVIEW_H
#define CASESMAINVIEW_H

#include "casesmain.h"
#include "caseinfoview.h"
#include <QDialog>

namespace Ui {
class casesMainView;
}

class casesMainView : public QDialog
{
    Q_OBJECT

public:
    explicit casesMainView(int, QWidget* parent = nullptr);
    ~casesMainView();

    QString giveNameToDelegate(int);
    QString giveCaseCatToDelegate(int);
    QString giveClStatusToDelegate(int);

signals:
    void startUpdateCaseView();
    void filterModelWithClient(int);

private slots:
    void finishUpdateCaseView(QSqlTableModel*, int);
    void on_buttonAddCase_clicked();
    void on_buttonEditCase_clicked();
    void on_buttonDelete_clicked();
    void on_buttonSearch_clicked();
    void on_buttonExit_clicked();
    void on_buttonDocsCase_clicked();
    void on_buttonAllCases_clicked();
    void on_buttonReport_clicked();
    void on_buttonAddPayment_clicked();
    void on_buttonDelPay_clicked();

private:
    void updateCasesView();
    void setPayView(QTableView*, int);

    Ui::casesMainView *ui;
    casesMain* cMainBE; //pointer to class of logic
    bool m_newCase;     //work with new or existing case
    int m_idClient;
};

#endif // CASESMAINVIEW_H
