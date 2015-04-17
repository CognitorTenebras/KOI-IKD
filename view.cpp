#include "view.h"


#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QMessageBox>
#include <QFileDialog>
#include <QGridLayout>


view::view(QWidget *parent) :
    QWidget(parent)
{

    cadr=0;
    stream=false;

    colorTableRBW.reserve(256);
    colorTableRPSE.reserve(256);
    for(int i=0;i<256;i++) //формируем таблицу цветов для черно-белого
    {
        colorTableRBW[i] =  0xFF000000|((uchar)(i)<<16)|((uchar)(i)<<8)|(uchar)(i);
    }
    for(int i=0;i<42;i++) //формируем таблицу цветов для псевдоцвета
        colorTableRPSE[i] =  0xFF000000|((uchar)(0)<<16)|((uchar)(0)<<8)|(uchar)(200+i);
    for(int i=0;i<42;i++)
        colorTableRPSE[42+i] =  0xFF000000|((uchar)(0)<<16)|((uchar)(200+i)<<8)|(uchar)(0xFF);
    for(int i=0;i<56;i++)
        colorTableRPSE[84+i] =  0xFF000000|((uchar)(0)<<16)|((uchar)(190+i)<<8)|(uchar)(0);
    for(int i=0;i<42;i++)
        colorTableRPSE[130+i] =  0xFF000000|((uchar)(200+i)<<16)|((uchar)(0xFF)<<8)|(uchar)(0);
    for(int i=0;i<42;i++)
        colorTableRPSE[172+i] =  0xFF000000|((uchar)(0xFF)<<16)|((uchar)(127+i)<<8)|(uchar)(0);
    for(int i=0;i<42;i++)
        colorTableRPSE[214+i] =  0xFF000000|((uchar)(200+i)<<16)|((uchar)(0)<<8)|(uchar)(0);


    this->setFixedSize(650,380);
    QHBoxLayout *mainlayout= new QHBoxLayout;//главный layout
    QVBoxLayout *videolayout = new QVBoxLayout;//виджеты одного кадра
    QVBoxLayout *streamlayout = new QVBoxLayout;//виджеты работы с потоком


    lbl = new QLabel(this);//вывод кадров
    lbl->setFixedSize(384,288);
    //lbl->setMaximumSize(384,288);

    fileButton = new QPushButton("Открыть источник");//открытие кадров
    fileButton->setMaximumSize(160,30);
    lblconnection = new QLabel;
    lblconnection->setFixedSize(200,30);
    lblconnection->setText("Соединение...");
    startStreamButton = new QPushButton("Старт");
    startStreamButton->setMaximumSize(160,30);
    stopStreamButton = new QPushButton("Стоп");
    stopStreamButton->setMaximumSize(160,30);

    streamlayout->addWidget(lblconnection);
    streamlayout->addWidget(startStreamButton);
    streamlayout->addWidget(stopStreamButton);


    QHBoxLayout *cadrslayout = new QHBoxLayout; //расположение виджетов для потока кадров
    openVideoBut = new QPushButton("Открыть видео файл");
    openVideoBut->setMaximumSize(160,30);
    lblcadr = new QLabel; //номер кадра
    lblcadr->setNum(cadr);

    nextBut = new QPushButton; //след кадр
    nextBut->setMaximumSize(30,30);
    nextBut->setIcon(QIcon("../KOI-IKD/next.jpg"));

    beforBut = new QPushButton; //предыдущий кадр
    beforBut->setMaximumSize(30,30);
    beforBut->setIcon(QIcon("../KOI-IKD/befor.jpg"));

    playVideoBut= new QPushButton;//начать воспроизведение потока
    playVideoBut->setMaximumSize(30,30);
    playVideoBut->setIcon(QIcon("../KOI-IKD/play.jpg"));

    stopVideoBut= new QPushButton;//остановить поток
    stopVideoBut->setMaximumSize(30,30);
    stopVideoBut->setIcon(QIcon("../KOI-IKD/stop.jpg"));

    //cadrslayout->addWidget(lblcadr);
    cadrslayout->addWidget(beforBut);
    cadrslayout->addWidget(playVideoBut);
    cadrslayout->addWidget(stopVideoBut);
    cadrslayout->addWidget(nextBut);


    imgOpen = new QPushButton("Открыть изображение");//открыть кадр
    imgOpen->setMaximumSize(160,30);
    imgSave = new QPushButton("Сохранить изображение");//сохранить кадр
    imgSave->setMaximumSize(160,30);


    rbw = new QRadioButton("Черно-белый",this); //черно белый вариант
    rbw->setChecked(true);
    rpse = new QRadioButton("Псевдоцвет", this); //псевдоцвет

    QFrame *hline = new QFrame;
    hline->setFrameShape(QFrame::HLine);
    hline->setFrameShadow(QFrame::Sunken);
    QFrame *hline_1 = new QFrame;
    hline_1->setFrameShape(QFrame::HLine);
    hline_1->setFrameShadow(QFrame::Sunken);
    QFrame *hline_2 = new QFrame;
    hline_2->setFrameShape(QFrame::HLine);
    hline_2->setFrameShadow(QFrame::Sunken);

    videolayout->addLayout(streamlayout);
    videolayout->addWidget(hline);
    videolayout->addWidget(fileButton);
    videolayout->addLayout(cadrslayout);
    videolayout->addWidget(hline_1);
    videolayout->addWidget(imgOpen);
    videolayout->addWidget(imgSave);
    videolayout->addWidget(hline_2);
    videolayout->addWidget(rbw);
    videolayout->addWidget(rpse);
    videolayout->addStretch();


    QFrame *vline = new QFrame;
    vline->setFrameShape(QFrame::VLine);
    vline->setFrameShadow(QFrame::Sunken);
    mainlayout->addWidget(lbl);
    mainlayout->addStretch();
    mainlayout->addWidget(vline);
    mainlayout->addLayout(videolayout);

    this->setLayout(mainlayout);

    connect(fileButton,SIGNAL(clicked()),this,SLOT(open()));
    connect(imgSave,SIGNAL(clicked()),this,SLOT(saveImage()));
    connect(imgOpen,SIGNAL(clicked()),this,SLOT(openImage()));
    connect(rbw,SIGNAL(clicked()),this,SLOT(setColor()));
    connect(rpse,SIGNAL(clicked()),this,SLOT(setColor()));
    connect(nextBut,SIGNAL(clicked()),this,SLOT(nextCadr()));
    connect(beforBut,SIGNAL(clicked()),this,SLOT(beforCadr()));
    //connect(playVideoBut,SIGNAL(clicked()),this,SLOT(play()));
    //connect(stopVideoBut,SIGNAL(clicked()),this,SLOT(stop()));
    streamConnection();
    enable(false);
}

view::~view()
{

}

void view::closeEvent(QCloseEvent *event)
{
    emit closed();
    event->accept();
}


void view::setColor()
{
    source();
}

void view::getResult(QImage* image)
{

    if (rbw->isChecked()==true) //черно-белый
        image->setColorTable(colorTableRBW);

    if (rpse->isChecked()==true)//псевдоцвет
        image->setColorTable(colorTableRPSE);

    lbl->setPixmap(QPixmap::fromImage(*image).scaled(QSize(384,288)));
}

void view::source()
{
    Picture *pic=new Picture(this);
    connect(pic,SIGNAL(sendPicture(QImage*)),this,SLOT(getResult(QImage*)));
    connect(pic,SIGNAL(finished()),pic,SLOT(deleteLater()));
    pic->start();
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

void view::nextCadr()
{
    if(cadr>=0&&cadr<100)
        cadr++;
    source();
    lblcadr->setNum(cadr);
}

void view::beforCadr()
{
    if(cadr>0&&cadr<=100)
        cadr--;
    source();
    lblcadr->setNum(cadr);
}

void view::enable(bool Visible)
{
    if(Visible==0)
    {
        startStreamButton->setEnabled(false);
        stopStreamButton->setEnabled(false);
        imgSave->setEnabled(false);
        nextBut->setEnabled(false);
        playVideoBut->setEnabled(false);
        stopVideoBut->setEnabled(false);
        beforBut->setEnabled(false);
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
         enable(false);
    else
    {
        nextBut->setEnabled(true);
        beforBut->setEnabled(true);
        playVideoBut->setEnabled(true);
        stopVideoBut->setEnabled(true);
        enable(true);
    }
}

void view::enableStream(bool Visible)
{
    if(Visible==0)
        enable(false);
    else
    {
        enable(true);
        startStreamButton->setEnabled(true);
        stopStreamButton->setEnabled(true);
    }
}

void view::streamConnection()
{
    if(stream==true)
    {
        lblconnection->setText("Соединение установлено");
        enableStream(true);
    }
    else
    {
        lblconnection->setText("Нет соединения");
        enableStream(false);
    }
}
