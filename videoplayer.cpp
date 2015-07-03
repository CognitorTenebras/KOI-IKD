#include "videoplayer.h"

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
    connect(timer, SLOT(timeout()),SLOT(showVideo()));
    image = new QImage(cPicture,ROW,COLUMN,QImage::Format_Indexed8);
}

videoPlayer::~videoPlayer()
{
    TIFFClose(out);
}

void videoPlayer::run()
{
    exec();
}

void videoPlayer::openVideo(QString s)
{
    out = TIFFOpen(s.toLocal8Bit().constData(),"w") ;

    if (!out)
    {
           fprintf (stderr, "Can't open %s for writing\n", s);
           emit numberDir(0);
    }
    else
    {

        do {
            dircount++;
        } while (TIFFReadDirectory(out));
        emit numberDir(dircount);
        TIFFClose(out);
    }

}

void videoPlayer::startVideo(QString s,int position)
{
    out = TIFFOpen(s.toLocal8Bit().constData(),"w") ;
    if (!out)
    {
           fprintf (stderr, "Can't open %s for writing\n", s);
    }
    else
    {
        numberCadr=position;
        int i=0;
        while (i!=numberCadr)
            TIFFReadDirectory(out);
        timer->start(100);
        showVideo();
    }
}

void videoPlayer::showVideo()
{
    if(numberCadr==dircount-1)
        timer->stop();
    uint32 imagelength;
    uint32 row;

    TIFFGetField(out, TIFFTAG_IMAGELENGTH, &imagelength);
    for (row = 0; row < imagelength; row++)
        TIFFReadScanline(out, &picture[row*ROW*2], row);

    unsigned short maxx=0,minn=0xffff;

    memcpy(sBuffer,picture,PICTURE_SIZE);
    for (int i=0;i<SBUF_SIZE;i++)
    {
        sBuffer[i]=encode(sBuffer[i])&0x3fff;
        if (sBuffer[i]>maxx)
            maxx=sBuffer[i];
        if (sBuffer[i]<minn)
            minn=sBuffer[i];

    }

    int mult, min2;

    mult = (maxx - minn)/255;
    min2 = (maxx - mult*255) + minn;

    for(int i=0;i<SBUF_SIZE;i++)
    {
        if(sBuffer[i]<min2)
            cPicture[i]=0;
        else
            cPicture[i]=(sBuffer[i]-min2)/mult;
    }
    numberCadr++;
    emit sendPicture(image);
    emit framePosition(numberCadr);
}

void videoPlayer::stopVideo()
{
    timer->stop();
    emit framePosition(numberCadr);
}
