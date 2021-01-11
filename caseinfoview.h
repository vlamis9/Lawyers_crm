#ifndef CASEINFOVIEW_H
#define CASEINFOVIEW_H

#include "caseinfo.h"
#include <QDialog>

class QSqlTableModel;
class QTableView;
class QDataWidgetMapper;

namespace Ui {
class caseInfoView;
}

class caseInfoView : public QDialog
{
    Q_OBJECT

public:
    explicit caseInfoView(QSqlTableModel*, QTableView*, bool, QWidget* parent = nullptr);
    ~caseInfoView();

public slots:
        void showLabelCatCl(int);

private slots:
        void submitChanges();
        void on_buttonCancelCase_clicked();
        void on_buttonSaveCase_clicked();

signals:
        void saveData(int);
        void sendInfoFromUser(QMap<QString, QVariant>);

private:
    void setMapper(QSqlTableModel*);
    void initInfo();

    Ui::caseInfoView* ui;
    caseInfo* cInfoBE;
    bool m_newCase; //new or existing case
    QDataWidgetMapper* m_mapper;
    QTableView* m_casesView;

    QMap<int, QString> caseMap;
    QMap<int, QString> clientMap;

    QMap<int, int> indComboUser; //?
    QMap<int, int> indexesCombo;
};

#endif // CASEINFOVIEW_H
