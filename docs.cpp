#include "docs.h"
#include "popup.h"
#include <QSqlQuery>
#include <QSqlTableModel>
#include <QFileDialog>
#include <QSqlRecord>
#include <QDateTime>
#include <QProcess>
#include <QApplication>
#include <QTextDocument>
#include <QTextDocumentWriter>
#include <QThread>

Docs::Docs(QObject* parent):
    QObject(parent),
    m_modelDocs(new QSqlTableModel(this))
{

}

QByteArray Docs::getFile(const QModelIndex& ind)
{
    QSqlRecord record = m_modelDocs->record(ind.row());
    int idDoc = record.value(m_modelDocs->fieldIndex("ID_DOC")).toInt();
    QSqlQuery query;
    query.prepare("SELECT FILE FROM files WHERE ID_FILE = :id");
    query.bindValue(":id", idDoc);
    query.exec();
    query.first();
    return QByteArray(query.value(0).toByteArray());
}

void Docs::openTextDoc(const QModelIndex& textDocId, bool isUser, int idCase, int idClient)
{
    QSqlRecord record = m_modelDocs->record(textDocId.row());
    int idDoc = record.value(m_modelDocs->fieldIndex("ID_DOC")).toInt();
    QString strFName = record.value(m_modelDocs->fieldIndex("FNAME")).toString();
    QByteArray arrText = getFile(textDocId);
    QFile file(strFName);
    if (!file.open(QFile::WriteOnly)) return;
    else
    {
        file.seek(0);
        file.write(arrText);
        file.close();
    }
    emit startMessageTextWarning();
    QProcess proc;

    QString strProc = "explorer " + strFName;

#ifdef Q_OS_LINUX
    strProc = "xdg-open " + strFName;
#endif

    proc.start(strProc);
    //sleep
    QThread::sleep(5); //5 seconds wait to open default app
    emit endMessageTextWarning();
    QByteArray inByteArray;
    if (!file.open(QFile::ReadOnly)) return;
    else
    {
        file.seek(0);
        inByteArray = file.readAll();
        file.close();
    }
    file.remove(strFName);

    QSqlQuery query;
    query.prepare("UPDATE files SET FILE = :fileData WHERE ID_FILE = :idF;");
    query.bindValue(":fileData", inByteArray);
    query.bindValue(":idF", idDoc);
    query.exec();

    query.prepare("UPDATE documents SET FSIZE = :fileSize WHERE ID_DOC = :idF;");
    QLocale locale;
    QString valueText = locale.formattedDataSize(inByteArray.size());
    query.bindValue(":fileSize", valueText);
    query.bindValue(":idF", idDoc);
    query.exec();

    startUpdateModel(DocViews::TEXT, isUser, idCase, idClient);
}

void Docs::newTextDoc(bool isUser, int idCase, int idClient)
{
    QTextDocumentWriter writer;
    QTextDocument doc("\t");
    writer.setFormat("PlainText");
    QString fname = "output.docx";
    writer.setFileName(fname);
    writer.write(&doc);
    QFile file(fname);
    emit startMessageTextWarning();

    QProcess proc;
    QString strProc = "explorer " + fname;

#ifdef Q_OS_LINUX
    strProc = "xdg-open " + fname;
#endif
    proc.start(strProc);
    //sleep
    QThread::sleep(5); //5 seconds wait to open default app
    emit endMessageTextWarning();
    QByteArray inByteArray;
    if (!file.open(QFile::WriteOnly | QFile::ReadOnly)) return;
    else
    {
        inByteArray = file.readAll();
        file.close();
    }
    file.remove(fname);
    addScanOrText(inByteArray, DocViews::TEXT, isUser, idCase, idClient);
}

void Docs::startUpdateModel(DocViews dv, bool isUser, int idCase, int idClient)
{
    m_modelDocs->setTable("documents");
    QStringList caseHeaders { QObject::tr("Имя файла"),   QObject::tr("Дата файла"),
                              QObject::tr("Размер файла")};
    int ind = 3;
    for (const auto& header : caseHeaders)
    {
        m_modelDocs->setHeaderData(ind, Qt::Horizontal, qPrintable(header));
        ind++;
    }
    QString catStr  = QString::number(static_cast<int>(dv));
    QString userStr = QString::number(static_cast<int>(OwnerCat::USER));
    QString caseStr = QString::number(static_cast<int>(OwnerCat::CASE));
    QString clStr   = QString::number(static_cast<int>(OwnerCat::CLIENT));

    if (isUser)
    {
        m_modelDocs->setFilter("CAT_DOC = '" + catStr + "' AND CAT_OWNER = '" + userStr + "'");
    }
    else if (idCase)
    {
        m_modelDocs->setFilter("CAT_DOC = '" + catStr + "' AND CAT_OWNER = '" + caseStr + "' AND ID_DOC IN "
                               "(SELECT ID_DOC FROM caseDocs "
                               "WHERE ID_CASE ='" + QString::number(idCase) + "')");
    }
    else
    {
        m_modelDocs->setFilter("CAT_DOC = '" + catStr + "' AND CAT_OWNER = '" + clStr + "' AND ID_DOC IN "
                               "(SELECT ID_DOC FROM clientDocs "
                               "WHERE ID_CLIENT ='" + QString::number(idClient) + "')");
    }
    m_modelDocs->select();
    emit finishUpdateModel(m_modelDocs, dv);
}

void Docs::addDocs(DocViews dv, bool isUser, int idCase, int idClient)
{
    m_modelDocs->setTable("documents");
    QString filterStr;
    if (dv == DocViews::FOTO)
    {
        filterStr = QObject::tr("Картинки (*.bmp *.gif *.jpg *.jpeg *.JPG *.png)");
    }
    else if (dv == DocViews::AUDIO)
    {
        filterStr = QObject::tr("Аудио (*.mp3 *.wav)");
    }
    else
    {
        filterStr = QObject::tr("Текст. документы (*.docx *.txt *.pdf *.odt *.doc *odf)");
    }

    QSqlTableModel* modelFiles = new QSqlTableModel(this);
    modelFiles->setTable("files");

    QSqlTableModel* modelCategory = new QSqlTableModel(this);

    QFileDialog* dialog = new QFileDialog(qobject_cast<QWidget*>(this));
    dialog->setFileMode(QFileDialog::ExistingFiles);
    dialog->setNameFilter(filterStr);

    QStringList fileNames;
    if (dialog->exec())
    {
        fileNames = dialog->selectedFiles();
    } else return;

    int catOwner;
    if (isUser)
    {
        modelCategory->setTable("userDocs");
        catOwner = static_cast<int>(OwnerCat::USER);
    }
    else if (idCase)
    {
        modelCategory->setTable("caseDocs");
        catOwner = static_cast<int>(OwnerCat::CASE);
    }
    else
    {
        modelCategory->setTable("clientDocs");
        catOwner = static_cast<int>(OwnerCat::CLIENT);
    }

    for (QString filename : fileNames)
    {
        QFile file(filename);
        if (!file.open(QIODevice::ReadOnly)) return;
        QByteArray inByteArray = file.readAll();
        if (dv == DocViews::FOTO)
        {
            QPixmap outPixmap = QPixmap();
            if (!outPixmap.loadFromData( inByteArray )) return;
        }
        QFileInfo fi(filename);
        QSqlRecord record = m_modelDocs->record();
        record.setValue("CAT_DOC",   static_cast<int>(dv));
        record.setValue("CAT_OWNER", catOwner);
        record.setValue("FNAME", fi.fileName());
        record.setValue("FDATE", fi.metadataChangeTime().toString(Qt::SystemLocaleShortDate));
        QLocale locale;
        QString valueText = locale.formattedDataSize(fi.size());
        record.setValue("FSIZE", valueText);
        m_modelDocs->insertRecord(-1, record);

        //to files table
        record = modelFiles->record();
        QString strID = "SELECT ID_DOC "
                        "FROM documents "
                        "WHERE ROWID = last_insert_rowid() ";
        QSqlQuery query(strID);
        query.exec();
        query.first();
        int id = query.value(0).toInt();
        record.setValue("ID_FILE", id);
        record.setValue("FILE", inByteArray);
        modelFiles->insertRecord(-1, record);

        //to owner's table
        record = modelCategory->record();
        record.setValue("ID_DOC", id);
        if (idCase) record.setValue("ID_CASE", idCase);
        if (idClient) record.setValue("ID_CLIENT", idClient);
        modelCategory->insertRecord(-1, record);
    }
    startUpdateModel(dv, isUser, idCase, idClient);
}

void Docs::addScanOrText(QByteArray imageArr, DocViews dv, bool isUser, int idCase, int idClient)
{
    m_modelDocs->setTable("documents");
    QSqlTableModel* modelFiles = new QSqlTableModel(this);
    modelFiles->setTable("files");
    QSqlTableModel* modelCategory = new QSqlTableModel(this);
    int catOwner;
    if (isUser)
    {
        modelCategory->setTable("userDocs");
        catOwner = static_cast<int>(OwnerCat::USER);
    }
    else if (idCase)
    {
        modelCategory->setTable("caseDocs");
        catOwner = static_cast<int>(OwnerCat::CASE);
    }
    else
    {
        modelCategory->setTable("clientDocs");
        catOwner = static_cast<int>(OwnerCat::CLIENT);
    }

    QString strfName;
    int catDoc;
    if (dv == DocViews::FOTO)
    {
        strfName = "ScanDoc_" + QDateTime::currentDateTime(). \
                toString(Qt::SystemLocaleShortDate) + QString(".jpg");
        catDoc = static_cast<int>(DocViews::FOTO);
    }
    else
    {
        strfName = "TextDoc_" + QDateTime::currentDateTime(). \
                toString("dd_MM_yyyy") + QString(".docx");
        catDoc = static_cast<int>(DocViews::TEXT);
    }
    QSqlRecord record = m_modelDocs->record();
    record.setValue("CAT_DOC",   catDoc);
    record.setValue("CAT_OWNER", catOwner);
    record.setValue("FNAME", strfName);
    record.setValue("FDATE", QDateTime::currentDateTime().toString(Qt::SystemLocaleShortDate));
    QLocale locale;
    QString valueText = locale.formattedDataSize(imageArr.size());
    record.setValue("FSIZE", valueText);
    m_modelDocs->insertRecord(-1, record);

    //to files table
    record = modelFiles->record();
    QString strID = "SELECT ID_DOC "
                    "FROM documents "
                    "WHERE ROWID = last_insert_rowid() ";
    QSqlQuery query(strID);
    query.exec();
    query.first();
    int id = query.value(0).toInt();
    record.setValue("ID_FILE", id);
    record.setValue("FILE", imageArr);
    modelFiles->insertRecord(-1, record);

    //to owner's table
    record = modelCategory->record();
    record.setValue("ID_DOC", id);
    if (idCase) record.setValue("ID_CASE", idCase);
    if (idClient) record.setValue("ID_CLIENT", idClient);
    modelCategory->insertRecord(-1, record);

    startUpdateModel(dv, isUser, idCase, idClient);

    PopUp* pop = new PopUp();
    pop->setAttribute(Qt::WA_DeleteOnClose);
    pop->setPopupText(QObject::tr("Документ ") + strfName + " успешно добавлен");
    pop->show();
}

void Docs::delDocs(QModelIndexList indList, DocViews dv, bool isUser, int idCase, int idClient)
{    
    for (QModelIndex line : indList)
    {
        //delete from documents with cascade delete from clientDocs and files
        m_modelDocs->removeRow(line.row());
    }
    startUpdateModel(dv, isUser, idCase, idClient);
}

void Docs::saveDocs(QModelIndexList indList, DocViews dv, bool isUser, int idCase, int idClient)
{
    QFileDialog* dialog = new QFileDialog(); //
    //QString strDir = QFileDialog::getExistingDirectory(0, "Выбор папки для сохранения файлов", "");
    for (QModelIndex line : indList)
    {
        QSqlRecord record = m_modelDocs->record(line.row());
        QString filename = record.value(static_cast<int>(File::FNAME)).toString();
        int idFile = record.value(static_cast<int>(File::ID_DOC)).toInt();
        QSqlQuery query;
        query.prepare("SELECT FILE FROM files WHERE ID_FILE = :idF");
        query.bindValue(":idF", idFile);
        query.exec();
        query.first();
        QByteArray arrF = query.value(0).toByteArray();
        dialog->setWindowModality(Qt::WindowModal);
        dialog->setAcceptMode(QFileDialog::AcceptSave);
        filename = dialog->getSaveFileName(nullptr, tr("Save File"),
                   filename, tr("Files"));
        QFile newDoc(filename);
        if(newDoc.open(QIODevice::WriteOnly))
        {
            newDoc.write(arrF);
        }
        newDoc.close();
    }
    startUpdateModel(dv, isUser, idCase, idClient);
}

void Docs::renameDocs(QModelIndex ind, QString strToRen, DocViews dv, bool isUser, int idCase, int idClient)
{
    QSqlRecord record = m_modelDocs->record(ind.row());
    QString filename = record.value(static_cast<int>(File::FNAME)).toString();
    int idFile = record.value(static_cast<int>(File::ID_DOC)).toInt();
    //get the extension of file
    QString strExtFile = filename.mid(filename.lastIndexOf(".") + 1);
    strExtFile = strExtFile.toLower();
    QSqlQuery query;
    query.prepare("UPDATE documents SET FNAME = :newFName WHERE ID_DOC = :idF;");
    query.bindValue(":newFName", strToRen + "." + strExtFile);
    query.bindValue(":idF", idFile);
    query.exec();
    startUpdateModel(dv, isUser, idCase, idClient);
}

QMap<int, QString> Docs::getCasesForView()
{
    QSqlQuery query;
    query.prepare("SELECT (ID_CASE || ' ' || CASECAT || ' ' || MAINPOINT || ' ' "
                  "|| CONTRACTNUM  || ' ' || COALESCE(CASEDATE, '') ) "
                  "FROM casesTable ");
    query.exec();
    QMap<int, QString> mapInfo;
    while (query.next())
    {        
        QString strIdToDelete = query.value(0).toString().section(" ", 0, 0, QString::SectionSkipEmpty);

        int index = strIdToDelete.toInt();
        //remove only ID, not other same strings
        mapInfo[index] = query.value(0).toString().remove(0, strIdToDelete.size());
        mapInfo[index] = mapInfo[index].trimmed();
        //get date
        QVariant strDate = query.value(0).toString().right(10);
        QDate d = strDate.toDate();
        //set date to ok format
        QString strFormattedDate = d.toString(Qt::SystemLocaleShortDate);
        //change date in map
        mapInfo[index].remove(query.value(0).toString().right(10));
        mapInfo[index].append(strFormattedDate);
        //set case category to string format
        strIdToDelete = mapInfo[index].section(" ", 0, 0, QString::SectionSkipEmpty);
        mapInfo[index].replace(0, strIdToDelete.size(), caseCatArr[strIdToDelete.toInt()]);
    }
    return mapInfo;
}

QMap<int, QString> Docs::getClientsForView()
{
    QSqlQuery query;
    query.prepare("SELECT (ID_FIZ || ' ' || SURNAME || ' ' || NAME || ' ' || MIDDLE || ' ' || COALESCE(BIRTHDAY, '') ) "
                  "FROM fizClient "
                  "UNION "
                  "SELECT (ID_YUR || ' ' || OPF || ' ' || COMPANY || ' ' || OGRN || ' ' || INN) "
                  "FROM yurClient ");
    query.exec();
    QMap<int, QString> mapInfo;
    while (query.next())
    {
        QString strIdToDelete = query.value(0).toString().section(" ", 0, 0, QString::SectionSkipEmpty);
        QVariant strDate = query.value(0).toString().right(10);
        QDate d = strDate.toDate();
        QString strFormattedDate;
        if (d.isValid())
        {
            strFormattedDate = d.toString(Qt::SystemLocaleShortDate);
            int index = strIdToDelete.toInt();
            mapInfo[index] = query.value(0).toString().remove(0, strIdToDelete.size());
            mapInfo[index] = mapInfo[index].trimmed();
            //change date in map
            mapInfo[index].remove(query.value(0).toString().right(10));
            mapInfo[index].append(strFormattedDate);
        }
        else
        {
            int index = strIdToDelete.toInt();
            mapInfo[index] = query.value(0).toString().remove(0, strIdToDelete.size());
            mapInfo[index] = mapInfo[index].trimmed();
        }
    }
    return mapInfo;
}

