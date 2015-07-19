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
#include <QSlider>

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

    QLabel *lbl; //вывод изображения
    QLabel *lblconnection;//статус соединения
    QLabel *lblfile;//выбранный файл
    QLabel *glbl; //рисование графика

    QPicture paintFigure; //для графика
    QPainter paint;

    QPushButton *startStreamButton, *stopStreamButton; //видеопоток
    QPushButton *imgSave; //jpg файл
    QPushButton *imgOpen;
    QPushButton *openVideoBut, *playVideoBut, *stopVideoBut, *recordVideoBut; //tiff файл

    QString videoFile; //название TIFF файла

    QRadioButton *rbw, *rpse; //черно белый или псевдоцвет
    QRadioButton *picRow, *picColumn; // график по строке или по столбцу

    QSpinBox *spinRow, *spinColumn; //номер столбца или строки
    QGroupBox *colourBox;

    QSlider *videoSlider; //перемотка видео

    Pivolsthread *pivols; //поток получения данных с пиволса
    Picture *pic; //поток обработки данных с пиволса
    videoPlayer *player; //поток для обработки видео из TIFF

    int cadr;
    bool stream;
    bool recording;
    int column;
    int row;
    int numberCadr;

    unsigned char *buffer;

    QVector<QRgb> colorTableRBW,colorTableRPSE;

    void enable(bool Visible); //ограничение пользователя
    void enableVideo(bool Visible);
    void enableStream();

protected:
    void closeEvent(QCloseEvent *event);
signals:
    void closed();
    void pictureStop(); //остановка потока pic
    void beginRecord(const QString s); //начало записи из потока
    void stopRecord(); //остановка записи из потока
    void stopPivols(); //отсановка потока pivols
    void openTIFF(const char* s); //открытие tiff файла для определения количества кадров
    void stopVideo(); //остановка воспроизведения (поток не останавливается)
    void playVideo(QString,int); //начало воспроизведения видео

public slots:
    void play(); //воспроизведение tiff
    void stop(); //остановка воспроизведения
    void record(); //запись вкл и выкл
    void open(); //открытие tiff файла
    void stopStream(); //показать поток
    void startStream(); //остановить поток
    void setColor(); //установка цвета картинки
    void saveImage(); //сохранить кадр в jpg
    void openImage(); //открыть файл jpg
    void streamConnection(); //проверка соединения с пиволс
    void getResult(QImage *image); //получение кадра из pic и вывод его
    void drawFigure(unsigned char *buf); //рисование графика
    void changeSlider(); //перетаскивание слайдера
    void setSizeSlider(int sizeSlider); //установка количества кадров в слайдере
    void setPositionSlider(int position); //установка слайдера в позицию


};

#endif // VIEW_H
