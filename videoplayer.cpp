#include "videoplayer.h"

#include <QDebug>

videoPlayer::videoPlayer(QObject *parent) :
    QThread(parent)
{
    numberCadr = 0;
    dircount = 0;

    sBuffer = new unsigned short [SBUF_SIZE];
    memset(sBuffer,0,SBUF_SIZE);
    picture = new unsigned char [PICTURE_SIZE];
    memset(picture,0,PICTURE_SIZE);
    cPicture =  new unsigned char [SBUF_SIZE];
    memset(cPicture,0,SBUF_SIZE);

    timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()),this,SLOT(showVideo()));
    image = new QImage(cPicture,ROW,COLUMN,QImage::Format_Indexed8);
}

videoPlayer::~videoPlayer()
{
    s.close();
}

void videoPlayer::run()
{
    exec();
    s.close();
}

void videoPlayer::openVideo(QString filename)
{
    QMessageBox mb;
    s.setFileName(filename);
    s.open(QIODevice::ReadOnly);
    if(!s.isOpen())
    {
        mb.setText(QString().fromLocal8Bit("Ошибка открытия файла TIFF!"));
        mb.setStandardButtons(QMessageBox::Ok);
        mb.exec();
        return;
    }

    s.read((char*)&tiffHeader,sizeof(TIFF_Header));
    if((tiffHeader.encoding!=0x4949)||(tiffHeader.tiff_definer!=42))
    {
        mb.setText(QString().fromLocal8Bit("Некорректный заголовок файла TIFF!"));
        mb.setStandardButtons(QMessageBox::Ok);
        mb.exec();
        s.close();
        return;
    }
    nextIFDOffset = tiffHeader.offset_first_IFD;
    dircount=0;
    while(nextIFDOffset)
    {
        unsigned short IFD_count;
        listCadr.append(nextIFDOffset);
        if(!s.seek(nextIFDOffset))
        {
            mb.setText(QString().fromLocal8Bit("Файл TIFF битый!"));
            mb.setStandardButtons(QMessageBox::Ok);
            mb.exec();
            s.close();
            return;
        }
        if(s.read((char*)&IFD_count,sizeof(IFD_count))!=sizeof(IFD_count))
        {
            mb.setText(QString().fromLocal8Bit("Файл TIFF битый!"));
            mb.setStandardButtons(QMessageBox::Ok);
            mb.exec();
            s.close();
            return;
        }
        if(!s.seek(nextIFDOffset+sizeof(IFD_count)+sizeof(IFD)*IFD_count))
        {
            mb.setText(QString().fromLocal8Bit("Файл TIFF битый!"));
            mb.setStandardButtons(QMessageBox::Ok);
            mb.exec();
            s.close();
            return;
        }
        if(s.read((char*)&nextIFDOffset,sizeof(nextIFDOffset))!=sizeof(nextIFDOffset))
        {
            mb.setText(QString().fromLocal8Bit("Файл TIFF битый!"));
            mb.setStandardButtons(QMessageBox::Ok);
            mb.exec();
            s.close();
            return;
        }
        dircount++;
    }
    qDebug()<<dircount;
    emit numberDir(dircount);
}

void videoPlayer::startVideo(QString filename,int position)
{

    numberCadr=position;
    nextIFDOffset = listCadr[numberCadr];
    timer->start(100);
    showVideo();

}

void videoPlayer::showVideo()
{
    if(!nextIFDOffset)
    {
        timer->stop();
        return;
    }

    unsigned int img_offset, img_size, img_size_old;
    unsigned short IFD_count;

        if(!s.seek(nextIFDOffset))
        {
            mb.setText(QString().fromLocal8Bit("Файл TIFF битый!"));
            mb.setStandardButtons(QMessageBox::Ok);
            mb.exec();
            s.close();
            return;
        }
        if(s.read((char*)&IFD_count,sizeof(IFD_count))!=sizeof(IFD_count))
        {
            mb.setText(QString().fromLocal8Bit("Файл TIFF битый!"));
            mb.setStandardButtons(QMessageBox::Ok);
            mb.exec();
            s.close();
            return;
        }

        img_offset = 0, img_size = 0, img_size_old = 0;
        for(int i=0; i<IFD_count; i++)
        {
            if(s.read((char*)&IFD,sizeof(IFD))!=sizeof(IFD))
            {
                mb.setText(QString().fromLocal8Bit("Файл TIFF битый!"));
                mb.setStandardButtons(QMessageBox::Ok);
                mb.exec();
                s.close();
                return;
            }
            switch(IFD.TagName)
            {
            case TIFF_TAG_ImageWidth:
                w = IFD.TagValue;
                break;
            case TIFF_TAG_ImageLength:
                h = IFD.TagValue;
                break;
            case TIFF_TAG_BitsPerSample:
                bpp = IFD.TagValue/8;
                break;
            case TIFF_TAG_StripOffsets:
                img_offset = IFD.TagValue;
                break;
            case TIFF_TAG_StripByteCounts:
                img_size = IFD.TagValue;
                break;
            }
        }
        if((w!=384)||(h!=288)||(bpp!=2)||(img_offset==0)||(img_size==0)||
                ((img_size_old)&&(img_size!=img_size_old)))
        {
            mb.setText(QString().fromLocal8Bit("Некорректный файл TIFF!"));
            mb.setStandardButtons(QMessageBox::Ok);
            mb.exec();
            s.close();
            return;
        }
        img_size_old = img_size;

        if(s.read((char*)&nextIFDOffset,sizeof(nextIFDOffset))!=sizeof(nextIFDOffset))
        {
            mb.setText(QString().fromLocal8Bit("Файл TIFF битый!"));
            mb.setStandardButtons(QMessageBox::Ok);
            mb.exec();
            s.close();
            return;
        }

        if(!s.seek(img_offset))
        {
            mb.setText(QString().fromLocal8Bit("Файл TIFF битый!"));
            mb.setStandardButtons(QMessageBox::Ok);
            mb.exec();
            s.close();
            return;
        }


        if(s.read((char*)picture, PICTURE_SIZE)!=PICTURE_SIZE)
        {
            mb.setText(QString().fromLocal8Bit("Файл TIFF битый!"));
            mb.setStandardButtons(QMessageBox::Ok);
            mb.exec();
            s.close();
            return;
        }



    unsigned short maxx=0,minn=0xffff;

    memcpy(sBuffer,picture,PICTURE_SIZE);
    for (int i=0;i<SBUF_SIZE;i++)
    {
        sBuffer[i]=sBuffer[i]&0x3fff;
        if (sBuffer[i]>maxx)
            maxx=sBuffer[i];
        if (sBuffer[i]<minn)
            minn=sBuffer[i];
    }

    int mult, min2;
    unsigned short tst;


    mult = (maxx - minn)/255;
    min2 = (maxx - mult*255) + minn;

    for(int i=0;i<SBUF_SIZE;i++)
    {
        if(sBuffer[i]<min2)
            cPicture[i]=0;
        else
        {
            tst = (sBuffer[i]-min2)/mult;
            if(tst>255)
                qDebug("%u!",tst);
            cPicture[i]=tst;
        }
    }

    numberCadr++;
    qDebug()<<numberCadr;
    emit sendPicture(image);
    emit framePosition(numberCadr);
}

void videoPlayer::stopVideo()
{
    timer->stop();
    emit framePosition(numberCadr);
}
