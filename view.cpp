#include "view.h"


#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QMessageBox>
#include <QFileDialog>
#include <QGridLayout>
#include <QtDebug>



view::view(QWidget *parent) :
    QWidget(parent)
{

    //Инициализация переменных
    cadr=0;
    stream=false;
    recording=false;
    numberCadr=0;

    //Задание таблиц цветов
    colorTableRBW.reserve(256);
    colorTableRPSE.reserve(256);

    for(int i=0;i<256;i++) //таблица цветов для черно-белого
    {
        colorTableRBW.insert(i,0xFF000000|((uchar)(i)<<16)|((uchar)(i)<<8)|(uchar)(i));
    }

    for(int i=0;i<42;i++) //таблица цветов для псевдоцвета
        colorTableRPSE.insert(i, 0xFF000000|((uchar)(0)<<16)|((uchar)(0)<<8)|(uchar)(200+i));
    for(int i=0;i<42;i++)
        colorTableRPSE.insert(42+i,0xFF000000|((uchar)(0)<<16)|((uchar)(200+i)<<8)|(uchar)(0xFF));
    for(int i=0;i<56;i++)
        colorTableRPSE.insert(i+84,0xFF000000|((uchar)(0)<<16)|((uchar)(190+i)<<8)|(uchar)(0));
    for(int i=0;i<42;i++)
        colorTableRPSE.insert(i+130,0xFF000000|((uchar)(200+i)<<16)|((uchar)(0xFF)<<8)|(uchar)(0));
    for(int i=0;i<42;i++)
        colorTableRPSE.insert(i+172,0xFF000000|((uchar)(0xFF)<<16)|((uchar)(127+i)<<8)|(uchar)(0));
    for(int i=0;i<42;i++)
        colorTableRPSE.insert(i+214,0xFF000000|((uchar)(200+i)<<16)|((uchar)(0)<<8)|(uchar)(0));

    //Определение лейаутов
    this->setFixedSize(650,700);
    QHBoxLayout *mainlayout= new QHBoxLayout;//главный layout
    QVBoxLayout *videolayout = new QVBoxLayout;//виджеты видео потока
    QVBoxLayout *streamlayout = new QVBoxLayout;//виджеты работы с потоком
    colourBox = new QGroupBox;

    lbl = new QLabel(this);//вывод кадров
    lbl->setFixedSize(384,288);

    glbl = new QLabel(this);
    glbl->setFixedSize(385,289);
    glbl->setPicture(paintFigure);

    //Сообщение о соединение с платой ПИВОЛС
    lblconnection = new QLabel;
    lblconnection->setFixedSize(200,30);
    lblconnection->setText("Соединение...");

    //Кнопки работы с потоком
    startStreamButton = new QPushButton("Старт");
    startStreamButton->setMaximumSize(160,30);
    stopStreamButton = new QPushButton("Стоп");
    stopStreamButton->setMaximumSize(160,30);

    //Установка виджетов потока в лейаут работы с потоком
    streamlayout->addWidget(lblconnection);
    streamlayout->addWidget(startStreamButton);
    streamlayout->addWidget(stopStreamButton);

    //Виджеты работы с видео файлом
    QHBoxLayout *cadrslayout = new QHBoxLayout; //расположение виджетов для потока кадров
    openVideoBut = new QPushButton("Открыть видео файл");
    openVideoBut->setMaximumSize(160,30);

    playVideoBut= new QPushButton;//начать воспроизведение потока
    playVideoBut->setMaximumSize(30,30);
    playVideoBut->setIcon(QIcon("../KOI-IKD/play.jpg"));

    stopVideoBut= new QPushButton;//остановить поток
    stopVideoBut->setMaximumSize(30,30);
    stopVideoBut->setIcon(QIcon("../KOI-IKD/stop.jpg"));

    recordVideoBut= new QPushButton;//начать или закончить запись
    recordVideoBut->setMaximumSize(30,30);
    recordVideoBut->setIcon(QIcon("../KOI-IKD/recordNO.jpg"));

    lblfile = new QLabel("Выбранный файл:");

    //Установка виджетов в лейаут
    cadrslayout->addWidget(playVideoBut);
    cadrslayout->addWidget(stopVideoBut);
    cadrslayout->addWidget(recordVideoBut);

    //Кнопки по работе с кадром
    imgOpen = new QPushButton("Открыть изображение");//открыть кадр
    imgOpen->setMaximumSize(160,30);
    imgSave = new QPushButton("Сохранить изображение");//сохранить кадр
    imgSave->setMaximumSize(160,30);

    //Параметры работы с гистограммой
    QHBoxLayout *layoutRow = new QHBoxLayout;
    picRow = new QRadioButton("Строка");
    spinRow = new QSpinBox();
    spinRow->setMaximum(384);
    spinRow->setMinimum(1);
    layoutRow->addWidget(picRow);
    layoutRow->addWidget(spinRow);
    QHBoxLayout *layoutColumn = new QHBoxLayout;
    picColumn = new QRadioButton("Столбец");
    spinColumn  = new QSpinBox();
    spinColumn ->setMaximum(384);
    spinColumn ->setMinimum(1);
    layoutColumn ->addWidget(picColumn);
    layoutColumn ->addWidget(spinColumn);

    //Задание опций кадра
    QVBoxLayout *colourVBox = new QVBoxLayout;
    colourBox->setTitle("Цветовая схема");
    rbw = new QRadioButton("Черно-белый",this); //черно белый вариант
    rbw->setChecked(true);
    rpse = new QRadioButton("Псевдоцвет", this); //псевдоцвет
    colourVBox->addWidget(rbw);
    colourVBox->addWidget(rpse);
    colourBox->setLayout(colourVBox);

    videoSlider = new QSlider(Qt::Horizontal);
    videoSlider->setGeometry(0,0,288,30);



    //Создание разделителей для панели
    QFrame *hline = new QFrame;
    hline->setFrameShape(QFrame::HLine);
    hline->setFrameShadow(QFrame::Sunken);
    QFrame *hline_1 = new QFrame;
    hline_1->setFrameShape(QFrame::HLine);
    hline_1->setFrameShadow(QFrame::Sunken);
    QFrame *hline_2 = new QFrame;
    hline_2->setFrameShape(QFrame::HLine);
    hline_2->setFrameShadow(QFrame::Sunken);
    QFrame *hline_3 = new QFrame;
    hline_3->setFrameShape(QFrame::HLine);
    hline_3->setFrameShadow(QFrame::Sunken);

    //Установка виджетов панели лейаутов
    videolayout->addLayout(streamlayout);
    videolayout->addWidget(hline);
    videolayout->addWidget(openVideoBut);
    videolayout->addLayout(cadrslayout);
    videolayout->addWidget(lblfile);
    videolayout->addWidget(hline_1);
    videolayout->addWidget(imgOpen);
    videolayout->addWidget(imgSave);
    videolayout->addWidget(hline_2);
    videolayout->addLayout(layoutRow);
    videolayout->addLayout(layoutColumn);
    videolayout->addWidget(hline_3);
    videolayout->addWidget(colourBox);
    videolayout->addStretch();

    //Заполнение главного лейаута
    QFrame *vline = new QFrame;
    vline->setFrameShape(QFrame::VLine);
    vline->setFrameShadow(QFrame::Sunken);
    QVBoxLayout *lblLayout = new QVBoxLayout;
    lblLayout->addWidget(lbl);
    lblLayout->addStretch();
    lblLayout->addWidget(videoSlider);
    lblLayout->addStretch();
    lblLayout->addWidget(glbl);
    mainlayout->addLayout(lblLayout);
    mainlayout->addStretch();
    mainlayout->addWidget(vline);
    mainlayout->addLayout(videolayout);

    //установка главного лейаута
    this->setLayout(mainlayout);

    //задание сигналов-слотов
    connect(startStreamButton,SIGNAL(clicked()),this,SLOT(startStream()));
    connect(stopStreamButton,SIGNAL(clicked()),this,SLOT(stopStream()));
    connect(openVideoBut,SIGNAL(clicked()),this,SLOT(open()));
    connect(imgSave,SIGNAL(clicked()),this,SLOT(saveImage()));
    connect(imgOpen,SIGNAL(clicked()),this,SLOT(openImage()));
    connect(rbw,SIGNAL(clicked()),this,SLOT(setColor()));
    connect(rpse,SIGNAL(clicked()),this,SLOT(setColor()));
    connect(playVideoBut,SIGNAL(clicked()),this,SLOT(play()));
    connect(stopVideoBut,SIGNAL(clicked()),this,SLOT(stop()));
    connect(recordVideoBut,SIGNAL(clicked()),this,SLOT(record()));
    //connect(videoSlider,SIGNAL(valueChanged(int)),this,SLOT(changeSlider()),Qt::DirectConnection);
    connect(videoSlider,SIGNAL(sliderMoved(int)),this,SLOT(changeSlider()),Qt::DirectConnection);
    connect(videoSlider,SIGNAL(sliderPressed()),this,SLOT(changeSlider()),Qt::DirectConnection);
    connect(videoSlider,SIGNAL(sliderReleased()),this,SLOT(changeSlider()),Qt::DirectConnection);

    //создание потока для воспроизведения tiff
    player = new videoPlayer();
    connect(this,&view::openTIFF,player,&videoPlayer::openVideo);
    connect(player,&videoPlayer::numberDir,this,&view::setSizeSlider);
    connect(this,&view::stopVideo,player,&videoPlayer::stopVideo);
    connect(this,&view::playVideo,player,&videoPlayer::startVideo);
    connect(player,&videoPlayer::sendPicture,this,&view::getResult);
    connect(this,&view::stopVideo,player,&videoPlayer::stopVideo);
    connect(player,&videoPlayer::numberDir,this,&view::setSizeSlider);
    connect(player,&videoPlayer::framePosition,this,&view::setPositionSlider);
    player->start();

    //подключение к пиволсу
    if(RegisterCard((unsigned long)0xA116FD719D83,BOTH_CHANNEL_AND_MARKER))
        stream=true;
    streamConnection();
    enableVideo(false);

    //потока обработки данных с пиволс
    pic=new Picture();
    connect(pic,&Picture::sendPicture,this,&view::getResult);
    connect(pic,&Picture::finished,pic,&view::deleteLater);
    connect(this,&view::pictureStop,pic,&Picture::stopped);
    connect(this,&view::beginRecord,pic,&Picture::startRecord);
    connect(this,&view::stopRecord,pic,&Picture::stopRecord);
    connect(pic,&Picture::sendFigure,this,&view::drawFigure);

    //потоко получения даннхы по пиволс
    pivols = new Pivolsthread();
    connect(pivols,&Pivolsthread::sendResult,
            pic,&Picture::makePicture);
    connect(pivols,&Pivolsthread::finished,pivols,&Pivolsthread::deleteLater);
    connect(pivols,&Pivolsthread::stopWork,pic,&Picture::stopped);
    connect(this,&view::stopPivols,pivols,&Pivolsthread::stopped);
}

view::~view()
{
    //закрытие потоков если они запущены (мне кажеться это надо делать не здесь)
    if(player->isRunning())
    {
        player->exit();
        player->wait();
        qDebug()<<"VideoPlayer "<<player->isRunning();
    }
    if(pic->isRunning())
    {
        pic->exit();
        pic->wait();
        qDebug()<<"Picture "<<pic->isRunning();
    }

    if(pivols->isRunning())
    {
        emit stopPivols();
        pivols->wait();
        qDebug()<<"Pivols "<<pivols->isRunning();
    }
}

void view::closeEvent(QCloseEvent *event)
{
    //а здесь (не успел проверить как правильно)
    if(player->isRunning())
    {
        player->exit();
        player->wait();
        qDebug()<<"VideoPlayer "<<player->isRunning();
    }
    if(pic->isRunning())
    {
        pic->exit();
        pic->wait();
        qDebug()<<"Picture "<<pic->isRunning();
    }

    if(pivols->isRunning())
    {
        emit stopPivols();
        pivols->wait();
        qDebug()<<"Pivols "<<pivols->isRunning();
    }

    emit closed();
    event->accept();
}

void view::setColor()
{

}

void view::getResult(QImage* image)
{
    // установка таблицы цветов
    if (rbw->isChecked()==true) //черно-белый
        image->setColorTable(colorTableRBW);

    if (rpse->isChecked()==true)//псевдоцвет
        image->setColorTable(colorTableRPSE);

    //вывод картинки на экран
    lbl->setPixmap(QPixmap::fromImage(*image).scaled(QSize(384,288)));
    setUpdatesEnabled(true);
    repaint(); //магическая перерисовка
    setUpdatesEnabled(false);
}
void view::startStream()
{
    /*
    //потока обработки данных с пиволс
    pic=new Picture();
    connect(pic,&Picture::sendPicture,this,&view::getResult);
    connect(pic,&Picture::finished,pic,&view::deleteLater);
    connect(this,&view::pictureStop,pic,&Picture::stopped);
    connect(this,&view::beginRecord,pic,&Picture::startRecord);
    connect(this,&view::stopRecord,pic,&Picture::stopRecord);
    connect(pic,&Picture::sendFigure,this,&view::drawFigure);

    //потоко получения даннхы по пиволс
    pivols = new Pivolsthread();
    connect(pivols,&Pivolsthread::sendResult,
            pic,&Picture::makePicture);
    connect(pivols,&Pivolsthread::finished,pivols,&Pivolsthread::deleteLater);
    connect(pivols,&Pivolsthread::stopWork,pic,&Picture::stopped);
    connect(this,&view::stopPivols,pivols,&Pivolsthread::stopped);
    */

    //запуск потоков для вывода видео потока
    //может получится так, что программа будет завершаться.
    //я не понял в чем причина. Надо будет убрать закоментированые выше и убрать это из конструктора
    pic->start();
    pivols->start();
}

void view::stopStream()
{
    //закрытие потоков воспроизведения видеопотка
    //есть конфликт с мютексом. не успел его решить
    //придеться тебе придумать что-то :)
    if(pic->isRunning())
    {
        pic->exit();
        pic->wait();
        qDebug()<<pic->isRunning();
    }
    if(pivols->isRunning())
    {
        emit stopPivols();
        pivols->wait();
        qDebug()<<pivols->isRunning();
    }

    lblconnection->setText("Поток остановлен");
}

void view::play()
{
    //воспроизведение tiff принимает только файлы записанные этой же программой
    if(videoFile!="")
    {
       emit playVideo(videoFile,videoSlider->value());
    }
    else
    {
        msgBox.setText("Выберите tif файл");
        msgBox.exec();
    }
}

void view::stop()
{
    emit stopVideo();
}

void view::record()
{
    //ничего сложного
    if(videoFile!="")
        if(!recording)
        {
            recording=true;
            emit beginRecord(videoFile);
            recordVideoBut->setIcon(QIcon("../KOI-IKD/recordYES.jpg"));
        }
        else
        {
            recording=false;
            emit stopRecord();
            recordVideoBut->setIcon(QIcon("../KOI-IKD/recordNO.jpg"));
        }
    else
    {
        msgBox.setText("Выберите файл для записи");
        msgBox.exec();
    }

}

void view::open()
{
    videoFile = QFileDialog::getOpenFileName(this,"Выберите файл","C:\\",
                                                          "Любой файл(*)");
    if(videoFile!="")
    {
        //отбрасываю лишнее чтобы вывести на экран какой файл открыт
        QRegExp rx("(\\w+.tif)");
        QString str;
        int pos = 0;
        pos = rx.indexIn(videoFile, pos);
        str = rx.cap(1);
        lblfile->setText("Выбраный файл: "+str);
        emit openTIFF(videoFile.toLocal8Bit().constData());

    }
}

void view::saveImage()
{
    QString imageName = QFileDialog::getSaveFileName(this,"Выберите файл для сохранения","C:\\",
                                                          "Изображение (*.jpg");
    if(imageName!="")
            lbl->pixmap()->save(imageName,"JPG");

}

void view::openImage()
{
    QString imageName = QFileDialog::getOpenFileName(this,"Выберите файл","C:\\",
                                                          "Изображение (*.jpg)");
     if(imageName!="")
     {
         QPixmap *pix=new QPixmap;
         pix->load(imageName);
            lbl->setPixmap(*pix);
     }
     enable(false);
}

void view::enable(bool Visible)
{
    if(Visible==0)
    {
        startStreamButton->setEnabled(false);
        stopStreamButton->setEnabled(false);
        imgSave->setEnabled(false);

        playVideoBut->setEnabled(false);
        stopVideoBut->setEnabled(false);

        rbw->setEnabled(false);
        rpse->setEnabled(false);
    }
    else
    {
        imgSave->setEnabled(true);
        rbw->setEnabled(true);
        rpse->setEnabled(true);
    }
}

void view::enableVideo(bool Visible)
{
    if(Visible==0)
    {
        playVideoBut->setEnabled(true);
        stopVideoBut->setEnabled(true);
        recordVideoBut->setEnabled(true);
    }
    else
    {
        playVideoBut->setEnabled(true);
        stopVideoBut->setEnabled(true);
        recordVideoBut->setEnabled(true);
    }
}

void view::enableStream()
{
    if(stream==false)
    {
        startStreamButton->setEnabled(false);
        stopStreamButton->setEnabled(false);
    }
    else
    {
        startStreamButton->setEnabled(true);
        stopStreamButton->setEnabled(true);
    }
}

void view::streamConnection()
{
    if(stream==true)
    {
        lblconnection->setText("Соединение установлено");
        enableStream();
    }
    else
    {
        lblconnection->setText("Нет соединения");
        enableStream();
    }
}

void view::drawFigure(unsigned char *buf)
{
    if(picRow->isChecked()||picColumn->isChecked())
    {
        QPainter p(&paintFigure);
        p.drawLine(10,0,10,255); //оси и стрелки
        p.drawLine(10,255,384,255);
        p.drawLine(0,10,10,0);
        p.drawLine(20,10,10,0);
        p.drawLine(374,245,384,255);
        p.drawLine(374,265,384,255);
        p.drawRect(0,0,384,255);
        row = spinRow->value();
        column = spinColumn->value();
        QPainterPath path;
        if(picRow->isChecked()) //по строке
            for(int i=0;i<384;i++)
                path.lineTo(i,255-buf[row*384+i]);

        if(picColumn->isChecked()) //по столбцу
            for(int i=0;i<288;i++)
                path.lineTo(i,255-buf[column+i*384]);//рисование ломанй линии по точкам
        p.drawPath(path);
    }
}

void view::changeSlider()
{
    numberCadr=videoSlider->value();
    setUpdatesEnabled(true);
    repaint();
    setUpdatesEnabled(false);
}

void view::setSizeSlider(int sizeSlider)
{
    if(sizeSlider>0)
    {
        videoSlider->setMaximum(sizeSlider);
        videoSlider->setEnabled(true);
        enableVideo(true);
    }
}

void view::setPositionSlider(int position)
{
    videoSlider->setValue(position);
    repaint();
}
