#include "reporter.h"
#include "ui_reporter.h"
#include "enums.h"

#include <QDateTime>
#include <QSqlQuery>
#include <QTextTableCell>
#include <QPrinter>
#include <QPrintDialog>
#include <QFileInfo>


reporter::reporter(int idCase, QWidget* parent):
    QDialog(parent),
    ui(new Ui::reporter),
    m_idCase(idCase)
{
    ui->setupUi(this);
    this->setWindowTitle(QObject::tr("Отчет"));

    QTextCursor cursor = ui->textBrowser->textCursor();
    cursor.movePosition(QTextCursor::Start);
    cursor.insertText(QObject::tr("Отчет"));
    cursor.insertBlock();
    cursor.insertText(QDateTime::currentDateTime().toString("dd.MM.yyyy hh:mm"));
    cursor.insertBlock();
    cursor.insertText(QObject::tr("КЛИЕНТ:"));
    cursor.insertBlock();

    QSqlQuery query;
    query.prepare("SELECT CLIENT_TYPE FROM mixClients WHERE ID_MIX = (SELECT ID_CLIENT FROM casesTable WHERE ID_CASE = :id);");
    query.bindValue(":id", idCase);
    query.exec();
    query.first();
    QString qStr;
    if (query.value(0).toInt() == static_cast<int>(Category::FIZ))
    {
        qStr = "SELECT (SURNAME || ' ' || NAME || ' ' || MIDDLE || ' ' || COALESCE(BIRTHDAY, '') ) "
               "FROM fizClient "
               "WHERE ID_FIZ = (SELECT ID_CLIENT FROM casesTable WHERE ID_CASE = :id);";
    }
    else
    {
        qStr = "SELECT (OPF || ' ' || COMPANY || ' ' || OGRN || ' ' || INN ) "
               "FROM yurClient "
               "WHERE ID_YUR = (SELECT ID_CLIENT FROM casesTable WHERE ID_CASE = :id);";
    }

    query.prepare(qStr);
    query.bindValue(":id", idCase);
    query.exec();
    query.first();
    QString strName = query.value(0).toString();
    cursor.insertText(strName);
    cursor.insertBlock();
    cursor.insertText(QObject::tr("ДЕЛО:"));
    cursor.insertBlock();
    query.prepare("SELECT (MAINPOINT || ' ' "
                  "|| CONTRACTNUM  || ' ' || CONTRACTSUM || ' ' || COALESCE(CASEDATE, '') ) "
                  "FROM casesTable WHERE ID_CASE = :id;");
    query.bindValue(":id", idCase);
    query.exec();
    query.first();
    QString strCase = query.value(0).toString();
    cursor.insertText(strCase);
    cursor.insertBlock();

    //set event's report
    cursor.insertText(QObject::tr("СОБЫТИЯ ПО ДЕЛУ:"));
    cursor.insertBlock();

    query.prepare("SELECT DATE_E, ENAME, EDET FROM events "
                  "WHERE ID_EVE IN (SELECT ID_EVE FROM caseEvents "
                  "WHERE ID_CASE = :id) ORDER BY DATE_E ASC;");

    query.bindValue(":id", idCase);
    query.exec();
    int initialPos = query.at();
    int pos = 0;
    if (query.last())
        pos = query.at() + 1;
    else
        pos = 0;
    //restore initial pos
    query.seek(initialPos);

    QTextTableFormat tableFormat;
    tableFormat.setAlignment(Qt::AlignCenter);
    tableFormat.setBorderStyle(QTextTableFormat::BorderStyle_Solid);
    tableFormat.setCellPadding(7);
    tableFormat.setCellSpacing(0);
    tableFormat.setWidth(QTextLength(QTextLength::PercentageLength, 100));

    QString arrHeaders[] = {QObject::tr("Дата соб."), QObject::tr("Название"), QObject::tr("Детали")};
    int size = sizeof(arrHeaders) / sizeof(*arrHeaders);

    QTextTable* table = cursor.insertTable(1, size, tableFormat);

    for (int col = 0; col < size; col++)
    {
        QTextTableCell cell = table->cellAt(0, col);
        QTextCursor cellCursor = cell.firstCursorPosition();
        QTextBlockFormat centerAlignment;
        centerAlignment.setAlignment(Qt::AlignHCenter|Qt::AlignVCenter);
        cellCursor.setBlockFormat(centerAlignment);
        QTextCharFormat boldFormat;
        boldFormat.setFontWeight(QFont::Bold);
        cellCursor.insertText(arrHeaders[col], boldFormat);
    }

    table->insertRows(table->rows(), 1);

    while (query.next())
    {
        QTextTableCell cell = table->cellAt(table->rows() - 1, 0);
        QTextCursor cellCursor = cell.firstCursorPosition();
        cellCursor.insertText(query.value(0).toDate().toString("dd.MM.yyyy"));
        cell = table->cellAt(table->rows() - 1, 1);
        cellCursor = cell.firstCursorPosition();
        cellCursor.insertText(query.value(1).toString());
        cell = table->cellAt(table->rows() - 1, 2);
        cellCursor = cell.firstCursorPosition();
        cellCursor.insertText(query.value(2).toString());
        if ((table->rows() - 1) == pos) break;
        table->insertRows(table->rows(), 1);
    }

    //set payment's report
    cursor.movePosition(QTextCursor::End);
    cursor.insertText(QObject::tr("ПЛАТЕЖИ ПО ДЕЛУ:"));
    cursor.insertBlock();

    query.prepare("SELECT DATE_PAY, SUM FROM payments "
                  "WHERE ID_CASE = :id ORDER BY DATE_PAY ASC;");

    query.bindValue(":id", idCase);
    query.exec();
    initialPos = query.at();
    pos = 0;
    if (query.last())
        pos = query.at() + 1;
    else
        pos = 0;
    //restore initial pos
    query.seek(initialPos);

    QString arrHeadersPay[] = {QObject::tr("Дата платежа"), QObject::tr("Сумма платежа")};
    size = sizeof(arrHeadersPay) / sizeof(*arrHeadersPay);

   table = cursor.insertTable(1, size, tableFormat);

   for (int col = 0; col < size; col++)
   {
       QTextTableCell cell = table->cellAt(0, col);
       QTextCursor cellCursor = cell.firstCursorPosition();
       QTextBlockFormat centerAlignment;
       centerAlignment.setAlignment(Qt::AlignHCenter|Qt::AlignVCenter);
       cellCursor.setBlockFormat(centerAlignment);
       QTextCharFormat boldFormat;
       boldFormat.setFontWeight(QFont::Bold);
       cellCursor.insertText(arrHeadersPay[col], boldFormat);
   }

   table->insertRows(table->rows(), 1);

   while (query.next())
   {
       QTextTableCell cell = table->cellAt(table->rows() - 1, 0);
       QTextCursor cellCursor = cell.firstCursorPosition();
       cellCursor.insertText(query.value(0).toDate().toString("dd.MM.yyyy"));
       cell = table->cellAt(table->rows() - 1, 1);
       cellCursor = cell.firstCursorPosition();
       cellCursor.insertText(query.value(1).toString());
       if ((table->rows() - 1) == pos) break;
       table->insertRows(table->rows(), 1);
   }
}

reporter::~reporter()
{
    delete ui;
}

void reporter::on_buttonPrint_clicked()
{
    QPrinter printer(QPrinter::HighResolution);
    QPrintDialog* dlg = new QPrintDialog(&printer,0);
    if(dlg->exec() == QDialog::Accepted)
    {
        ui->textBrowser->print(&printer);
    }
}

void reporter::on_buttonSave_clicked()
{
    QPrinter printer;
    printer.setOutputFormat(QPrinter::PdfFormat);
    QString strFName = "Report_" + QDateTime::currentDateTime().toString("dd.MM.yyyy_hh_mm") + ".pdf";
    printer.setOutputFileName(strFName);
    ui->textBrowser->print(&printer);

    QFile file(strFName);
    if (!file.open(QIODevice::ReadOnly)) return;
    QByteArray inByteArray = file.readAll();
    QFileInfo fi(strFName);

    //to documents table
    QSqlQuery query;
    query.prepare("INSERT INTO documents (CAT_DOC, CAT_OWNER, FNAME, FDATE, FSIZE) "
                  "VALUES (:cat_doc, :cat_own, :fname, :fdate, :fsize);");
    query.bindValue(":cat_doc", static_cast<int>(DocViews::TEXT));
    query.bindValue(":cat_own", static_cast<int>(OwnerCat::CASE));
    query.bindValue(":fname",   fi.fileName());
    query.bindValue(":fdate",   fi.metadataChangeTime().toString(Qt::SystemLocaleShortDate));
    QLocale locale;
    QString valueText = locale.formattedDataSize(fi.size());
    query.bindValue(":fsize",   valueText);
    query.exec();

    //to files table
    QString strID = "SELECT ID_DOC "
                    "FROM documents "
                    "WHERE ROWID = last_insert_rowid() ";
    query.exec(strID);
    query.first();
    int id = query.value(0).toInt();
    query.prepare("INSERT INTO files (ID_FILE, FILE) VALUES (:idF, :file);");
    query.bindValue(":idF",  id);
    query.bindValue(":file", inByteArray);
    query.exec();

    //to owner's table
    query.prepare("INSERT INTO caseDocs (ID_DOC, ID_CASE) VALUES (:idD, :idC);");
    query.bindValue(":idD",  id);
    query.bindValue(":idC", m_idCase);
    query.exec();

    file.close();
    //file.remove(strFName);
    QFile::remove(strFName);
}

void reporter::on_buttonExit_clicked()
{
    close();
}
