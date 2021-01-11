#include "clientsformview.h"
#include "ui_clientsformview.h"
#include "docsview.h"
#include "dateformatdelegate.h"
#include "casesmainview.h"
#include <QSqlTableModel>
#include <QMessageBox>
#include <QCompleter>

clientsFormView::clientsFormView(QWidget* parent):
    QDialog(parent),
    ui(new Ui::clientsFormView)
{
    ui->setupUi(this);
    this->setWindowTitle(QObject::tr("Клиенты"));

    cFormBE = new clientsForm(this); //class of logic

    //"signal-slot" connections for updating Models and Views
    connect(this, &clientsFormView::startUpdateFizView,  cFormBE, &clientsForm::startUpdateFizModel);
    connect(this, &clientsFormView::startUpdateYurView,  cFormBE, &clientsForm::startUpdateYurModel);
    connect(cFormBE, &clientsForm::finishUpdateFizModel, this, &clientsFormView::finishUpdateFizView);
    connect(cFormBE, &clientsForm::finishUpdateYurModel, this, &clientsFormView::finishUpdateYurView);

    //"signal-slot" connection to be able to open the record by double-clicking
    connect(ui->fizView, &QTableView::doubleClicked, this, &clientsFormView::on_buttonEditFiz_clicked);
    connect(ui->yurView, &QTableView::doubleClicked, this, &clientsFormView::on_buttonEditYur_clicked);

    //emit signal to update Models and Views
    emit startUpdateFizView();
    emit startUpdateYurView();
}

clientsFormView::~clientsFormView()
{
    delete ui;
}

void clientsFormView::finishUpdateFizView(QSqlTableModel* modelFiz, int ind)
{
    //set model to the view
    ui->fizView->setModel(modelFiz);
    ui->fizView->setSelectionMode(QAbstractItemView::SingleSelection);
    ui->fizView->setSelectionBehavior(QAbstractItemView::SelectRows);

    //set hidden column "id"
    ui->fizView->setColumnHidden(0, true);

    //hide other than viewable headers
    for(int i = ind; i < modelFiz->columnCount(); i++)
    {
        ui->fizView->setColumnHidden(i, true);
    }
    //allow to sort items (by "Surname"), forbid to edit items
    ui->fizView->setSortingEnabled(true);
    ui->fizView->sortByColumn(static_cast<int>(FIZ::SURNAME), Qt::AscendingOrder);
    ui->fizView->setEditTriggers(QAbstractItemView::NoEditTriggers);

    //this allows you to see the data for the entire width of the column (and the item, and header)
    ui->fizView->resizeColumnsToContents();

    //set delegate to show date column in like-look format
    ui->fizView->setItemDelegateForColumn(static_cast<int>(FIZ::BIRTHDAY), new dateFormatDelegate(this));

    //set QCompleter to help user
    QCompleter* completer = new QCompleter(this);
    completer->setModel(modelFiz);
    ui->editSearchFiz->setCompleter(completer);
    completer->setCaseSensitivity( Qt::CaseInsensitive );
    completer->setCompletionColumn(static_cast<int>(FIZ::SURNAME));
}

void clientsFormView::finishUpdateYurView(QSqlTableModel* modelYur, int ind)
{
    //set model to the view
    ui->yurView->setModel(modelYur);
    ui->yurView->setSelectionMode(QAbstractItemView::SingleSelection);
    ui->yurView->setSelectionBehavior(QAbstractItemView::SelectRows);

    //set hidden column "id"
    ui->yurView->setColumnHidden(0, true);

    //hide other than viewable headers
    for(int i = ind; i < modelYur->columnCount(); i++)
    {
        ui->yurView->setColumnHidden(i, true);
    }

    ui->yurView->setSortingEnabled(true);
    ui->yurView->sortByColumn(static_cast<int>(YUR::COMPANY), Qt::AscendingOrder);
    ui->yurView->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->yurView->resizeColumnsToContents();

    //set QCompleter to help user
    QCompleter* completer = new QCompleter(this);
    completer->setModel(modelYur);
    ui->editSearchFiz->setCompleter(completer);
    completer->setCaseSensitivity( Qt::CaseInsensitive );
    completer->setCompletionColumn(static_cast<int>(YUR::COMPANY));
}

void clientsFormView::on_buttonAddFiz_clicked()
{
    m_newCl = true;
    fizClientView fizView(cFormBE->getModelFiz(), cFormBE->getModelMix(), ui->fizView, m_newCl, this);
    fizView.exec();
    //emit signal to update Models and Views
    emit startUpdateFizView();
}

void clientsFormView::on_buttonEditFiz_clicked()
{
    if (!(ui->fizView->selectionModel()->isSelected(ui->fizView->currentIndex())))
    {
        QMessageBox::warning(nullptr, QObject::tr("Не выбран клиент."),
                             QObject::tr("Необходимо выбрать строку с клиентом для редактирования"));
        return;
    }
    m_newCl = false;

    fizClientView fiz(cFormBE->getModelFiz(), cFormBE->getModelMix(), ui->fizView, m_newCl, this);
    fiz.exec();

    //this allows you to see the data for the entire width of the column (and the item, and header)
    ui->fizView->resizeColumnsToContents();
}

void clientsFormView::on_buttonDelFiz_clicked()
{
    if (!(ui->fizView->selectionModel()->isSelected(ui->fizView->currentIndex())))
    {
        QMessageBox::warning(nullptr, QObject::tr("Не выбран клиент"),
                             QObject::tr("Необходимо выбрать строку с клиентом для удаления"));
        return;
    }

    QMessageBox msgBox;
    msgBox.setText(QObject::tr("Удаление выбранного клиента"));
    msgBox.setInformativeText(QObject::tr("Вы собираетесь удалить клиента. Вы уверены?\n"
                              "Вы можете потерять важную информацию\n"
                              "Сохраните все нужные файлы!"));
    msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
    msgBox.setDefaultButton(  QMessageBox::No);
    msgBox.setIcon(QMessageBox::Warning);

    int decision = msgBox.exec();
    if (decision == QMessageBox::Yes)
    {
        cFormBE->deleteClient(ui->fizView->currentIndex().row(), Category::FIZ);
    }
}

void clientsFormView::on_buttonSearchFiz_clicked()
{
    cFormBE->searchClientsFiz(ui->editSearchFiz->text());
    ui->editSearchFiz->clear();
}

void clientsFormView::on_buttonAllFiz_clicked()
{
    emit startUpdateFizView();
}

void clientsFormView::on_buttonAddYur_clicked()
{
    m_newCl = true;
    yurClientView yurView(cFormBE->getModelYur(), cFormBE->getModelMix(), ui->fizView, m_newCl, this);
    yurView.exec();
    //emit signal to update Models and Views
    emit startUpdateYurView();
}

void clientsFormView::on_buttonEditYur_clicked()
{
    if (!(ui->yurView->selectionModel()->isSelected(ui->yurView->currentIndex())))
    {
        QMessageBox::warning(nullptr, QObject::tr("Не выбран клиент"),
                             QObject::tr("Необходимо выбрать строку с клиентом для редактирования"));
        return;
    }
    m_newCl = false;

    yurClientView yur(cFormBE->getModelYur(), cFormBE->getModelMix(), ui->yurView, m_newCl, this);
    yur.exec();

    //this allows you to see the data for the entire width of the column (and the item, and header)
    ui->fizView->resizeColumnsToContents();
}

void clientsFormView::on_buttonDelYur_clicked()
{
    if (!(ui->yurView->selectionModel()->isSelected(ui->yurView->currentIndex())))
    {
        QMessageBox::warning(nullptr, QObject::tr("Не выбран клиент"),
                             QObject::tr("Необходимо выбрать строку с клиентом для удаления"));
        return;
    }

    QMessageBox msgBox;
    msgBox.setText(QObject::tr("Удаление выбранного клиента"));
    msgBox.setInformativeText(QObject::tr("Вы собираетесь удалить клиента. Вы уверены?\n"
                              "Вы можете потерять важную информацию\n"
                              "Сохраните все нужные файлы!"));
    msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
    msgBox.setDefaultButton(  QMessageBox::No);
    msgBox.setIcon(QMessageBox::Warning);

    int decision = msgBox.exec();
    if (decision == QMessageBox::Yes)
    {
        cFormBE->deleteClient(ui->yurView->currentIndex().row(), Category::YUR);
    }
}

void clientsFormView::on_buttonSearchYur_clicked()
{
    cFormBE->searchClientsYur(ui->editSearchYur->text());
    ui->editSearchYur->clear();
}

void clientsFormView::on_buttonAllYur_clicked()
{
    emit startUpdateYurView();
}

void clientsFormView::on_buttonDocsFiz_clicked()
{
    if (!(ui->fizView->selectionModel()->isSelected(ui->fizView->currentIndex())))
    {
        QMessageBox::warning(nullptr, QObject::tr("Не выбран клиент"),
                             QObject::tr("Необходимо выбрать строку с клиентом для перехода к его документам"));
        return;
    }
    QModelIndex idIndex = ui->fizView->model()->index(ui->fizView->currentIndex().row(), 0, ui->fizView->currentIndex().parent());
    int idClient = idIndex.data().toInt();

    docsView view(false, this, 0, idClient);
    view.exec();
}

void clientsFormView::on_buttonDocsYur_clicked()
{
    if (!(ui->yurView->selectionModel()->isSelected(ui->yurView->currentIndex())))
    {
        QMessageBox::warning(nullptr, QObject::tr("Не выбран клиент"),
                             QObject::tr("Необходимо выбрать строку с клиентом для перехода к его документам"));
        return;
    }
    QModelIndex idIndex = ui->yurView->model()->index(ui->yurView->currentIndex().row(), 0, ui->yurView->currentIndex().parent());
    int idClient = idIndex.data().toInt();

    docsView view(false, this, 0, idClient);
    view.exec();
}

void clientsFormView::on_buttonCasesFiz_clicked()
{
    if (!(ui->fizView->selectionModel()->isSelected(ui->fizView->currentIndex())))
    {
        QMessageBox::warning(nullptr, QObject::tr("Не выбран клиент"),
                             QObject::tr("Необходимо выбрать строку с клиентом для перехода к его делам"));
        return;
    }
    QModelIndex idIndex = ui->fizView->model()->index(ui->fizView->currentIndex().row(), 0, ui->fizView->currentIndex().parent());
    int idClient = idIndex.data().toInt();

    casesMainView view(idClient, this);
    view.exec();
}

void clientsFormView::on_buttonCasesYur_clicked()
{
    if (!(ui->yurView->selectionModel()->isSelected(ui->yurView->currentIndex())))
    {
        QMessageBox::warning(nullptr, QObject::tr("Не выбран клиент"),
                             QObject::tr("Необходимо выбрать строку с клиентом для перехода к его делам"));
        return;
    }
    QModelIndex idIndex = ui->yurView->model()->index(ui->yurView->currentIndex().row(), 0, ui->yurView->currentIndex().parent());
    int idClient = idIndex.data().toInt();

    casesMainView view(idClient, this);
    view.exec();
}
