#include "casesmainview.h"
#include "ui_casesmainview.h"
#include "namedelegate.h"
#include "casecatdelegate.h"
#include "clstatusdelegate.h"
#include "dateformatdelegate.h"
#include "docsview.h"
#include "reporter.h"

#include <QSqlTableModel>
#include <QMessageBox>
#include <QDateEdit>

casesMainView::casesMainView(int idClient, QWidget* parent) :
    QDialog(parent),
    ui(new Ui::casesMainView),
    m_idClient(idClient)
{
    ui->setupUi(this);
    this->setWindowTitle(QObject::tr("Дела"));

    cMainBE = new casesMain(this); //class of logic

    //"signal-slot" connections for updating Models and Views
    connect(this, &casesMainView::startUpdateCaseView,  cMainBE, &casesMain::startUpdateCaseModel);
    connect(cMainBE, &casesMain::finishUpdateCaseModel, this, &casesMainView::finishUpdateCaseView);

    //"signal-slot" connection to be able to open the record by double-clicking
    connect(ui->casesView, &QTableView::doubleClicked, this, &casesMainView::on_buttonEditCase_clicked);

    emit startUpdateCaseView();

    //"signal-slot" connections for filter Model to see only one client's cases
    connect(this, &casesMainView::filterModelWithClient, cMainBE, &casesMain::filterModelWithClient);

    if (m_idClient)
    {
        emit filterModelWithClient(m_idClient);
    }
}

casesMainView::~casesMainView()
{
    delete ui;
}

QString casesMainView::giveNameToDelegate(int id)
{
    return cMainBE->giveNameToDelegate(id);
}

QString casesMainView::giveCaseCatToDelegate(int cat)
{
    QString catCaseArr[]  = {QObject::tr("гражданское"),             QObject::tr("уголовное"),
                             QObject::tr("административное"),        QObject::tr("арбитраж"),
                             QObject::tr("кодекс админ. судопр-ва"), QObject::tr("иное")};
    return catCaseArr[cat];
}

QString casesMainView::giveClStatusToDelegate(int clSt)
{
    QString clStatusArr[] = {QObject::tr("истец"),        QObject::tr("ответчик"),
                             QObject::tr("третье лицо"),  QObject::tr("обвиняемый"),
                             QObject::tr("потерпевший"),  QObject::tr("привл. к админ. отв."),
                             QObject::tr("админ. истец"), QObject::tr("иное")};
    return clStatusArr[clSt];
}

void casesMainView::finishUpdateCaseView(QSqlTableModel* modelCase, int count)
{
    //set model to the view
    ui->casesView->setModel(modelCase);
    ui->casesView->setSelectionMode(QAbstractItemView::SingleSelection);
    ui->casesView->setSelectionBehavior(QAbstractItemView::SelectRows);

    //set hidden column "id"
    ui->casesView->setColumnHidden(0, true);

    //hide other than viewable headers
    for(int i = count; i < modelCase->columnCount(); i++)
    {
        ui->casesView->setColumnHidden(i, true);
    }

    //allow to sort items (by Client), forbid to edit items
    ui->casesView->setSortingEnabled(true);
    ui->casesView->sortByColumn(static_cast<int>(CASE::ID_CLIENT), Qt::AscendingOrder);
    ui->casesView->setEditTriggers(QAbstractItemView::NoEditTriggers);

    //set delegate to show name of the Client
    int nameDelegateCol = static_cast<int>(CASE::ID_CLIENT);
    ui->casesView->setItemDelegateForColumn(nameDelegateCol, new nameDelegate(this));

    //set delegate to show category of the case
    int caseCatDelegateCol = static_cast<int>(CASE::CASECAT);
    ui->casesView->setItemDelegateForColumn(caseCatDelegateCol, new caseCatDelegate(this));

    //set delegate to show Client's status
    int clStDelegateCol = static_cast<int>(CASE::CLIENTSTATUS);
    ui->casesView->setItemDelegateForColumn(clStDelegateCol, new clStatusDelegate(this));

    //set delegate to show date column in like-look format
    int caseDate = static_cast<int>(CASE::CASEDATE);
    ui->casesView->setItemDelegateForColumn(caseDate, new dateFormatDelegate(this));    

    //this allows you to see the data for the entire width of the column (and the item, and header)
    ui->casesView->resizeColumnsToContents();    
}

void casesMainView::on_buttonAddCase_clicked()
{    
    if (cMainBE->checkClientExist())
    {
        m_newCase = true;
        caseInfoView cas(cMainBE->getModelCase(), ui->casesView, m_newCase, this);
        cas.exec();
        emit startUpdateCaseView();
    }
    else
    {
        QMessageBox::warning(nullptr, QObject::tr("Ошибка"),
                             QObject::tr("Не существует клиента, которому можно добавить дело. "
                                         "<BR><BR>"
                                         "Необходимо вначале добавить клиента!"
                                         ));
    }
}

void casesMainView::on_buttonEditCase_clicked()
{
    if (!(ui->casesView->selectionModel()->isSelected(ui->casesView->currentIndex())))
    {
        QMessageBox::warning(nullptr, QObject::tr("Не выбрано дело. "),
                                      QObject::tr("Необходимо выбрать строку с делом "
                                                  "для редактирования"));
        return;
    }
    m_newCase = false;
    caseInfoView cas(cMainBE->getModelCase(), ui->casesView, m_newCase, this);
    cas.exec();

    ui->casesView->resizeColumnsToContents();
}

void casesMainView::on_buttonDelete_clicked()
{  
    if (!(ui->casesView->selectionModel()->isSelected(ui->casesView->currentIndex())))
    {
        QMessageBox::warning(nullptr, QObject::tr("Не выбрано дело"),
                             QObject::tr("Необходимо выбрать строку с делом для удаления"));
        return;
    }

    QMessageBox* msgBox = new QMessageBox(this);
    msgBox->setText(QObject::tr("Удаление выбранного дела"));
    msgBox->setInformativeText(QObject::tr("Вы собираетесь удалить дело. Вы уверены?\n"
                              "Вы можете потерять важную информацию\n"
                              "Сохраните все нужные файлы!"));
    msgBox->setStandardButtons(QMessageBox::Yes | QMessageBox::No);
    msgBox->setDefaultButton(  QMessageBox::No);
    msgBox->setIcon(QMessageBox::Warning);

    int decision = msgBox->exec();
    if (decision == QMessageBox::Yes)
    {
        cMainBE->deleteCase(ui->casesView->currentIndex().row());
    }
}

void casesMainView::on_buttonSearch_clicked()
{
    cMainBE->searchCase(ui->editSearch->text());
    ui->editSearch->clear();
}

void casesMainView::on_buttonExit_clicked()
{
    close();
}

void casesMainView::on_buttonDocsCase_clicked()
{
    if (!(ui->casesView->selectionModel()->isSelected(ui->casesView->currentIndex())))
    {
        QMessageBox::warning(nullptr, QObject::tr("Не выбрано дело"),
                             QObject::tr("Необходимо выбрать строку с делом для перехода к документам"));
        return;
    }
    QModelIndex idIndex = ui->casesView->model()->index(ui->casesView->currentIndex().row(), 0, ui->casesView->currentIndex().parent());
    int idCase = idIndex.data().toInt();

    docsView view(false, this, idCase, 0);
    view.exec();
}


void casesMainView::on_buttonAllCases_clicked()
{
    emit startUpdateCaseView();
}

void casesMainView::on_buttonReport_clicked()
{
    if (!(ui->casesView->selectionModel()->isSelected(ui->casesView->currentIndex())))
    {
        QMessageBox::warning(nullptr, QObject::tr("Не выбрано дело"),
                             QObject::tr("Необходимо выбрать строку с делом для подготовки отчета"));
        return;
    }
    QModelIndex idIndex = ui->casesView->model()->index(ui->casesView->currentIndex().row(), 0, ui->casesView->currentIndex().parent());
    int idCase = idIndex.data().toInt();
    reporter rep(idCase, this);
    rep.exec();
}

void casesMainView::on_buttonAddPayment_clicked()
{
    if (!(ui->casesView->selectionModel()->isSelected(ui->casesView->currentIndex())))
    {
        QMessageBox::warning(nullptr, QObject::tr("Не выбрано дело"),
                             QObject::tr("Необходимо выбрать строку с делом для добавления платежа"));
        return;
    }
    QModelIndex idIndex = ui->casesView->model()->index(ui->casesView->currentIndex().row(), 0, ui->casesView->currentIndex().parent());
    int idCase = idIndex.data().toInt();
    QDialog* addPayDlg = new QDialog(this);
    QLineEdit* editAddPay = new QLineEdit(addPayDlg);
    QIntValidator val;
    editAddPay->setValidator(&val);
    QPushButton* buttonAddPay = new QPushButton(addPayDlg);
    QDateEdit* dateAddPay = new QDateEdit(addPayDlg);
    dateAddPay->setDate(QDate::currentDate());
    buttonAddPay->setText(QObject::tr("Доб. платеж"));
    buttonAddPay->setSizePolicy(QSizePolicy
                               (QSizePolicy::Maximum,
                                QSizePolicy::Fixed,
                                QSizePolicy::ToolButton));
    dateAddPay->  setSizePolicy(QSizePolicy
                               (QSizePolicy::Maximum,
                                QSizePolicy::Fixed,
                                QSizePolicy::ToolButton));
    QVBoxLayout vbox;
    vbox.addWidget(editAddPay);
    vbox.addWidget(dateAddPay);
    vbox.addWidget(buttonAddPay);

    addPayDlg->setLayout(&vbox);
    addPayDlg->setFixedWidth(500);
    addPayDlg->setWindowTitle(QObject::tr("Добавление нового платежа"));

    connect(buttonAddPay, &QPushButton::clicked, this,
            [&](){if (editAddPay->text().isEmpty()) return;
                  cMainBE->addPayment(idCase, editAddPay->text().toInt(),
                  dateAddPay->date());});
    connect(buttonAddPay, &QPushButton::clicked, addPayDlg, &QDialog::close);
    addPayDlg->exec();
}

void casesMainView::on_buttonDelPay_clicked()
{
    if (!(ui->casesView->selectionModel()->isSelected(ui->casesView->currentIndex())))
    {
        QMessageBox::warning(nullptr, QObject::tr("Не выбрано дело"),
                             QObject::tr("Необходимо выбрать строку с делом для удаления платежа"));
        return;
    }
    QModelIndex idIndex = ui->casesView->model()->index(ui->casesView->currentIndex().row(), 0, ui->casesView->currentIndex().parent());
    int idCase = idIndex.data().toInt();
    QDialog* delPayDlg = new QDialog(this);
    QTableView* payView = new QTableView(delPayDlg);
    QPushButton* buttonDelPay = new QPushButton(delPayDlg);
    QPushButton* buttonExit = new QPushButton(delPayDlg);

    buttonDelPay->setText(QObject::tr("Удалить платеж"));
    buttonDelPay->setSizePolicy(QSizePolicy
                               (QSizePolicy::Maximum,
                                QSizePolicy::Fixed,
                                QSizePolicy::ToolButton));
    buttonExit->setText(QObject::tr("Выход"));
    buttonExit->setSizePolicy(QSizePolicy
                               (QSizePolicy::Maximum,
                                QSizePolicy::Fixed,
                                QSizePolicy::ToolButton));
    QHBoxLayout hbox;
    hbox.addWidget(payView);
    hbox.addWidget(buttonDelPay);
    hbox.addWidget(buttonExit);
    delPayDlg->setLayout(&hbox);
    delPayDlg->setFixedWidth(500);
    delPayDlg->setWindowTitle(QObject::tr("Удаление платежа"));

    //set model to the view
    setPayView(payView, idCase);

    //connect
    connect(buttonDelPay, &QPushButton::clicked, this,
            [&](){if (!(payView->selectionModel()->isSelected(payView->currentIndex()))) return;
                  cMainBE->delPayment(payView->currentIndex(), idCase);
                  setPayView(payView, idCase);});

    connect(buttonExit, &QPushButton::clicked, delPayDlg, &QDialog::close);
    delPayDlg->exec();

}

void casesMainView::setPayView(QTableView* payView, int idCase)
{
    payView->setModel(cMainBE->getModelPay(idCase));
    payView->setSelectionMode(QAbstractItemView::SingleSelection);
    payView->setSelectionBehavior(QAbstractItemView::SelectRows);

    //set hidden column "id_pay" and "id_case"
    payView->setColumnHidden(0, true);
    payView->setColumnHidden(1, true);

    //allow to sort items (by "date"), forbid to edit items
    payView->setSortingEnabled(true);
    payView->sortByColumn(2, Qt::AscendingOrder);
    payView->setEditTriggers(QAbstractItemView::NoEditTriggers);

    //this allows you to see the data for the entire width of the column (and the item, and header)
    payView->resizeColumnsToContents();
}
