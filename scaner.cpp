#include "scaner.h"
#include <QDebug>
#include <QFile>
#include <fstream>
#include <libinsane/constants.h>
#include <libinsane/safebet.h>
#include <libinsane/util.h>

SCANER::SCANER(QObject* parent):
    QObject(parent),
    image(QImage())
{

}

QImage SCANER::initScan(QString res, QString mode)
{
    //progress bar
    emit progressChanged(0);

    struct lis_api* impl = nullptr; //LibInsane C API

    lis_safebet(&impl); //start lib work
    qDebug() << "Will use API" << impl->base_name; //This shows what API is using (SANE on Linux or other)

    struct lis_device_descriptor** dev_infos;

    impl->list_devices(impl, LIS_DEVICE_LOCATIONS_ANY, &dev_infos); //search for available devices
    if (!dev_infos[0])
    {
        emit noScanerFound();
        return QImage();
    }

    qDebug() << "Will use device" << dev_infos[0]->vendor << dev_infos[0]->model
             << dev_infos[0]->type <<dev_infos[0]->dev_id;
    const char* dev_id = dev_infos[0]->dev_id;

    struct lis_item* device = nullptr;

    impl->get_device(impl, dev_id, &device);

    struct lis_item** sources;
    device->get_children(device, &sources);

    lis_set_option(sources[0], OPT_NAME_RESOLUTION, qPrintable(res)); //scan resolution

    lis_set_option(sources[0], OPT_NAME_MODE, qPrintable(mode)); //scan mode

    struct lis_scan_session* scan_session;
    sources[0]->scan_start(sources[0], &scan_session);

    struct lis_scan_parameters parameters;

    scan_session->get_scan_parameters(scan_session, &parameters);

    std::ofstream out;
    bool bmp_ = true;
    QString fname = "fileName.bmp";
#ifdef Q_OS_LINUX
    bmp_ = false;
    fname = "fileName.pnm";
#endif

    if (bmp_)
    {
        out.open(qPrintable(fname), std::ios::out|std::ios::binary);

        int w = parameters.width;
        int h = -parameters.height;
        int filesize = 54 + 3 * w * h;

        unsigned char bmpfileheader[14] = {'B','M', 0,0,0,0, 0,0, 0,0, 54,0,0,0};
        unsigned char bmpinfoheader[40] = {40,0,0,0, 0,0,0,0, 0,0,0,0, 1,0, 24,0};

        bmpfileheader[ 2] = (unsigned char)(filesize    );
        bmpfileheader[ 3] = (unsigned char)(filesize>> 8);
        bmpfileheader[ 4] = (unsigned char)(filesize>>16);
        bmpfileheader[ 5] = (unsigned char)(filesize>>24);

        bmpinfoheader[ 4] = (unsigned char)(       w    );
        bmpinfoheader[ 5] = (unsigned char)(       w>> 8);
        bmpinfoheader[ 6] = (unsigned char)(       w>>16);
        bmpinfoheader[ 7] = (unsigned char)(       w>>24);
        bmpinfoheader[ 8] = (unsigned char)(       h    );
        bmpinfoheader[ 9] = (unsigned char)(       h>> 8);
        bmpinfoheader[10] = (unsigned char)(       h>>16);
        bmpinfoheader[11] = (unsigned char)(       h>>24);

        out.write(reinterpret_cast<char*>(bmpfileheader),14);
        out.write(reinterpret_cast<char*>(bmpinfoheader),40);
    }
    else
    {
        out.open(qPrintable(fname), std::ios::out|std::ios::binary);
        out << "P6\n" << parameters.width << " " << parameters.height << "\n" << 255 << "\n";
    }

    int padSize = (4 - (parameters.width * 3) % 4);
    unsigned char pad[3] = {0};
    char img_buffer [parameters.image_size / parameters.height];
    size_t bufsize = parameters.width * 3;

    int i = 0;
    int size = parameters.height + 1;

    while (!scan_session->end_of_page(scan_session))
    {
        scan_session->get_scan_parameters(scan_session, &parameters);
        scan_session->scan_read(scan_session, img_buffer, &bufsize);

        out.write(reinterpret_cast<char*>(img_buffer), bufsize);
        if (bmp_)
        {
            out.write(reinterpret_cast<char*>(pad), padSize);
        }

        i++;
        int currentPercent = 100 * i / size;
        emit progressChanged(currentPercent);
    }
    device->close(device);
    impl->cleanup(impl);
    out.close();

    image = QImage(fname);
    if (bmp_) image = image.rgbSwapped();
    QFile::remove(fname);

    return image;
}
