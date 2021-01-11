#ifndef SCANER_H
#define SCANER_H

#include <QObject>
#include <QImage>

class SCANER : public QObject
{
    Q_OBJECT
public:
    explicit SCANER(QObject* parent = nullptr);

signals:
    void noScanerFound();
    void progressChanged(int);

public:
    QImage initScan(QString, QString);

private:
    QImage image;
};

#endif // SCANER_H
