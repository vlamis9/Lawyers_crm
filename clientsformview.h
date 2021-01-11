#ifndef CLIENTSFORMVIEW_H
#define CLIENTSFORMVIEW_H

#include "clientsform.h"
#include "fizclientview.h"
#include "yurclientview.h"
#include <QDialog>

namespace Ui {
class clientsFormView;
}

class clientsFormView : public QDialog
{
    Q_OBJECT

public:
    explicit clientsFormView(QWidget* parent = nullptr);
    ~clientsFormView();

signals:
    void startUpdateFizView();
    void startUpdateYurView();
    void sendInfoFromUser(QString);

private slots:
    void finishUpdateFizView(QSqlTableModel*, int);
    void finishUpdateYurView(QSqlTableModel*, int);
    void on_buttonAddFiz_clicked();
    void on_buttonEditFiz_clicked();
    void on_buttonDelFiz_clicked();
    void on_buttonSearchFiz_clicked();
    void on_buttonAllFiz_clicked();
    void on_buttonAddYur_clicked();
    void on_buttonEditYur_clicked();
    void on_buttonDelYur_clicked();
    void on_buttonSearchYur_clicked();
    void on_buttonAllYur_clicked();
    void on_buttonDocsFiz_clicked();
    void on_buttonDocsYur_clicked();
    void on_buttonCasesFiz_clicked();
    void on_buttonCasesYur_clicked();

private:
    Ui::clientsFormView *ui;
    clientsForm* cFormBE; //pointer to class of logic
    bool m_newCl;         //work with new or existing client
};

#endif // CLIENTSFORMVIEW_H
