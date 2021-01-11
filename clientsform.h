#ifndef CLIENTSFORM_H
#define CLIENTSFORM_H

#include <QObject>
#include "enums.h"

class QSqlTableModel;

class clientsForm : public QObject
{
    Q_OBJECT
public:
    explicit clientsForm(QObject* parent = nullptr);
    void deleteClient(int, enum Category);

    QSqlTableModel* getModelFiz();
    QSqlTableModel* getModelYur();
    QSqlTableModel* getModelMix();
    void searchClientsFiz(QString);
    void searchClientsYur(QString);

signals:
    void finishUpdateFizModel(QSqlTableModel*, int);
    void finishUpdateYurModel(QSqlTableModel*, int);

public slots:
    void startUpdateFizModel();
    void startUpdateYurModel();

private:
    QSqlTableModel* m_modelFiz;
    QSqlTableModel* m_modelYur;
    QSqlTableModel* m_modelMix;
};

#endif // CLIENTSFORM_H
