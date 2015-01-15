#ifndef VIEW_H
#define VIEW_H

#include <vector>
#include <QWidget>
#include <QLabel>
#include <QPushButton>
#include <QFile>
#include <QList>

using namespace std;

class view : public QWidget
{
    Q_OBJECT
public:
    explicit view(QWidget *parent = 0);

private:

    QLabel *lbl;
    QPushButton *fileButton;

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

    QString FileName;

    vector<fInfo*> idxs;
    qint64 *framesPos;
    qint64 indexStartPos;
    qint64 frameStartPos;

    //FVCameraStateInfo cameraStateHeader;

    QByteArray fpnBuffer;
    QByteArray commentBuffer;
    QByteArray cameraName;

    int totalFrames;

    //filterParamsS * parameters;
    //Params * parametersW;

    quint32 numSectors;
    quint16 *sectors;

    QFile FVRFile;
    uchar *fmap;

signals:

public slots:
    void open();


};

#endif // VIEW_H
