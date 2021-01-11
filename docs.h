#ifndef DOCS_H
#define DOCS_H

#include "enums.h"
#include <QObject>
#include <QModelIndexList>

class QSqlTableModel;

class Docs : public QObject
{
    Q_OBJECT
public:
    explicit Docs(QObject* parent = nullptr);

    QByteArray getFile(const QModelIndex&);
    void openTextDoc(const QModelIndex&, bool, int = 0, int = 0);
    void newTextDoc(bool, int = 0, int = 0);
    QMap<int, QString> getClientsForView();
    QMap<int, QString> getCasesForView();

public slots:
    void startUpdateModel(DocViews, bool, int = 0, int = 0);
    void addDocs(DocViews, bool, int = 0, int = 0);
    void addScanOrText(QByteArray, DocViews, bool, int = 0, int = 0);
    void delDocs(QModelIndexList,  DocViews, bool, int = 0, int = 0);
    void saveDocs(QModelIndexList, DocViews, bool, int = 0, int = 0);
    void renameDocs(QModelIndex, QString, DocViews, bool, int = 0, int = 0);

signals:
    void finishUpdateModel(QSqlTableModel*, DocViews);
    void startMessageTextWarning();
    void endMessageTextWarning();

private:
    QSqlTableModel* m_modelDocs;
};

#endif // DOCS_H
