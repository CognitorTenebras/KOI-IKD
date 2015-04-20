#include "picture.h"



extern QReadWriteLock lock;

Picture::Picture(QObject *parent) :
    QThread(parent)
{
    stop=false;
    record=false;

    picture = new unsigned char [ROW*COLUMN];
    memset(&picture,0,ROW*COLUMN);

    memset(&pack,0,sizeof(pack));

    mesBox.setWindowTitle("КОИ-ИКД");
}

Picture::~Picture()
{
    delete []picture;
}

void Picture::run()
{
    pivolsthread *pivols = new pivolsthread(this);
    connect(pivols,SIGNAL(sendResult(unsigned char*)),
            this,SLOT(makePicture(unsigned char*)));
    connect(pivols,SIGNAL(finished()),pivols,SLOT(deleteLater()));
    pivols->start();
    forever{
        if(stop)
        {
            emit stopPivols(true);
            break;
        }

    }
}

void Picture::makePicture(unsigned char *buf)
{
    lock.lockForRead();
    buffer=buf;

    memcpy(&pack.pack_mark,buffer,6);
    if(pack.pack_mark!=BEGIN_MARK)
    {
        mesBox.setText(QString("Несовпадение маркера пакета: %1").arg(pack.pack_mark,0,16));
        mesBox.exec();
        return;
    }

    memcpy(&pack.pack_type,buffer+6,1);
    if(pack.pack_type!=0x1||pack.pack_type!=0x10)
    {
        mesBox.setText(QString("Неправильный тип иформации: %1").arg(pack.pack_type,0,16));
        mesBox.exec();
        return;
    }

    memcpy(&pack.pack_zip,buffer+7,1);
    memcpy(&pack.pack_time,buffer+8,7);
    memcpy(&pack.pack_number,buffer+15,3);
    memcpy(&pack.pack_row,buffer+18,2);

    if(pack.pack_row==1)
        memset(&picture,0,ROW*COLUMN);
    memcpy(picture+ROW*(pack.pack_row-1),buffer+24,ROW);
    lock.unlock();
    if(pack.pack_row==288)
    {
        if(record==true)
            if(out.writeRawData((const char*)picture,COLUMN*ROW)!=COLUMN*ROW)
            {
                mesBox.setText(QString("Ошибка записи в файл"));
                mesBox.exec();
            }
        image= new QImage(picture,COLUMN,ROW,QImage::Format_Indexed8);
        emit sendPicture(image);
        delete image;
    }

    memset(&pack,0, sizeof(pack));
}

void Picture::startRecord(const QString s)
{
    videoFile.setFileName(s);
    videoFile.open(QIODevice::WriteOnly);
    out.setDevice(&videoFile);

}

void Picture::stopRecord()
{
    record=false;
    out.unsetDevice();
    if(!videoFile.remove())
    {
        mesBox.setText("Файл не был закрыт");
        mesBox.exec();
    }
}

void Picture::stopped(bool s)
{
    stop=s;
}
