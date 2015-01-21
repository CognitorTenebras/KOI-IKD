#ifndef MOVIEFILEPARAMS_H
#define MOVIEFILEPARAMS_H

#include <QSettings>

struct MovieFileParams
{
    QString fName;
    QString comments;
    int  codecId;
    int  compression;
    int  rate;
    int  selectedFrame;
    int  quality;
    //
    MovieFileParams(void)
    {
        fName.clear();
        comments.clear();
        codecId = -2;
        compression = 110;
        rate = 25;
        selectedFrame = 0;
        quality = 100;
    }
    //
    bool isFVRawFormat(void)
    {
        return (codecId == -2);
    }
    //
    static bool FpnCorrection(int compression)
    {
        return ((qAbs(compression)/100)==1);
    }
    //
    static int CompressionLevel(int compression)
    {
        return (compression < 0) ? 0 : qAbs(compression)%100;
    }
    //
    static bool CompressionEnabled(int compression)
    {
        return (compression>0);
    }
    //
    static int CompressionID(int level, bool fpnCorrection)
    {
        level = level%100;
        if( fpnCorrection )
            level += 100;
        return level;
    }
    //
    void readSettings(QSettings & parent)
    {
        QSettings settings;
        settings.beginGroup(parent.group() + "/MovieFileParams");
        fName = settings.value("MovieName",fName).toString();
        comments = settings.value("Comments",comments).toString();
        codecId = settings.value("CodecId",codecId).toInt();
        compression = settings.value("Compression",compression).toInt();
        rate = settings.value("Rate",rate).toInt();
        quality = settings.value("Quality",quality).toInt();
        settings.endGroup();
    }
    void writeSettings(QSettings & parent)
    {
        QSettings settings;
        settings.beginGroup(parent.group() + "/MovieFileParams");
        settings.setValue("MovieName",fName);
        settings.setValue("Comments",comments);
        settings.setValue("CodecId",codecId);
        settings.setValue("Compression",compression);
        settings.setValue("Rate",rate);
        settings.setValue("Quality",quality);
        settings.endGroup();
    }
    //
};

#endif // MOVIEFILEPARAMS_H
