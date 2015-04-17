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

public slots:
    void source();
    void setColor();
    void saveImage();
    void openImage();
    void nextCadr();
    void beforCadr();
    void streamConnection();
    void getResult(QImage *image);

};

#endif // VIEW_H
