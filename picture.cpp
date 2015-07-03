#include "picture.h"
#include <QDebug>


extern QMutex lock1, lock2;

Picture::Picture(QObject *parent) :
    QThread(parent)
{
    stop=false;
    record=false;
    option=0;

    sBuffer = new unsigned short [SBUF_SIZE];
    memset(sBuffer,0,SBUF_SIZE);
    picture = new unsigned char [PICTURE_SIZE];
    memset(picture,0,PICTURE_SIZE);
    cPicture =  new unsigned char [SBUF_SIZE];
    memset(cPicture,0,SBUF_SIZE);
    memset(&pack,0,sizeof(pack));

    image = new QImage(cPicture,ROW,COLUMN,QImage::Format_Indexed8);
}

Picture::~Picture()
{
    delete []picture;
    delete []sBuffer;
    TIFFClose(out);
    emit finished();
}


void Picture::run()
{
    exec();
    if(!lock1.tryLock())
        lock1.unlock();
    if(!lock2.tryLock())
        lock2.unlock();
}

void Picture::makePicture(unsigned char *buf, bool buf_flag)
{

    buffer=buf;

    //memcpy(&pack.pack_type,buffer+6,1);
    //memcpy(&pack.pack_zip,buffer+7,1);
    //memcpy(&pack.pack_time,buffer+8,7);
    //memcpy(&pack.pack_number,buffer+15,3);


    unsigned int count=0;
    while(count!=BUF_SIZE)
    {
        memcpy(&pack.pack_mark,&buffer[count],6);
        if(pack.pack_mark==BEGIN)
        {
            memcpy(&pack.pack_row,&buffer[count+18],2);
            pack.pack_row=encode(pack.pack_row);

            unsigned short row=pack.pack_row;
            memcpy(&picture[VI_SIZE*(row-1)],&buffer[count+23],VI_SIZE);


            if(pack.pack_row==COLUMN)
            {
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

                //for (int i=0;i<SBUF_SIZE;i++)
                        //cPicture[i]=sBuffer[i]<<1;

                if(record==true)
                {

                   TIFFSetField(out, TIFFTAG_IMAGEWIDTH, (uint16)ROW);
                   TIFFSetField(out, TIFFTAG_IMAGELENGTH, (uint16) COLUMN);
                   TIFFSetField(out, TIFFTAG_BITSPERSAMPLE, 16);
                   TIFFSetField(out, TIFFTAG_COMPRESSION, 1);
                   TIFFSetField(out, TIFFTAG_STRIPOFFSETS, 0);
                   TIFFSetField(out, TIFFTAG_PHOTOMETRIC, 1);
                   TIFFSetField(out, TIFFTAG_IMAGEDESCRIPTION,"12345,12345,ДД:ЧЧ:ММ:СС");
                   TIFFSetField(out, TIFFTAG_STRIPBYTECOUNTS,(uint32) PICTURE_SIZE);

                   for (int j = 0; j < COLUMN; j++)
                       TIFFWriteScanline(out, &picture[j * ROW*2], j, 0);

                   TIFFWriteDirectory(out);
                }

                emit sendPicture(image);
                emit sendFigure(cPicture);
            }
            count+=792;
        }
    }
    if(!buf_flag)
        lock2.unlock();
    else
        lock1.unlock();
}

void Picture::startRecord(const QString s)
{
    filename = s;

    out = TIFFOpen(filename.toLocal8Bit().constData(),"w") ;

    if (!out)
    {
           fprintf (stderr, "Can't open %s for writing\n", filename);
           record=false;
    }
    else
        record=true;
}

void Picture::stopRecord()
{

    record=false;
    TIFFClose(out);
}

void Picture::stopped()
{
    this->exit(0);
}




