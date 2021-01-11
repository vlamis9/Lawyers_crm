#include "caseinfoview.h"
#include "ui_caseinfoview.h"
#include "enums.h"

#include <QSqlTableModel>
#include <QTableView>
#include <QDataWidgetMapper>

caseInfoView::caseInfoView(QSqlTableModel* modelCase, QTableView* casesView,
                           bool newCase, QWidget* parent):
    QDialog(parent),
    ui(new Ui::caseInfoView),
    m_newCase(newCase),
    m_mapper(new QDataWidgetMapper(this)),
    m_casesView(casesView)
{
    ui->setupUi(this);

    //set current date
    ui->dateCaseDate->setDate(QDate::currentDate());

    cInfoBE = new caseInfo(modelCase, m_newCase, this);

    //"signal-slot" connection for save data in database by class of logic
    connect(this, &caseInfoView::saveData, cInfoBE, &caseInfo::saveData);

    //"signal-slot" connection for sending-giving User's input data and check it
    connect(this, &caseInfoView::sendInfoFromUser, cInfoBE, &caseInfo::giveInfoFromUser);

    //"signal-slot" connection for close View window after DB was changed
    connect(cInfoBE, &caseInfo::closeView, this, &QDialog::close);

    //"signal-slot" connection for submit changes in DB
    connect(cInfoBE, &caseInfo::submitChanges, this, &caseInfoView::submitChanges);

    caseMap =   {{static_cast<int>(CaseCategory::CIVIL),  QObject::tr("гражданское")},
                 {static_cast<int>(CaseCategory::CRIME),  QObject::tr("уголовное")},
                 {static_cast<int>(CaseCategory::ADMIN),  QObject::tr("административное")},
                 {static_cast<int>(CaseCategory::ARBITR), QObject::tr("арбитраж")},
                 {static_cast<int>(CaseCategory::CAS),    QObject::tr("кодекс админ. судопр-ва")},
                 {static_cast<int>(CaseCategory::OTHER),  QObject::tr("иное")}};

    clientMap = {{static_cast<int>(ClientStatus::APPLICANT),  QObject::tr("истец")},
                 {static_cast<int>(ClientStatus::RESPONDER),  QObject::tr("ответчик")},
                 {static_cast<int>(ClientStatus::THIRD),      QObject::tr("третье лицо")},
                 {static_cast<int>(ClientStatus::ACCUSED),    QObject::tr("обвиняемый")},
                 {static_cast<int>(ClientStatus::VICTIM),     QObject::tr("потерпевший")},
                 {static_cast<int>(ClientStatus::ATTRACTED),  QObject::tr("привл. к админ. отв.")},
                 {static_cast<int>(ClientStatus::ADMINAPP),   QObject::tr("админ. истец")},
                 {static_cast<int>(ClientStatus::OTHER),      QObject::tr("иное")}};

    initInfo();
    setMapper(modelCase);
}

caseInfoView::~caseInfoView()
{
    delete ui;
}

void caseInfoView::submitChanges()
{
    m_mapper->submit();
}

void caseInfoView::showLabelCatCl(int cat)
{
    cInfoBE->getLabelClient(indexesCombo.value(cat)) ?
             ui->labelCategoryCl->setText(QObject::tr("Юридическое лицо")) :
             ui->labelCategoryCl->setText(QObject::tr("Физическое лицо"));
}

void caseInfoView::setMapper(QSqlTableModel* modelCase)
{
    m_mapper->setModel(modelCase);
    m_mapper->setSubmitPolicy(QDataWidgetMapper::ManualSubmit);
    m_mapper->addMapping(ui->textRulesOfLaw,     static_cast<int>(CASE::RULESOFLAW));
    m_mapper->addMapping(ui->lineContractNum,    static_cast<int>(CASE::CONTRACTNUM));
    m_mapper->addMapping(ui->lineContractSum,    static_cast<int>(CASE::CONTRACTSUM));
    m_mapper->addMapping(ui->dateCaseDate,       static_cast<int>(CASE::CASEDATE));
    m_mapper->addMapping(ui->linePayDate,        static_cast<int>(CASE::PAYDATE));
    m_mapper->addMapping(ui->textMainPoint,      static_cast<int>(CASE::MAINPOINT));
    m_mapper->addMapping(ui->textCaseParts,      static_cast<int>(CASE::CASEPARTS));
    m_mapper->addMapping(ui->textNotes,          static_cast<int>(CASE::NOTES));

    if (m_newCase)
    {
        setWindowTitle(QObject::tr("Добавление нового дела"));
    }
    else
    {
        setWindowTitle(QObject::tr("Просмотр и редактирование информации о деле"));
        m_mapper->setCurrentIndex(m_casesView->currentIndex().row());
    }
}

void caseInfoView::initInfo()
{
    for (const auto& key : caseMap.keys())
    {
        ui->comboCategoryCase->addItem(caseMap.value(key));
    }
    for (const auto& key : clientMap.keys())
    {
        ui->comboClientStatus->addItem(clientMap.value(key));
    }

    QMap<int, QString> mapInfo = cInfoBE->getTableConcat();
    int indCombo = 0;

    for (const auto& key : mapInfo.keys())
    {
        ui->comboClientName->addItem(mapInfo.value(key));
        indexesCombo[indCombo] = key;
        indCombo++;
    }
    showLabelCatCl(ui->comboClientName->currentIndex());
    connect(ui->comboClientName, qOverload<int>(&QComboBox::currentIndexChanged),
                                 this, &caseInfoView::showLabelCatCl);

    if (!m_newCase)
    {
        //get Client's id
        int curROWindex  = m_casesView->currentIndex().row();
        int idClient = m_casesView->currentIndex().sibling(curROWindex,
                                                   static_cast<int>(CASE::ID_CLIENT)).data().toInt();

        //set combo box with Client's name and forbid to choose
        ui->comboClientName->setCurrentIndex(indexesCombo.key(idClient));
        ui->comboClientName->setEnabled(false);

        //get case' category
        int caseCat = m_casesView->currentIndex().sibling(curROWindex,
                                                  static_cast<int>(CASE::CASECAT)).data().toInt();
        //set combo box with case categories
        ui->comboCategoryCase->setCurrentIndex(caseCat);

        //get client' status
        int clStatus = m_casesView->currentIndex().sibling(curROWindex,
                                                  static_cast<int>(CASE::CLIENTSTATUS)).data().toInt();
        //set combo box with client status
        ui->comboClientStatus->setCurrentIndex(clStatus);
    }
}

void caseInfoView::on_buttonCancelCase_clicked()
{
    close();
}

void caseInfoView::on_buttonSaveCase_clicked()
{
    emit sendInfoFromUser(QMap<QString, QVariant> {
                         {"ID_CLIENT",    indexesCombo.value(ui->comboClientName->currentIndex())},
                         {"CASECAT",      ui->comboCategoryCase->currentIndex()},
                         {"MAINPOINT",    ui->textMainPoint->toPlainText()},
                         {"CLIENTSTATUS", ui->comboClientStatus->currentIndex()},
                         {"RULESOFLAW",   ui->textRulesOfLaw->toPlainText()},
                         {"CASEPARTS",    ui->textCaseParts->toPlainText()},
                         {"PAYDATE",      ui->linePayDate->text()},
                         {"CONTRACTNUM",  ui->lineContractNum->text()},
                         {"CONTRACTSUM",  ui->lineContractSum->text()},
                         {"CASEDATE",     ui->dateCaseDate->date()},
                         {"NOTES",        ui->textNotes->toPlainText()},
                         {"CLIENTNAME",   ui->comboClientName->currentText()},
                         });
    emit saveData(m_casesView->currentIndex().row());
}

