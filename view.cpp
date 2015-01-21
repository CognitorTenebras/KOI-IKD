#include "view.h"

#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QMessageBox>
#include <QFileDialog>
#include <QGridLayout>

const char * view::fvlabMagicId  = "FVLAB";
const char * view::headerMagicId = "FVLABHDR";
const char * view::fpnMagicId    = "FVLABFPN";
const char * view::indexMagicId  = "FVLABIDX";

view::view(QWidget *parent) :
    QWidget(parent)
{
    totalFrames = 0;
    framesPos = 0;
    sectors = 0;


    tmpImg0 = 0; tmpImg1 = 0;

    QHBoxLayout *hlayout= new QHBoxLayout;
    QVBoxLayout *vlayout = new QVBoxLayout;
    //QGridLayout *grid = new QGridLayout;

    lbl = new QLabel(this);
    //lbl->setGeometry(30,30,200,200);
    //grid->addWidget(lbl);
    fileButton = new QPushButton("Open file");
    fileButton->setMaximumSize(100,30);

    rbw = new QRadioButton("Black and White",this);
    rbw->setChecked(true);
    rpse = new QRadioButton("PseudoColor", this);


    vlayout->addWidget(fileButton);
    vlayout->addWidget(rbw);
    vlayout->addWidget(rpse);

    hlayout->addWidget(lbl);
    hlayout->addStretch();
    hlayout->addLayout(vlayout);

    this->setLayout(hlayout);

    connect(fileButton,SIGNAL(clicked()),this,SLOT(open()));
    connect(rbw,SIGNAL(clicked()),this,SLOT(setColor()));
    connect(rpse,SIGNAL(clicked()),this,SLOT(setColor()));
}

void view::open()
{
    char *fvlabMagicIdTest, *headerMagicIdTest, *indexMagicIdTest/*, *image*/;
    int sizes[maxSizes];
    fvlabMagicIdTest = (char*)malloc(strlen(fvlabMagicId)+1);
    headerMagicIdTest = (char*)malloc(strlen(headerMagicId)+1);
    indexMagicIdTest = (char*)malloc(strlen(indexMagicId)+1);

    QString FileName;
    bool anotherOne;
    QMessageBox mb;
    //mb.setText(QString().fromLocal8Bit("Ошибка открытия файла!"));
    mb.setInformativeText(QString().fromLocal8Bit("Открыть другой файл?"));
    mb.setStandardButtons(QMessageBox::Ok | QMessageBox::Cancel);
    mb.setDefaultButton(QMessageBox::Cancel);

    while(1)
    {
        anotherOne = false;
        FileName = QFileDialog::getOpenFileName(this,QString().fromLocal8Bit("Открыть FVRawFile"),QString().fromLocal8Bit("C:\\"),
                                                QString().fromLocal8Bit("FVRaw video (*.fvlab)"));
        if(FileName.isEmpty())
        {
            free(fvlabMagicIdTest);
            free(headerMagicIdTest);
            free(indexMagicIdTest);
            return;
        }

        //QFile FVRFile(FileName);
        if(FVRFile.isOpen())
        {
            FVRFile.unmap(fmap);
            FVRFile.close();
        }
        FVRFile.setFileName(FileName);
        if(FVRFile.open(QIODevice::ReadOnly)==false)
            mb.setText(QString().fromLocal8Bit("Ошибка, не удалось открыть\nфайл на чтение!"));


        totalFrames = 0;

        if(FVRFile.read(fvlabMagicIdTest,strlen(fvlabMagicId))!=strlen(fvlabMagicId))
        {
            mb.setText(QString().fromLocal8Bit("Ошибка, не удалось открыть\nфайл на чтение!"));
            if(mb.exec()==QMessageBox::Ok)
                continue;
            else
                break;
        }
        fvlabMagicIdTest[strlen(fvlabMagicId)] = '\0';
        if( ! strcmp(fvlabMagicIdTest,fvlabMagicId) == 0 )
        {
            mb.setText(QString().fromLocal8Bit("Ошибка, некорректный формат файла!"));
            if(mb.exec()==QMessageBox::Ok)
                continue;
            else
                break;
        }

        QDataStream fileStream(&FVRFile);
        qint32 fver = 0;
        fileStream >> fver;
        if( fver != ver )
        {
            mb.setText(QString().fromLocal8Bit("Ошибка, неизвестная версия\nформата файла (0x%1)!").arg(fver,0,16));
            if(mb.exec()==QMessageBox::Ok)
                continue;
            else
                break;
        }

        int nsize = 0;
        fileStream >> nsize;
        if( nsize != maxSizes )
        {
            mb.setText(QString().fromLocal8Bit("Ошибка внутренней структуры файла!"));
            if(mb.exec()==QMessageBox::Ok)
                continue;
            else
                break;
        }

        sizes[0] = nsize;
        for(int i = 1; i < nsize; i++ )
            fileStream >> sizes[i];
        int sum = 0;
        for(int i = 2; i < nsize; i++ )
            sum += sizes[i];
        if( sum != sizes[1] )
        {
            mb.setText(QString().fromLocal8Bit("Ошибка внутренней структуры файла!"));
            if(mb.exec()==QMessageBox::Ok)
                continue;
            else
                break;
        }

        //Читаем Sizes[2] (заголовок):
        if(sizes[2]==0)
        {
            mb.setText(QString().fromLocal8Bit("Ошибка, отсутствует заголовок файла!"));
            if(mb.exec()==QMessageBox::Ok)
                continue;
            else
                break;
        }
        if(sizes[2]!=(strlen(headerMagicId) + sizeof(FVCameraStateInfo)))
        {
            mb.setText(QString().fromLocal8Bit("Ошибка, некорректный размер заголовка файла!"));
            if(mb.exec()==QMessageBox::Ok)
                continue;
            else
                break;
        }

        qint32 rsize = 0;
        fileStream >> rsize;
        if( sizes[2] != rsize )
        {
            mb.setText(QString().fromLocal8Bit("Ошибка, некорректный размер заголовка файла!"));
            if(mb.exec()==QMessageBox::Ok)
                continue;
            else
                break;
        }
        fileStream >> rsize;//Так надо (данные записаны в QByteArray, содержащий в начале размер (32 разряда))
        if(FVRFile.read(headerMagicIdTest,strlen(headerMagicId))!=strlen(headerMagicId))
        {
            mb.setText(QString().fromLocal8Bit("Ошибка, не удалось прочитать\nидентификатор заголовка файла!"));
            if(mb.exec()==QMessageBox::Ok)
                continue;
            else
                break;
        }
        headerMagicIdTest[strlen(headerMagicId)] = '\0';
        if( ! strcmp(headerMagicIdTest,headerMagicId) == 0 )
        {
            mb.setText(QString().fromLocal8Bit("Ошибка, некорректный идентификатор заголовка файла!"));
            free(headerMagicIdTest);
            if(mb.exec()==QMessageBox::Ok)
                continue;
            else
                break;
        }
        free(headerMagicIdTest);
        //читаем параметры камеры
        if(FVRFile.read((char*)&cameraStateHeader,sizeof(FVCameraStateInfo))!=sizeof(FVCameraStateInfo))
        {
            mb.setText(QString().fromLocal8Bit("Ошибка, не удалось прочитать\nзаголовок файла!"));
            if(mb.exec()==QMessageBox::Ok)
                continue;
            else
                break;
        }
        /*qDebug("fps: %f, width: %d, height: %d, bpp: %d, compression level: %d, FilledSize: %d, FrameSize: %d",
               cameraStateHeader.GetFPS(),cameraStateHeader.GetAcqWidth(),cameraStateHeader.GetAcqHeight(),cameraStateHeader.GetBPP(),
               cameraStateHeader.GetCompressionLevel(),cameraStateHeader.GetFilledSize(),cameraStateHeader.GetFrameSize());*/
        /*cameraStateHeader.QPrintAll();
        QDateTime dt;
        dt.setMSecsSinceEpoch(cameraStateHeader.GetTimeMsec());
        qDebug() << dt.toString();*/

        if(sizes[3])
        {
            fileStream >> rsize;
            if(rsize!=sizes[3])
            {
                mb.setText(QString().fromLocal8Bit("Ошибка, некорректный размер\nсекции параметров съёмки!"));
                if(mb.exec()==QMessageBox::Ok)
                    continue;
                else
                    break;
            }
            fileStream >> fpnBuffer;
            /*qDebug("fpn size: %d",sizes[3]);//1310776 - чуть больше метра
            QFile f_out("fpn.bin");
            f_out.open(QIODevice::WriteOnly);
            f_out.write(fpnBuffer);
            f_out.close();*/
        } //else qDebug("fpn info is absent");

        if(sizes[4])
        {
            fileStream >> rsize;
            if(rsize!=sizes[4])
            {
                mb.setText(QString().fromLocal8Bit("Ошибка, некорректный размер\nсекции комментариев!"));//битый файл
                if(mb.exec()==QMessageBox::Ok)
                    continue;
                else
                    break;
            }
            fileStream >> commentBuffer;
        } //else qDebug("comments are absent");

        if(sizes[5])
        {
            fileStream >> rsize;
            if(rsize!=sizes[5])
            {
                mb.setText(QString().fromLocal8Bit("Ошибка, некорректный размер\nсекции названия камеры!"));//битый файл
                if(mb.exec()==QMessageBox::Ok)
                    continue;
                else
                    break;
            }
            fileStream >> cameraName;
            //qDebug("camera name: %s",cameraName.data());
        } //else qDebug("camera name is absent");

        fileStream >> indexStartPos;
        fileStream >> frameStartPos;

        //
        if(FVRFile.read(indexMagicIdTest,strlen(indexMagicId))!=strlen(indexMagicId))
        {
            mb.setText(QString().fromLocal8Bit("Ошибка, не удалось прочитать\nидентификатор секции индексов!"));
            if(mb.exec()==QMessageBox::Ok)
                continue;
            else
                break;
        }
        indexMagicIdTest[strlen(indexMagicId)] = '\0';
        if( ! strcmp(indexMagicIdTest,indexMagicId) == 0 )
        {
            mb.setText(QString().fromLocal8Bit("Ошибка, некорректный идентификатор секции индексов!"));
            if(mb.exec()==QMessageBox::Ok)
                continue;
            else
                break;
        }

        int fInfoSize = 0;
        fileStream >> totalFrames >> fInfoSize;

        totalFrames = 0;
        for(int i=idxs.size()-1;i>=0;i--)
        {
            free(idxs.at(i)->frames);
        }
        idxs.clear();
        if(framesPos)
            free(framesPos);

        if(FVRFile.seek(indexStartPos))
        {
            //цикл чтения индексов, если после последнего индексированного элемента еще остается место в файле,
            //перемещаемся туда и продолжаем чтение индексов (и так до конца)
            while(FVRFile.pos()<FVRFile.size())
            {
                if(FVRFile.read(indexMagicIdTest,strlen(indexMagicId))!=strlen(indexMagicId))
                    break;

                indexMagicIdTest[strlen(indexMagicId)] = '\0';
                if( ! strcmp(indexMagicIdTest,indexMagicId) == 0 )
                    break;

                QDataStream fileStream(&FVRFile);

                int fCnt = 0, fInfoSize = 0;
                fileStream >> fCnt >> fInfoSize;
                if(fInfoSize!=sizeof(frameInfo))
                    break;
                //
                if( fCnt <= 0 )
                    break;
                //
                int bsize = fInfoSize*fCnt;
                //size_t size0 = idxs.size();
                //
                //indexBuffer->resize(bsize+size0);
                idxs.push_back(new fInfo());
                idxs.back()->frames = (frameInfo*)calloc(fCnt,fInfoSize);
                idxs.back()->size = fCnt;
                //indexBufferPtr = &indexBuffer[size0];
                //
                //if( fileStream.readRawData(&indexBuffer[0][size0],bsize) != bsize )
                if(fileStream.readRawData((char*)(idxs.back()->frames),bsize) != bsize )
                {
                    //qDebug("Can\'t read index!");
                    //не смогли прочитать даже индекс => изображений точно не будет
                    //indexBuffer->resize(size0);
                    free(idxs.back()->frames);
                    idxs.pop_back();
                    break;
                }
                //
                frameInfo *indexFrames = (frameInfo *)idxs.back()->frames;
                if( totalFrames == 0 )
                {
                    if( indexFrames[0].pos != frameStartPos )
                    {
                        //qDebug("File is broken!");
                        mb.setText(QString().fromLocal8Bit("Ошибка формата файла!"));
                        if(mb.exec()==QMessageBox::Ok)
                            anotherOne = true;
                        break;
                    }
                }
                //
                totalFrames += fCnt;
                qint64 next = indexFrames[totalFrames-1].pos + indexFrames[totalFrames-1].size;
                //
                if( next < FVRFile.size() )
                {
                    int failedFrames = 1;
                    if( ! FVRFile.seek( next ) )
                    {
                        while(!FVRFile.seek( indexFrames[totalFrames-1-failedFrames].pos + indexFrames[totalFrames-1-failedFrames].size ))
                        {
                            failedFrames++;
                        }
                        //indexBuffer->resize(bsize+size0-(failedFrames*fInfoSize));
                        idxs.back()->size -= failedFrames;
                        totalFrames -= failedFrames;
                        break;
                    }
                }
                else
                    break;
            }

            if(anotherOne)
                continue;

            if(totalFrames==0)
            {
                mb.setText(QString().fromLocal8Bit("Ошибка, файл не содержит кадров!"));
                if(mb.exec()==QMessageBox::Ok)
                    continue;
                else
                    break;
            }

            framesPos = new qint64[totalFrames];
            int tik=0,tak=0,last_tak;
            for(int i=0;i<totalFrames;i++)
            {
                if(tak==0)
                    last_tak = idxs.at(tik)->size - 1;
                framesPos[i] = idxs.at(tik)->frames[tak].pos;
                if(tak==(last_tak))
                {
                    tak = 0;
                    tik++;
                } else
                    tak++;
            }

            for(int i=idxs.size()-1;i>=0;i--)
            {
                free(idxs.at(i)->frames);
            }
            idxs.clear();

            qDebug("%d",totalFrames);
        }

        free(fvlabMagicIdTest);
        free(indexMagicIdTest);
        //FVRFile.close();
        fmap = FVRFile.map(0,FVRFile.size());
        qDebug("%d",fmap);
        break;
    }

      source(fmap, totalFrames, framesPos, &cameraStateHeader);
}

void view::setColor()
{
    source(fmap, totalFrames, framesPos, &cameraStateHeader);
}

/*
void view::rbwClicked()
{
    color=false;
}

void view::rpseClicked()
{
    color=true;
}
*/

void view::source(uchar *map, int totalFrames, qint64 *framesPos, FVCameraStateInfo *fileInfo)
{
    processedImgs = 0;
    fromUPdoDOWN = 0;
    preResult.clear();
    int bpp,tmp;
    this->map = map;
    this->totalFrames = totalFrames;

    this->framesPos = framesPos;
    this->fileInfo = fileInfo;
    if(direction&2)//если движемся по горизонтали (поворачиваемся на 90 градусов)
    {
        w = fileInfo->GetAcqHeight();
        h = fileInfo->GetAcqWidth();
    } else
    {
        h = fileInfo->GetAcqHeight();
        w = fileInfo->GetAcqWidth();
    }
    bpp = fileInfo->GetBPP();
    tmp = h*w*bpp/4;
    /*
    if(tmpImg0)
    {
        free(tmpImg0);
        free(tmpImg1);
    }
    */
    tmpImg0 = (uchar*)calloc(tmp,4);
    tmpImg1 = (uchar*)calloc(tmp,4);


    if (rbw->isChecked()==true)
    {
        QVector<QRgb> colorTable(256);//550nm: 380+160
        colorTable[0]=0xFF000000;
        FVRFile.seek(framesPos[0]);
        FVRFile.read((char*)tmpImg0,h*w);
        image = new QImage(tmpImg0,w,h,QImage::Format_Indexed8);
        for(int i=1;i<256;i++)
        {
            colorTable[i] =  0xFF000000|((uchar)(i)<<16)|((uchar)(i)<<8)|(uchar)(i);

        }
        image->setColorTable(colorTable);
    }
    if (rpse->isChecked()==true)
    {
        QVector<QRgb> colorTable(256);//550nm: 380+160
        FVRFile.seek(framesPos[0]);
        FVRFile.read((char*)tmpImg0,h*w);
        image = new QImage(tmpImg0,w,h,QImage::Format_Indexed8);
        for(int i=0;i<42;i++)
            colorTable[i] =  0xFF000000|((uchar)(0)<<16)|((uchar)(0)<<8)|(uchar)(200+i);
        for(int i=0;i<42;i++)
            colorTable[42+i] =  0xFF000000|((uchar)(0)<<16)|((uchar)(200+i)<<8)|(uchar)(0xFF);
        for(int i=0;i<56;i++)
            colorTable[84+i] =  0xFF000000|((uchar)(0)<<16)|((uchar)(190+i)<<8)|(uchar)(0);
        for(int i=0;i<42;i++)
            colorTable[130+i] =  0xFF000000|((uchar)(200+i)<<16)|((uchar)(0xFF)<<8)|(uchar)(0);
        for(int i=0;i<42;i++)
            colorTable[172+i] =  0xFF000000|((uchar)(0xFF)<<16)|((uchar)(127+i)<<8)|(uchar)(0);
        for(int i=0;i<42;i++)
            colorTable[214+i] =  0xFF000000|((uchar)(200+i)<<16)|((uchar)(0)<<8)|(uchar)(0);

        image->setColorTable(colorTable);
    }



        lbl->setPixmap(QPixmap::fromImage(*image));

        delete image;


}
