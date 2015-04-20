#ifndef PICTURE_H
#define PICTURE_H

#include "pivolsthread.h"

#include <QThread>
#include <QImage>
#include <QMessageBox>
#include <QFile>
#include <QDataStream>

#define BEGIN_MARK 0xa116fd719d83
#define ROW 384
#define COLUMN 288

class Picture : public QThread
{
    Q_OBJECT
    QImage *image;
    QMessageBox mesBox;

    QFile videoFile;
    QDataStream out;
    bool record;

    bool stop;
    unsigned char *buffer;//пакет с ПИВОЛС
    unsigned char *picture;//массив кадра

struct package{
    unsigned long pack_mark; //маркер начала пакета
    unsigned char pack_type; //тип информации
    unsigned char pack_zip; //вид сжатия
    unsigned long pack_time; //время пакета
    unsigned int pack_number; //номр пакета
    unsigned int pack_row; //номер строки
}pack;

    ~Picture();
public:
    explicit Picture(QObject *parent = 0);
protected:
    void run();

signals:
    void sendPicture(QImage *);
    void finished();
    void stopPivols(bool);

public slots:
    void makePicture(unsigned char *buf);
    void stopped(bool s);
    void startRecord(const QString s);
    void stopRecord();
};

#endif // PICTURE_H
