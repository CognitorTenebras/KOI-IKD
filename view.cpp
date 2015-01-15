#include "view.h"

#include <QHBoxLayout>
#include <QMessageBox>
#include <QFileDialog>

const char * view::fvlabMagicId  = "FVLAB";
const char * view::headerMagicId = "FVLABHDR";
const char * view::fpnMagicId    = "FVLABFPN";
const char * view::indexMagicId  = "FVLABIDX";

view::view(QWidget *parent) :
    QWidget(parent)
{
    QHBoxLayout *hlayout= new QHBoxLayout;

    lbl = new QLabel(this);
    lbl->setMinimumSize(100,100);

    fileButton = new QPushButton("Open file");

    hlayout->addWidget(lbl);
    hlayout->addWidget(fileButton);

    this->setLayout(hlayout);

    connect(fileButton,SIGNAL(clicked()),this,SLOT(open()));
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
    mb.setInformativeText(QString().fromLocal8Bit("������� ������ ����?"));
    mb.setStandardButtons(QMessageBox::Ok | QMessageBox::Cancel);
    mb.setDefaultButton(QMessageBox::Cancel);

    while(1)
    {
        anotherOne = false;
        FileName = QFileDialog::getOpenFileName(this,QString().fromLocal8Bit("������� FVRawFile"),QString().fromLocal8Bit("C:\\"),
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
        FVRFile.open(QIODevice::ReadOnly);
        totalFrames = 0;

        if(FVRFile.read(fvlabMagicIdTest,strlen(fvlabMagicId))!=strlen(fvlabMagicId))
        {
            mb.setText(QString().fromLocal8Bit("������, �� ������� �������\n���� �� ������!"));
            if(mb.exec()==QMessageBox::Ok)
                continue;
            else
                break;
        }
        fvlabMagicIdTest[strlen(fvlabMagicId)] = '\0';
        if( ! strcmp(fvlabMagicIdTest,fvlabMagicId) == 0 )
        {
            mb.setText(QString().fromLocal8Bit("������, ������������ ������ �����!"));
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
            mb.setText(QString().fromLocal8Bit("������, ����������� ������\n������� ����� (0x%1)!").arg(fver,0,16));
            if(mb.exec()==QMessageBox::Ok)
                continue;
            else
                break;
        }

        int nsize = 0;
        fileStream >> nsize;
        if( nsize != maxSizes )
        {
            mb.setText(QString().fromLocal8Bit("������ ���������� ��������� �����!"));
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
            mb.setText(QString().fromLocal8Bit("������ ���������� ��������� �����!"));
            if(mb.exec()==QMessageBox::Ok)
                continue;
            else
                break;
        }

        //������ Sizes[2] (���������):
        if(sizes[2]==0)
        {
            mb.setText(QString().fromLocal8Bit("������, ����������� ��������� �����!"));
            if(mb.exec()==QMessageBox::Ok)
                continue;
            else
                break;
        }


        qint32 rsize = 0;
        fileStream >> rsize;
        if( sizes[2] != rsize )
        {
            mb.setText(QString().fromLocal8Bit("������, ������������ ������ ��������� �����!"));
            if(mb.exec()==QMessageBox::Ok)
                continue;
            else
                break;
        }
        fileStream >> rsize;
        if(FVRFile.read(headerMagicIdTest,strlen(headerMagicId))!=strlen(headerMagicId))
        {
            mb.setText(QString().fromLocal8Bit("������, �� ������� ���������\n������������� ��������� �����!"));
            if(mb.exec()==QMessageBox::Ok)
                continue;
            else
                break;
        }
        headerMagicIdTest[strlen(headerMagicId)] = '\0';
        if( ! strcmp(headerMagicIdTest,headerMagicId) == 0 )
        {
            mb.setText(QString().fromLocal8Bit("������, ������������ ������������� ��������� �����!"));
            free(headerMagicIdTest);
            if(mb.exec()==QMessageBox::Ok)
                continue;
            else
                break;
        }
        free(headerMagicIdTest);




        if(sizes[3])
        {
            fileStream >> rsize;
            if(rsize!=sizes[3])
            {
                mb.setText(QString().fromLocal8Bit("������, ������������ ������\n������ ���������� ������!"));
                if(mb.exec()==QMessageBox::Ok)
                    continue;
                else
                    break;
            }
            fileStream >> fpnBuffer;

        } //else qDebug("fpn info is absent");

        if(sizes[4])
        {
            fileStream >> rsize;
            if(rsize!=sizes[4])
            {
                mb.setText(QString().fromLocal8Bit("������, ������������ ������\n������ ������������!"));//����� ����
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
                mb.setText(QString().fromLocal8Bit("������, ������������ ������\n������ �������� ������!"));//����� ����
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
            mb.setText(QString().fromLocal8Bit("������, �� ������� ���������\n������������� ������ ��������!"));
            if(mb.exec()==QMessageBox::Ok)
                continue;
            else
                break;
        }
        indexMagicIdTest[strlen(indexMagicId)] = '\0';
        if( ! strcmp(indexMagicIdTest,indexMagicId) == 0 )
        {
            mb.setText(QString().fromLocal8Bit("������, ������������ ������������� ������ ��������!"));
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
                        //qDebug("File is broken!");
                        mb.setText(QString().fromLocal8Bit("������ ������� �����!"));
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
                mb.setText(QString().fromLocal8Bit("������, ���� �� �������� ������!"));
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

            //statusBar()->setToolTip(QString().fromLocal8Bit("���� �������� %1 ������").arg(totalFrames));
        }

        free(fvlabMagicIdTest);
        free(indexMagicIdTest);
        fmap = FVRFile.map(0,FVRFile.size());
        break;
    }
    //vw->newSourceFile(fmap, totalFrames, framesPos, &cameraStateHeader);
}
