#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QHBoxLayout>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    QHBoxLayout *hl= new QHBoxLayout;
    v = new view;
    v->show();
    //hl->addWidget(v);
    //ui->centralWidget->setLayout(hl);
}

MainWindow::~MainWindow()
{
    delete v;
    delete ui;
}
