#ifndef PIVOLSTHREAD_H
#define PIVOLSTHREAD_H

#include "stdafx.h"
#include "pivols.h"

#include <QThread>
#include <QReadWriteLock>


#define PACKET_SIZE 792


class pivolsthread: public QThread
{
    Q_OBJECT

    unsigned char *buffer1;
    unsigned char *buffer2;
    bool buffer_flag;
    bool stop;

public:
    explicit pivolsthread(QObject *parent = 0);
protected:
    void run();

signals:
    void finished();
    void sendResult(unsigned char *buf);
public slots:
    void stopped(bool s);
};

#endif // PIVOLSTHREAD_H
