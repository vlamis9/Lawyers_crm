#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "reminder.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class QSqlTableModel;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget* parent = nullptr);
    ~MainWindow();

private slots:
    void on_buttonCases_clicked();
    void on_buttonClients_clicked();
    void on_buttonDocs_clicked();
    void on_buttonTasks_clicked();

private:
    void init(); // init actions (DB connection, make tables...)

    Ui::MainWindow *ui;
    REMINDER* reminder; //class to remind of events
};
#endif // MAINWINDOW_H
