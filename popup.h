#ifndef POPUP_H
#define POPUP_H

#include <QWidget>
#include <QLabel>
#include <QGridLayout>
#include <QPropertyAnimation>

class QTimer;

class PopUp : public QWidget
{
    Q_OBJECT

    //opacity property
    Q_PROPERTY(float popupOpacity READ getPopupOpacity WRITE setPopupOpacity)

    void  setPopupOpacity(float opacity);
    float getPopupOpacity() const;

public:
    explicit PopUp(QWidget* parent = nullptr);

protected:
    void paintEvent(QPaintEvent*);

public slots:
    void setPopupText(const QString&);
    void show();

private slots:
    void hideAnimation();
    void hide();

private:
    QLabel label;
    QGridLayout layout;
    QPropertyAnimation animation;
    float popupOpacity;
    QTimer* timer;
};

#endif // POPUP_H
