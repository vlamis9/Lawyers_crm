#ifndef SCANERVIEW_H
#define SCANERVIEW_H

#include <QDialog>
#include "scaner.h"

namespace Ui {
class scanerView;
}

class scanerView : public QDialog
{
    Q_OBJECT

public:
    explicit scanerView(QWidget* parent = nullptr);
    ~scanerView();

signals:
    void giveImageToSave(QImage);

public slots:
    void errNoScanerFound();

private slots:
    void on_buttonStartScan_clicked();
    void on_buttonView_clicked();
    void on_buttonSave_clicked();
    void on_buttonCancel_clicked();
    void on_buttonExit_clicked();
    void on_buttonPrintDoc_clicked();

private:
    void previewDoc();

    Ui::scanerView* ui;
    SCANER* m_scanBE; //pointer to scaner logic class
    QImage image;
};

#endif // SCANERVIEW_H
