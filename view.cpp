#include "view.h"

#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QMessageBox>
#include <QFileDialog>
#include <QGridLayout>
#include <QThread>
#include <QTime>

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

    cadr=0;
    stream=false;


    QHBoxLayout *hlayout= new QHBoxLayout;
    QVBoxLayout *vlayout = new QVBoxLayout;
    QHBoxLayout *cadrlayout = new QHBoxLayout;

    lbl = new QLabel(this);
    lbl->setMaximumSize(384,288);
    fileButton = new QPushButton("Open Source");
    fileButton->setMaximumSize(100,30);
    imgOpen = new QPushButton("Open Image");
    imgOpen->setMaximumSize(100,30);
    imgSave = new QPushButton("Save Image");
    imgSave->setMaximumSize(100,30);

    lblcadr = new QLabel;
    lblcadr->setNum(cadr);

    nextBut = new QPushButton;
    nextBut->setMaximumSize(30,30);
    nextBut->setIcon(QIcon("next.jpg"));

    beforBut = new QPushButton;
    beforBut->setMaximumSize(30,30);
    beforBut->setIcon(QIcon("befor.jpg"));
    
    playBut= new QPushButton("Play");
    stopBut= new QPushButton("Stop");
    
    cadrlayout->addWidget(lblcadr);
    cadrlayout->addWidget(beforBut);
    cadrlayout->addWidget(nextBut);
    

    rbw = new QRadioButton("Black and White",this);
    rbw->setChecked(true);
    rpse = new QRadioButton("PseudoColor", this);

    vlayout->addWidget(fileButton);
    vlayout->addWidget(rbw);
    vlayout->addWidget(rpse);
    vlayout->addLayout(cadrlayout);
    
    //vlayout->addStretch();
    //vlayout->addWidget(playBut);
    //vlayout->addWidget(stopBut);
    
    vlayout->addStretch();
    vlayout->addWidget(imgOpen);
    vlayout->addWidget(imgSave);

    hlayout->addWidget(lbl);
    hlayout->addStretch();
    hlayout->addLayout(vlayout);

    this->setLayout(hlayout);

    connect(fileButton,SIGNAL(clicked()),this,SLOT(open()));
    connect(imgSave,SIGNAL(clicked()),this,SLOT(saveImage()));
    connect(imgOpen,SIGNAL(clicked()),this,SLOT(openImage()));
    connect(rbw,SIGNAL(clicked()),this,SLOT(setColor()));
    connect(rpse,SIGNAL(clicked()),this,SLOT(setColor()));
    connect(nextBut,SIGNAL(clicked()),this,SLOT(nextCadr()));
    connect(beforBut,SIGNAL(clicked()),this,SLOT(beforCadr()));
    connect(playBut,SIGNAL(clicked()),this,SLOT(play()));
    connect(stopBut,SIGNAL(clicked()),this,SLOT(stop()));

    visible(false);
}

view::~view()
{
    if(tmpImg0)
    {
        free(tmpImg0);
        free(tmpImg1);
    }
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
    mb.setInformativeText(QString().fromLocal8Bit("Открыть другой файл?"));
    mb.setStandardButtons(QMessageBox::Ok | QMessageBox::Cancel);
    mb.setDefaultButton(QMessageBox::Cancel);

    while(1)
    {
        anotherOne = false;
        FileName = QFileDialog::getOpenFileName(this,QString().fromLocal8Bit("Открыть FVRawFil"),QString().fromLocal8Bit("C:\\"),
                                                QString().fromLocal8Bit("FVRaw video (*.fvlab)"));
        if(FileName.isEmpty())
        {
            free(fvlabMagicIdTest);
            free(headerMagicIdTest);
            free(indexMagicIdTest);
            return;
        }

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

        //пїЅпїЅпїЅпїЅпїЅпїЅ Sizes[2] (пїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅ):
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
        fileStream >> rsize;
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
        if(FVRFile.read((char*)&cameraStateHeader,sizeof(FVCameraStateInfo))!=sizeof(FVCameraStateInfo))
        {
            mb.setText(QString().fromLocal8Bit("Ошибка, не удалось прочитать\nзаголовок файла!"));
            if(mb.exec()==QMessageBox::Ok)
                continue;
            else
                break;
        }
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
        }

        if(sizes[4])
        {
            fileStream >> rsize;
            if(rsize!=sizes[4])
            {
                mb.setText(QString().fromLocal8Bit("Ошибка, некорректный размер\nсекции комментариев!"));
                if(mb.exec()==QMessageBox::Ok)
                    continue;
                else
                    break;
            }
            fileStream >> commentBuffer;
        }

        if(sizes[5])
        {
            fileStream >> rsize;
            if(rsize!=sizes[5])
            {
                mb.setText(QString().fromLocal8Bit("Ошибка, некорректный размер\nсекции названия камеры!"));
                if(mb.exec()==QMessageBox::Ok)
                    continue;
                else
                    break;
            }
            fileStream >> cameraName;
        }

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
                idxs.push_back(new fInfo());
                idxs.back()->frames = (frameInfo*)calloc(fCnt,fInfoSize);
                idxs.back()->size = fCnt;
                if(fileStream.readRawData((char*)(idxs.back()->frames),bsize) != bsize )
                {
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
        break;
    }

      source(fmap, totalFrames, framesPos, &cameraStateHeader);
      visible(true);
}

void view::setColor()
{
    source(fmap, totalFrames, framesPos, &cameraStateHeader);
}



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
    if(direction&2)
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

    tmpImg0 = (uchar*)calloc(tmp,4);
    tmpImg1 = (uchar*)calloc(tmp,4);

    QVector<QRgb> colorTable(256);
    FVRFile.seek(framesPos[cadr]);
    FVRFile.read((char*)tmpImg0,h*w);
    image = new QImage(tmpImg0,w,h,QImage::Format_Indexed8);


    if (rbw->isChecked()==true)
    {
        for(int i=0;i<256;i++)
        {
            colorTable[i] =  0xFF000000|((uchar)(i)<<16)|((uchar)(i)<<8)|(uchar)(i);
        }
    }
    if (rpse->isChecked()==true)
    {

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

    }
    image->setColorTable(colorTable);
    lbl->setPixmap(QPixmap::fromImage(*image).scaled(QSize(384,288)));

    delete image;
}

void view::saveImage()
{
    QString imageName = QFileDialog::getSaveFileName(this,QString().fromLocal8Bit("Выберите файл для сохранения"),QString().fromLocal8Bit("C:\\"),
                                                          QString().fromLocal8Bit("Изображение (*.jpg"));
    if(imageName!="")
            lbl->pixmap()->save(imageName,"JPG");

}

void view::openImage()
{
    QString imageName = QFileDialog::getOpenFileName(this,QString().fromLocal8Bit("Выберите файл"),QString().fromLocal8Bit("C:\\"),
                                                          QString().fromLocal8Bit("Изображение (*.jpg)"));
     if(imageName!="")
     {
         QPixmap *pix=new QPixmap;
         pix->load(imageName);
            lbl->setPixmap(*pix);
     }
     visible(false);
}

void view::nextCadr()
{
    if(cadr>=0&&cadr<totalFrames)
        cadr++;
    source(fmap, totalFrames, framesPos, &cameraStateHeader);
    lblcadr->setNum(cadr);
}

void view::beforCadr()
{
    if(cadr>0&&cadr<=totalFrames)
        cadr--;
    source(fmap, totalFrames, framesPos, &cameraStateHeader);
    lblcadr->setNum(cadr);
}

void view::visible(bool Visible)
{
    if(Visible==0)
    {
        imgSave->setEnabled(false);
        nextBut->setEnabled(false);
        beforBut->setEnabled(false);
        rbw->setEnabled(false);
        rpse->setEnabled(false);
    }
    else
    {
        imgSave->setEnabled(true);
        nextBut->setEnabled(true);
        beforBut->setEnabled(true);
        rbw->setEnabled(true);
        rpse->setEnabled(true);
    }
}

