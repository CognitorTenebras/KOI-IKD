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
#include <QMessageBox>
#include <QSpinBox>
#include <QPainter>
#include <QPicture>
#include <QGroupBox>
#include <QMutex>

#include "picture.h"
#include "pivols.h"
#include "pivolsthread.h"
#include "videoplayer.h"

using namespace std;

class view : public QWidget
{
    Q_OBJECT
public:
    explicit view(QWidget *parent = 0);
    ~view();

private:

    QMessageBox msgBox;
    //QPushButton *sourceButton, *stopSourceButton;
    QLabel *lbl; //вывод изображения
    QLabel *lblconnection;//статус соединения
    QLabel *lblfile;//выбранный файл
    QLabel *glbl;
    QPicture paintFigure;
    Pivolsthread *pivols;
    QPushButton *startStreamButton, *stopStreamButton;

    QPushButton *imgSave;
    QPushButton *imgOpen;

    QPushButton *openVideoBut, *playVideoBut, *stopVideoBut, *recordVideoBut;

    QRadioButton *rbw, *rpse;
    QString videoFile;

    QRadioButton *picRow, *picColumn;

    QSpinBox *spinRow, *spinColumn;
    QPushButton *showHistogram;
    QGroupBox *colourBox;

    QPainter paint;

    Picture *pic;

    int cadr;
    bool stream;
    bool recording;
    int column;
    int row;

    unsigned char *buffer;

    QVector<QRgb> colorTableRBW,colorTableRPSE;

    void enable(bool Visible);
    void enableVideo(bool Visible);
    void enableStream();

protected:
    void closeEvent(QCloseEvent *event);
    void paintEvent(QPaintEvent *);

signals:
    void closed();
    void pictureStop();
    void beginRecord(const QString s);
    void stopRecord();
    void optionSignal(int);
    void stopPivols();


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
    void drawFigure(unsigned char *buf);
};

#endif // VIEW_H
