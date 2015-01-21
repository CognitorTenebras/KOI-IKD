#ifndef FVCAMERASTATEINFO_H
#define FVCAMERASTATEINFO_H

#include <QRect>
#include "fvtypes.h"
//#include "fvcamerabase.h"
#include "MovieFileParams.h"
#include <QDebug>

class FVGrabberBase;


class FVCameraStateInfo
{
protected:
    qint64 time;
    int    id;
    int    bpp;
    double fps;
    double exposure;
    double ds_exposure;
    double ts_exposure;
    int    blackLevel;
    int    frameSize;
    int    compress;
    int    filledSize;
    QRect  acqRect;
    QRect  sensorRect;
public:
    FVCameraStateInfo(){}
    /*FVCameraStateInfo(const FVCameraStateInfo & info);
    FVCameraStateInfo(FVGrabberBase *grabber);
    bool Fill(FVGrabberBase *grabber);
    void Clear(void);*/
    ~FVCameraStateInfo()
    {
    }
    //
    const int GetAcqWidth()
    {
        return acqRect.width();
    }
    const int GetAcqHeight(void)
    {
        return acqRect.height();
    }
    const int GetBPP(void)
    {
        return bpp;
    }
    //
    int GetFilledSize(void)
    {
        return filledSize;
    }
    //
    double GetFPS(void)
    {
        return fps;
    }
    //
    int GetFrameSize(void)
    {
        return frameSize;
    }
    //
    QRect & GetAcqRect(void)
    {
        return acqRect;
    }
    QRect & GetSensorRect(void)
    {
        return sensorRect;
    }
    //
    int GetId()
    {
        return id;
    }
    //
    bool isSS(void)
    {
        return false;//FVCameraBase::isSS(id);
    }
    //
    int GetCompressionLevel(void)
    {
        return MovieFileParams::CompressionLevel(compress);
    }
    //
    bool SubstructFpn(void)
    {
        if( (compress < 0) || (GetCompressionLevel() == 0) )
            return false;
        return MovieFileParams::FpnCorrection(compress);
    }
    //
    void SetCompression(int c)
    {
        compress = c;
    }

    qint64 GetTimeMsec()
    {
        return time;
    }

    void QPrintAll()
    {
        qDebug("File info:");
        qDebug("time: %lld",time);
        qDebug("id: %d",id);
        qDebug("bpp: %d",bpp);
        qDebug("fps: %f",fps);
        qDebug("exposure: %f",exposure);
        qDebug("ds_exposure: %f",ds_exposure);
        qDebug("ts_exposure: %f",ts_exposure);
        qDebug("blackLevel: %d",blackLevel);
        qDebug("frameSize: %d",frameSize);
        qDebug("compress: %d",compress);
        qDebug("filledSize: %d",filledSize);
        qDebug("acqRect(x1,y1,x2,y2): %d, %d, %d, %d",acqRect.left(),acqRect.top(),acqRect.right(),acqRect.bottom());
        qDebug("sensorRect(x1,y1,x2,y2): %d, %d, %d, %d",sensorRect.left(),sensorRect.top(),sensorRect.right(),sensorRect.bottom());
    }
    //
    //bool GetInformation(StringMap & smap);

    /*switch ( config->Mode&0x3)
    {
        case 0: // 10 bits/pixel -> 2 bytes
        FrameSizeInBytes *= 2;
        cout << " 10 bits/pixel -> 2 bytes/pixel: 9-0 bits " << endl;
        break;
        case 1:
        cout << " 8 bits/pixel -> 1 bytes/pixel : 9-2 bits" << endl;
        break;
        case 2:
        cout << " 8 bits/pixel -> 1 bytes/pixel : 8-1 bits" << endl;
        break;
        case 3:
        cout << " 8 bits/pixel -> 1 bytes/pixel : 7-0 bits" << endl;
        break;
    }*/
};


#endif // FVCAMERASTATEINFO_H
