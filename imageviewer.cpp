#include "imageviewer.h"
#include "ui_imageviewer.h"
#include <QLabel>

imageViewer::imageViewer(QImage* image, QWidget* parent):
    QDialog(parent),
    ui(new Ui::imageViewer),
    m_image(image)
{
    ui->setupUi(this);
    this->setWindowTitle(QObject::tr("Просмотр изображения"));

    scaleFactor = 1.05;

    label = new QLabel(ui->scrollArea);
    label->setScaledContents(true);
    label->setPixmap(QPixmap::fromImage(*image));

    ui->scrollArea->setWidgetResizable(false);
    ui->scrollArea->setWidget(label);
}

imageViewer::~imageViewer()
{
    delete ui;
}

void imageViewer::on_buttonZoomIN_clicked()
{
   label->resize(scaleFactor * label->size());
}

void imageViewer::on_buttonZoomOUT_clicked()
{
   label->resize(label->size() / scaleFactor);
}

void imageViewer::on_buttonFitToW_clicked()
{
    label->adjustSize();
}
