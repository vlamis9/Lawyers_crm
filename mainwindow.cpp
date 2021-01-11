#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "clientsformview.h"
#include "casesmainview.h"
#include "popup.h"
#include "docsview.h"
#include "eventtrackingview.h"
#include <QSqlQuery>
#include <QMessageBox>
#include <QFile>

MainWindow::MainWindow(QWidget *parent):
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    this->setWindowTitle(QObject::tr("Электронный помощник адвоката"));
    this->setFixedSize(QSize(480, 460));

    ui->label->setStyleSheet("image:url(:/jus1.jpg);");

    init();

    reminder = new REMINDER(this);

    //set gui effects of app
    this->setStyleSheet(
          "QLineEdit { "
          " border-radius: 8px; "
          " color:rgb(0, 0, 0); "
          " font-size:16px; "
          " background-color: rgb(80%, 85%, 85%); "
          "} "

          "QLineEdit:focus { "
          " border:1px outset; "
          " border-radius: 8px; "
          " border-color: rgb(5%, 10%, 30%); "
          " color:rgb(0, 0, 0); "
          " background-color: rgb(95%, 90%, 75%);"
          "} "

          "QTableView { "
          " border: 2px solid rgb(5%, 15%, 15%); "
          " border-top-color: rgb(5%, 20%, 35%); "
          " border-radius: 4px; "
          " background-color: rgb(85%, 90%, 90%); "
          " gridline-color: #771; "
          " selection-background-color: rgb(90%, 75%, 55%); "
          " selection-color: rgb(5%, 5%, 25%); "
          " color:rgb(5%, 5%, 15%); "
          " font-size:16px; "
          "} "

          "QLabel { "
          " font-size:16px; "
          " color: palette(shadow); "
          " font: bold; "
          "} "

          "QHeaderView { "
          " font-size:12px; "
          " color: palette(shadow); "
          "} "

          "QPushButton { "
          " border: 1px solid rgb(5%, 15%, 15%); "
          " border-radius: 4px; "
          " font-size:16px; "
          " color: palette(text ); "
          " background-color: rgb(80%, 85%, 100%); "
          "} "

          "QPushButton:pressed { "
          " background-color: rgb(224, 0, 0); "
          " border-style: inset; "
          "} "

          "QPushButton:!enabled { "
          " background-color: rgb(85, 90, 85); "
          " border-style: inset; "
          "} "

          "QGroupBox { "
          " border: 2px solid gray; "
          " border-radius: 5px; "
          " margin-top: 1ex; "
          "} "
          );
}

MainWindow::~MainWindow()
{
    QSqlDatabase::database().close();
    delete ui;
}

void MainWindow::on_buttonClients_clicked()
{
    clientsFormView* cForm = new clientsFormView(this);
    cForm->exec();
}

void MainWindow::on_buttonCases_clicked()
{
    casesMainView cas(0, this);
    cas.exec();
}

void MainWindow::init()
{
    QString dbName = "MyDB.db";
    if (!QFile::exists(dbName)) //if DB file don't exist
    {
        QString greetTitle = "Приветствие";
        QString greetText  = "Это первый запуск программы"
                             "<BR><BR>Файл базы данных: <b>\"" + dbName + "\"</b> "
                             "будет создан в вашей рабочей директории.";

        QMessageBox greetMsg; //first greeting message
        //qPrintable converts QString to const char*
        greetMsg.setWindowTitle(QObject::tr(qPrintable(greetTitle)));
        greetMsg.setText(QObject::tr(qPrintable(greetTitle)));
        greetMsg.setInformativeText(QObject::tr(qPrintable(greetText)));
        greetMsg.setIcon(QMessageBox::Information);
        greetMsg.exec();
    }

    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE"); //set connection
    db.setDatabaseName(dbName); //set DB name
    if(!db.open()) //if not opened
    {
        QString errTitle = QObject::tr("Ошибка!");
        QString errText  = QObject::tr("Произошла ошибка\n"
                                       "Не получается открыть файл базы данных");

        QMessageBox errMsg; //message about error
        errMsg.setText(QObject::tr(qPrintable(errTitle)));
        errMsg.setInformativeText(QObject::tr(qPrintable(errText)));
        errMsg.setIcon(QMessageBox::Critical);
        errMsg.exec();
        exit(0);
    }
    else
    {
        QString strSuccessTitle = "Файл базы данных успешно открыт";
        QString strSuccessText  = "Файл: "
                                  "<b>\"" + dbName + "\"</b> "
                                  "открыт";

        //pop-up message
        PopUp* pop = new PopUp();
        pop->setAttribute(Qt::WA_DeleteOnClose);
        pop->setPopupText(strSuccessText);
        pop->show();

        QMessageBox yesMsg; //message if DB was opened
        yesMsg.setWindowTitle(QObject::tr("БД"));
        yesMsg.setText(QObject::tr(qPrintable(strSuccessTitle)));
        yesMsg.setInformativeText(QObject::tr(qPrintable(strSuccessText)));
        yesMsg.setIcon(QMessageBox::Information);
        yesMsg.exec();
    }
        QSqlQuery fkeys;
        fkeys.exec("PRAGMA foreign_keys = ON;");

        QString strFiz = "CREATE TABLE fizClient "
                         "(ID_FIZ  INTEGER NOT NULL PRIMARY KEY, "
                         "SURNAME  VARCHAR, " //Фамилия
                         "NAME     VARCHAR, " //Имя
                         "MIDDLE   VARCHAR, " //Отчество
                         "BIRTHDAY DATE, "    //Дата рождения VARCHAR
                         "TELM     VARCHAR, " //
                         "EMAIL    VARCHAR, " //
                         "PASSER   INTEGER, " //
                         "PASNUM   INTEGER, " //
                         "WHENIS   DATE, "    //
                         "ISBYWHOM VARCHAR, " //
                         "PASCODE  VARCHAR, " //
                         "TELW     VARCHAR, " //
                         "IND      INTEGER, " //
                         "CITY     VARCHAR, " //
                         "STREET   VARCHAR, " //
                         "BLD      VARCHAR, " //
                         "CORP     VARCHAR, " //
                         "FLAT     VARCHAR, " //
                         "NOTES    VARCHAR, " //
                         "CONSTRAINT link_mixClients_fizClient FOREIGN KEY (ID_FIZ) "
                         "REFERENCES mixClients (ID_MIX), "
                         "CONSTRAINT unique_id_fiz UNIQUE (ID_FIZ) )";

        QString strYur = "CREATE TABLE yurClient "
                         "(ID_YUR  INTEGER NOT NULL PRIMARY KEY, "
                         "OPF      VARCHAR, "
                         "COMPANY  VARCHAR, "
                         "OGRN     INTEGER, "
                         "INN      INTEGER, "
                         "TEL      VARCHAR, "
                         "EMAIL    VARCHAR, "
                         "DELEGATE VARCHAR, "
                         "SURDEL   VARCHAR, "
                         "NAMEDEL  VARCHAR, "
                         "MIDDEL   VARCHAR, "
                         "IND      INTEGER, "
                         "CITY     VARCHAR, "
                         "STREET   VARCHAR, "
                         "BLD      VARCHAR, "
                         "CORP     VARCHAR, "
                         "ROOM     VARCHAR, "
                         "DOC      VARCHAR, "
                         "REQS     VARCHAR, "
                         "BSURNAME VARCHAR, "
                         "BNAME    VARCHAR, "
                         "BMIDDLE  VARCHAR, "
                         "NOTES    VARCHAR, "
                         "CONSTRAINT link_mixClients_yurClient FOREIGN KEY (ID_YUR) "
                         "REFERENCES mixClients (ID_MIX), "
                         "CONSTRAINT unique_id_yur UNIQUE (ID_YUR) )";

        QString strMix = "CREATE TABLE mixClients "
                         "(ID_MIX      INTEGER NOT NULL PRIMARY KEY AUTOINCREMENT, "
                         "CLIENT_TYPE  INTEGER NOT NULL)";

        QString strCase = "CREATE TABLE casesTable "
                          "(ID_CASE       INTEGER NOT NULL PRIMARY KEY AUTOINCREMENT, "
                          "ID_CLIENT      INTEGER NOT NULL, "
                          "CLIENTSTATUS   INTEGER NOT NULL, " //статус клиента в деле
                          "CASECAT        INTEGER NOT NULL, " //категория дела
                          "CASEDATE       DATE, "    //дата дела
                          "CONTRACTNUM    VARCHAR, " //номер договора
                          "CONTRACTSUM    INTEGER, " //сумма по договору
                          "MAINPOINT      VARCHAR, " //суть дела
                          "CASEPARTS      VARCHAR, " //участники дела
                          "RULESOFLAW     VARCHAR, " //нормы права
                          "PAYDATE        VARCHAR, " //оплата до
                          "NOTES          VARCHAR, " //примечания
                          "CONSTRAINT link_mixClients_cases FOREIGN KEY (ID_CLIENT) "
                          "REFERENCES mixClients (ID_MIX) ON DELETE CASCADE )";

        QString strClD = "CREATE TABLE clientDocs "
                          "(ID_DOC      INTEGER NOT NULL PRIMARY KEY, "
                          "ID_CLIENT    INTEGER NOT NULL, "
                          "CONSTRAINT link_mixClients_clientDocs FOREIGN KEY (ID_CLIENT) "
                          "REFERENCES mixClients (ID_MIX), "
                          "CONSTRAINT link_documents_clientDocs FOREIGN KEY (ID_DOC) "
                          "REFERENCES documents (ID_DOC) ON DELETE CASCADE, "
                          "CONSTRAINT unique_id_doc_cl UNIQUE (ID_DOC) )";

        QString strCaD = "CREATE TABLE caseDocs "
                         "(ID_DOC INTEGER NOT NULL PRIMARY KEY, "
                         "ID_CASE INTEGER NOT NULL, "
                         "CONSTRAINT link_cases_caseDocs FOREIGN KEY (ID_CASE) "
                         "REFERENCES casesTable (ID_CASE), "
                         "CONSTRAINT link_documents_caseDocs FOREIGN KEY (ID_DOC) "
                         "REFERENCES documents (ID_DOC) ON DELETE CASCADE, "
                         "CONSTRAINT unique_id_doc_ca UNIQUE (ID_DOC) )";

        QString strUsD = "CREATE TABLE userDocs "
                         "(ID_DOC INTEGER NOT NULL PRIMARY KEY, "
                         "CONSTRAINT link_documents_userDocs FOREIGN KEY (ID_DOC) "
                         "REFERENCES documents (ID_DOC) ON DELETE CASCADE, "
                         "CONSTRAINT unique_id_doc_us UNIQUE (ID_DOC) )";

        QString strDoc = "CREATE TABLE documents "
                         "(ID_DOC   INTEGER NOT NULL PRIMARY KEY AUTOINCREMENT, "
                         "CAT_DOC   INTEGER NOT NULL, "
                         "CAT_OWNER INTEGER NOT NULL, "
                         "FNAME     VARCHAR NOT NULL, "
                         "FDATE     Date NOT NULL, "
                         "FSIZE     VARCHAR NOT NULL )";

        QString strFi = "CREATE TABLE files "
                        "(ID_FILE INTEGER NOT NULL PRIMARY KEY, "
                        "FILE     BLOB NOT NULL, "
                        "CONSTRAINT link_documents_files FOREIGN KEY (ID_FILE) "
                        "REFERENCES documents (ID_DOC) ON DELETE CASCADE, "
                        "CONSTRAINT unique_id_file UNIQUE (ID_FILE) )";

        QString strEve = "CREATE TABLE events "
                         "(ID_EVE   INTEGER NOT NULL PRIMARY KEY AUTOINCREMENT, " //ID события
                         "DATE_E    DATE NOT NULL, "                              //дата события
                         "CAT_EVE   INTEGER NOT NULL, "                           //категория соб.
                         "ENAME     VARCHAR, "                                    //название соб.
                         "EDET      VARCHAR, "                                    //детали соб.
                         "DATE_CR   DATE NOT NULL, "                              //дата создания
                         "CAT_OWNER INTEGER NOT NULL, "                           //категория влад.
                         "NUMD      INTEGER NOT NULL )";                          //уведомить за дн.

        QString strUeV = "CREATE TABLE userEvents "
                         "(ID_EVE INTEGER NOT NULL PRIMARY KEY, "
                         "CONSTRAINT link_events_userEvents FOREIGN KEY (ID_EVE) "
                         "REFERENCES events (ID_EVE) ON DELETE CASCADE, "
                         "CONSTRAINT unique_id_eve_us UNIQUE (ID_EVE) )";

        QString strCeV = "CREATE TABLE caseEvents "
                         "(ID_EVE INTEGER NOT NULL PRIMARY KEY, "
                         "ID_CASE INTEGER NOT NULL, "
                         "CONSTRAINT link_cases_caseEvents FOREIGN KEY (ID_CASE) "
                         "REFERENCES casesTable (ID_CASE), "
                         "CONSTRAINT link_events_caseEvents FOREIGN KEY (ID_EVE) "
                         "REFERENCES events (ID_EVE) ON DELETE CASCADE, "
                         "CONSTRAINT unique_id_eve_ca UNIQUE (ID_EVE) )";

        QString strPay = "CREATE TABLE payments "
                         "(ID_PAY  INTEGER NOT NULL PRIMARY KEY, "
                         "ID_CASE  INTEGER NOT NULL, "
                         "DATE_PAY DATE NOT NULL, "
                         "SUM      INTEGER NOT NULL, "
                         "CONSTRAINT link_cases_payments FOREIGN KEY (ID_CASE) "
                         "REFERENCES casesTable (ID_CASE) ON DELETE CASCADE ) ";

        QStringList listStrTables {strFiz, strYur, strMix, strCase, strClD,
                                   strCaD, strUsD, strDoc, strFi, strEve, strUeV, strCeV, strPay};

        QSqlQuery query(db);
        for(const auto& table : listStrTables)
        {
            query.exec(table);
        }
}

void MainWindow::on_buttonDocs_clicked()
{
    docsView view(true, this);
    view.exec();
}

void MainWindow::on_buttonTasks_clicked()
{
    eventTrackingView view(this);
    view.exec();
}


