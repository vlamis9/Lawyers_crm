#ifndef REPORTER_H
#define REPORTER_H

#include <QDialog>

namespace Ui {
class reporter;
}

class reporter : public QDialog
{
    Q_OBJECT

public:
    explicit reporter(int idCase, QWidget* parent = nullptr);
    ~reporter();

private slots:
    void on_buttonPrint_clicked();
    void on_buttonSave_clicked();
    void on_buttonExit_clicked();

private:
    Ui::reporter *ui;
    int m_idCase;
};

#endif // REPORTER_H
