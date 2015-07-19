#ifndef VIDEOPLAYER_H
#define VIDEOPLAYER_H

#include <QThread>
#include <QTimer>
#include <QImage>
#include <QFile>
#include <QMessageBox>

#include "tiff.h"
#include "resources.h"


#define ROW 384
#define COLUMN 288
#define VI_SIZE 768
#define BUF_SIZE 228096
#define SBUF_SIZE 110592
#define PICTURE_SIZE 221184

class videoPlayer : public QThread
{
    Q_OBJECT

    int numberCadr;
    int dircount;

    QTimer *timer;

    QMessageBox mb;
    QFile s;
    QImage *image;
    TIFF_Header tiffHeader;
    TIFF_Tag    IFD;
    uint nextIFDOffset;
    QList<uint> listCadr;

    int h, w, bpp;
    unsigned short *sBuffer;
    unsigned char *cPicture;
    unsigned char *picture;

    void run();
    ~videoPlayer();
public:
    explicit videoPlayer(QObject *parent = 0);

signals:
    void finished();
    void numberDir(int i);
    void framePosition(int i);
    void sendPicture(QImage *);
public slots:
    void showVideo();
    void startVideo(QString filename, int position);
    void stopVideo();
    void openVideo(QString filename);
};

#endif // VIDEOPLAYER_H
