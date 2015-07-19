#ifndef PICTURE_H
#define PICTURE_H

#include "pivolsthread.h"
#include "tiffio.h"
#include "tiff.h"


#include <QThread>
#include <QImage>
#include <QMessageBox>
#include <QFile>
#include <QDataStream>

#define BEGIN 0x839d71fd16a1
#define ROW 384
#define COLUMN 288
#define VI_SIZE 768
#define BUF_SIZE 228096
#define SBUF_SIZE 110592
#define PICTURE_SIZE 221184

class Picture : public QThread
{
    Q_OBJECT

    QMessageBox mesBox;
    QImage *image;
    QString filename;
    TIFF *out;


    int option;
    bool record;
    bool stop;

    unsigned char *buffer;//пакет с ПИВОЛС
    unsigned char *picture;//массив кадра
    unsigned short *sBuffer;
    unsigned char *cPicture;

struct package{
    unsigned long long pack_mark; //маркер начала пакета
    unsigned char pack_type; //тип информации
    unsigned char pack_zip; //вид сжатия
    unsigned long pack_time; //время пакета
    unsigned int pack_number; //номр пакета
    unsigned short pack_row; //номер строки
}pack;

    ~Picture();

public:
    explicit Picture(QObject *parent = 0);
protected:
    void run();

signals:
    void sendPicture(QImage *);
    void sendFigure(unsigned char *buf);
    void finished();

public slots:
    void makePicture(unsigned char *buf, bool buf_flag);
    void stopped();
    void startRecord(const QString s);
    void stopRecord();
};

#endif // PICTURE_H
