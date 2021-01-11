#include "fizclientview.h"
#include "ui_fizclientview.h"
#include "clientsformview.h"
#include <QDataWidgetMapper>
#include <QSqlTableModel>
#include <QTableView>
#include <QMessageBox>

fizClientView::fizClientView(QSqlTableModel* modelFiz, QSqlTableModel* modelMix,
                             QTableView* fizView, bool newCl, QWidget* parent):
    QDialog(parent),
    ui(new Ui::fizClientView),    
    m_newCl(newCl),    
    m_mapper(new QDataWidgetMapper(this)),    
    m_fizView(fizView)
{
    ui->setupUi(this);

    ui->lineSurname->setFocus(); //set focus to Surname field first

    fClientBE = new fizClient(modelFiz, modelMix, m_newCl, this);

    //"signal-slot" connection for save data in database by class of logic
    connect(this, &fizClientView::saveData, fClientBE, &fizClient::saveData);

    //"signal-slot" connection for sending-giving User's input data and check it
    connect(this, &fizClientView::sendInfoFromUser, fClientBE, &fizClient::giveInfoFromUser);

    //"signal-slot" connection for create error message when duplicates is found
    connect(fClientBE, &fizClient::duplicatesFound, this, &fizClientView::errMsgDuplicate);

    //"signal-slot" connection for close View window after DB was changed
    connect(fClientBE, &fizClient::closeView, this, &QDialog::close);

    //"signal-slot" connection for submit changes in DB
    connect(fClientBE, &fizClient::submitChanges, this, &fizClientView::submitChanges);

    //"signal-slot" connection for update fizClient View
    connect(fClientBE, &fizClient::updateFiz, this, &fizClientView::updateFiz);

    setMapper(modelFiz);
}

fizClientView::~fizClientView()
{
    delete ui;
}

void fizClientView::errMsgDuplicate()
{
    QMessageBox::warning(nullptr, QObject::tr("Ошибка"),
                         QObject::tr("Клиент уже существует в базе."
                                     "<BR><BR>"
                                     "Если вы уверены в необходимости добавления клиента, "
                                     "отредактируйте информацию в записи "
                                     "содержащей совпадение данных"
                                     "<BR><BR>"
                                     "Вы можете воспользоваться кнопкой"
                                     "<b> \"Поиск\"</b>"));
}

void fizClientView::submitChanges()
{
    m_mapper->submit();
}

void fizClientView::updateFiz()
{
    clientsFormView* form = qobject_cast<clientsFormView*>(this->parent());
    emit form->startUpdateFizView();
}

void fizClientView::on_buttonSaveFiz_clicked()
{
    if (ui->lineSurname->text().isEmpty())
    {
        QMessageBox::warning(nullptr, QObject::tr("Ошибка"),
                             QObject::tr("Необходимо указать"
                                         "<BR><BR>"
                                         "<b>\"Фамилию\"</b> клиента"));
        return;
    }
    else
    {
        emit sendInfoFromUser(QMap<QString, QVariant> {
                             {"SURNAME",  ui->lineSurname->text()},
                             {"NAME",     ui->lineName->text()},
                             {"MIDDLE",   ui->lineMiddle->text()},
                             {"TELM",     ui->lineTelM->text()},
                             {"EMAIL",    ui->lineEmail->text()},
                             {"PASSER",   ui->linePasSer->text()},
                             {"PASNUM",   ui->linePasNum->text()},
                             {"WHENIS",   ui->dateWhenIs->date()},
                             {"ISBYWHOM", ui->lineIsByWhom->text()},
                             {"PASCODE",  ui->linePasCode->text()},
                             {"TELW",     ui->lineTelW->text()},
                             {"IND",      ui->lineIndex->text()},
                             {"CITY",     ui->lineCity->text()},
                             {"STREET",   ui->lineStreet->text()},
                             {"BLD",      ui->lineBld->text()},
                             {"CORP",     ui->lineCorp->text()},
                             {"FLAT",     ui->lineFlat->text()},
                             {"NOTES",    ui->textNotes->toPlainText()},
                             {"BIRTHDAY", ui->dateBirthday->date()}
                             });
        emit saveData(m_fizView->currentIndex().row());
    }
}

void fizClientView::on_buttonCancelFiz_clicked()
{
    close();
}

void fizClientView::setMapper(QSqlTableModel* modelFiz)
{
    m_mapper->setModel(modelFiz);
    m_mapper->setSubmitPolicy(QDataWidgetMapper::ManualSubmit);

    m_mapper->addMapping(ui->lineSurname,  static_cast<int>(FIZ::SURNAME));
    m_mapper->addMapping(ui->lineName,     static_cast<int>(FIZ::NAME));
    m_mapper->addMapping(ui->lineMiddle,   static_cast<int>(FIZ::MIDDLE));
    m_mapper->addMapping(ui->dateBirthday, static_cast<int>(FIZ::BIRTHDAY));
    m_mapper->addMapping(ui->lineTelM,     static_cast<int>(FIZ::TELM));
    m_mapper->addMapping(ui->lineEmail,    static_cast<int>(FIZ::EMAIL));
    m_mapper->addMapping(ui->linePasSer,   static_cast<int>(FIZ::PASSER));
    m_mapper->addMapping(ui->linePasNum,   static_cast<int>(FIZ::PASNUM));
    m_mapper->addMapping(ui->dateWhenIs,   static_cast<int>(FIZ::WHENIS));
    m_mapper->addMapping(ui->lineIsByWhom, static_cast<int>(FIZ::ISBYWHOM));
    m_mapper->addMapping(ui->linePasCode,  static_cast<int>(FIZ::PASCODE));
    m_mapper->addMapping(ui->lineTelW,     static_cast<int>(FIZ::TELW));
    m_mapper->addMapping(ui->lineIndex,    static_cast<int>(FIZ::IND));
    m_mapper->addMapping(ui->lineCity,     static_cast<int>(FIZ::CITY));
    m_mapper->addMapping(ui->lineStreet,   static_cast<int>(FIZ::STREET));
    m_mapper->addMapping(ui->lineBld,      static_cast<int>(FIZ::BLD));
    m_mapper->addMapping(ui->lineCorp,     static_cast<int>(FIZ::CORP));
    m_mapper->addMapping(ui->lineFlat,     static_cast<int>(FIZ::FLAT));
    m_mapper->addMapping(ui->textNotes,    static_cast<int>(FIZ::NOTES));

    if (m_newCl)
    {
        setWindowTitle(QObject::tr("Добавление нового клиента"));
    }
    else
    {
        setWindowTitle(QObject::tr("Просмотр и редактирование информации о клиенте"));
        m_mapper->setCurrentIndex(m_fizView->currentIndex().row());
    }
}



