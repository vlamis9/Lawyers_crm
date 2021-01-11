#include "scanerview.h"
#include "ui_scanerview.h"
#include "imageviewer.h"
#include <QMessageBox>
#include <QPrinter>
#include <QPrintDialog>
#include <QPainter>

scanerView::scanerView(QWidget *parent):
    QDialog(parent),
    ui(new Ui::scanerView)
{
    ui->setupUi(this);

    this->setWindowTitle(QObject::tr("Сканирование документа"));
    this->setFixedSize(640, 380);
    ui->buttonSave->setEnabled(false);
    ui->buttonCancel->setEnabled(false);
    ui->buttonView->setEnabled(false);
    ui->progressBar->hide();

    ui->comboQuality->addItems(QStringList{"150",   "300",  "450"}); //px
    ui->comboColor->  addItems(QStringList{"Color", "Gray", "Lineart"}); //color mode

    m_scanBE = new SCANER(this);

    //"signal-slot" connection for create error message when no scan device was found
    connect(m_scanBE, &SCANER::noScanerFound, this, &scanerView::errNoScanerFound);

    //"signal-slot" connection for showing scan progress
    connect(m_scanBE, &SCANER::progressChanged, ui->progressBar, &QProgressBar::setValue);
}

scanerView::~scanerView()
{
    delete ui;
}

void scanerView::errNoScanerFound()
{
    QMessageBox::warning(nullptr, QObject::tr("Ошибка"),
                         QObject::tr("Не найдено сканирующее устройство."
                                     "<BR><BR>"
                                     "Проверьте правильность его подключения."));
}

void scanerView::on_buttonStartScan_clicked()
{
    ui->progressBar->show();
    image = m_scanBE->initScan(ui->comboQuality->currentText(), ui->comboColor->currentText());
    if (!image.isNull()) previewDoc();
    ui->progressBar->hide();
}

void scanerView::previewDoc()
{
    QPixmap fromImage = QPixmap::fromImage(image, Qt::AutoColor);
    ui->labelPreView->setPixmap(fromImage.scaled(QSize(ui->labelPreView->size()), Qt::KeepAspectRatio));
    ui->buttonCancel->setEnabled(true);
    ui->buttonSave->setEnabled(true);
    ui->buttonView->setEnabled(true);
}

void scanerView::on_buttonView_clicked()
{
    imageViewer* viewIm = new imageViewer(&image, this);
    viewIm->exec();
}

void scanerView::on_buttonSave_clicked()
{
    emit giveImageToSave(image);
    ui->buttonSave->setEnabled(false);
    on_buttonExit_clicked();
}

void scanerView::on_buttonCancel_clicked()
{
    ui->labelPreView->clear();
    ui->buttonSave->setEnabled(false);
    ui->buttonCancel->setEnabled(false);
    ui->buttonView->setEnabled(false);
}

void scanerView::on_buttonExit_clicked()
{
    close();
}

void scanerView::on_buttonPrintDoc_clicked()
{
    if (!image.isNull())
    {
        QPrinter printer;
        QPrintDialog* dlg = new QPrintDialog(&printer,0);
        if(dlg->exec() == QDialog::Accepted)
        {
            QPainter painter(&printer);
            QRect rect = painter.viewport();
            painter.drawImage(rect, image);
            painter.end();
        }
    }
}

