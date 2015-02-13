#ifndef VIEW_H
#define VIEW_H

#include <vector>
#include <QWidget>
#include <QLabel>
#include <QPushButton>
#include <QFile>
#include <QFileDevice>
#include <QList>
#include <QRadioButton>
#include <QCloseEvent>


#include "fvcamerastateinfo.h"

using namespace std;

struct sectorsS
{
    qint64 frameNum;
    quint32 Y;
    qint32 dX;
};

struct halfFinishedS
{
    explicit halfFinishedS():sectors(0){}
    explicit halfFinishedS(int sectorsCount)
    {
        sectors = new sectorsS[sectorsCount];
    }
    ~halfFinishedS()
    {
        if(sectors)
            free(sectors);
    }

    quint32 Xmin,Xmax;
    sectorsS *sectors;
};


class view : public QWidget
{
    Q_OBJECT
public:
    explicit view(QWidget *parent = 0);
    ~view();

private:

    QPushButton *fileButton;
    QLabel *lbl, *lblbw, *lblpse, *lblcadr, *lblconnection;
    QPushButton *startStreamButton, *stopStreamButton;

    QPushButton *imgSave;
    QPushButton *imgOpen;

    QPushButton *openVideoBut, *playVideoBut, *stopVideoBut, *nextBut, *beforBut;

    QRadioButton *rbw, *rpse;
    QString FileName;

    int cadr;
    bool stream;


    struct frameInfo
    {
        int gNumber;
        int wNumber;
        qint64 pos;
        int size;
        frameInfo(int _gNumber,int _wNumber, qint64 _pos, int _size)
        {
            gNumber = _gNumber;
            wNumber = _wNumber;
            pos     = _pos;
            size    = _size;
        }
        qint64 EndOfFrame(void)
        {
            return (size+pos);
        }
    };

    struct fInfo
    {
        int size;
        frameInfo* frames;
        fInfo()
        {
            size = 0;
            frames = (frameInfo*)0;
        }
        ~fInfo()
        {
            if(frames)
                free(frames);
        }
    };

    static const int maxFrameNumber = 65536;
    static const int frameInfoSize  = sizeof(frameInfo);
    static const int indexBufferSize = maxFrameNumber*frameInfoSize;
    //
    static const char *  fvlabMagicId;
    static const char * headerMagicId;
    static const char *    fpnMagicId;
    static const char *  indexMagicId;
    //
    static const qint32 ver = 0x03042012;
    static const int    maxSizes = 8;

    vector<fInfo*> idxs;
    qint64 *framesPos;
    qint64 indexStartPos;
    qint64 frameStartPos;

    QByteArray fpnBuffer;
    QByteArray commentBuffer;
    QByteArray cameraName;

    int totalFrames;

    quint32 numSectors;
    quint16 *sectors;

    FVCameraStateInfo cameraStateHeader;

    QFile FVRFile;
    uchar *fmap;

    int delta;
    int h, w;
    QList<halfFinishedS> preResult;
    int fromUPdoDOWN;
    int processedImgs;

    quint32 direction;

    uchar *tmpImg0,*tmpImg1;

    uchar *map;
    FVCameraStateInfo *fileInfo;
    QImage *image;

    void enable(bool Visible);
    void enableStream(bool Visible);
    void enableVideo(bool Visible);

protected:
    void closeEvent(QCloseEvent *event);

signals:
    void closed();

public slots:
    void open();
    void source(uchar *map, int totalFrames, qint64 *framesPos, FVCameraStateInfo *fileInfo);
    void setColor();
    void saveImage();
    void openImage();
    void nextCadr();
    void beforCadr();
    void streamConnection();

};

#endif // VIEW_H
