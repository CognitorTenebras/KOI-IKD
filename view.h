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
#include "picture.h"

using namespace std;

class view : public QWidget
{
    Q_OBJECT
public:
    explicit view(QWidget *parent = 0);
    ~view();

private:

    //QPushButton *sourceButton, *stopSourceButton;
    QLabel *lbl, *lblbw, *lblpse, *lblcadr, *lblconnection;
    QPushButton *startStreamButton, *stopStreamButton;

    QPushButton *imgSave;
    QPushButton *imgOpen;

    QPushButton *openVideoBut, *playVideoBut, *stopVideoBut, *recordVideoBut;

    QRadioButton *rbw, *rpse;
    QString videoFile;

    int cadr;
    bool stream;
    bool recording;

    unsigned char *buffer;

    QVector<QRgb> colorTableRBW,colorTableRPSE;

    void enable(bool Visible);
    void enableStream(bool Visible);
    void enableVideo(bool Visible);

protected:
    void closeEvent(QCloseEvent *event);

signals:
    void closed();
    void pictureStop(bool);
    void beginRecord(const QString s);
    void stopRecord();


public slots:
    void play();
    void stop();
    void record();
    void open();
    void stopStream();
    void startStream();
    void setColor();
    void saveImage();
    void openImage();
    void streamConnection();
    void getResult(QImage *image);

};

#endif // VIEW_H
