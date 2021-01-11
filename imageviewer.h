#ifndef IMAGEVIEWER_H
#define IMAGEVIEWER_H

#include <QDialog>

class QScrollBar;
class QLabel;

namespace Ui {
class imageViewer;
}

class imageViewer : public QDialog
{
    Q_OBJECT

public:
    explicit imageViewer(QImage*, QWidget* parent = nullptr);
    ~imageViewer();

private:
    void adjustScrollBar(QScrollBar*, double);

    Ui::imageViewer *ui;
    QImage* m_image;
    qreal scaleFactor;
    QLabel* label;

private slots:
    void on_buttonZoomIN_clicked();
    void on_buttonZoomOUT_clicked();
    void on_buttonFitToW_clicked();
};

#endif // IMAGEVIEWER_H
