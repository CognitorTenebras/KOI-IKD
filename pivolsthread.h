#ifndef PIVOLSTHREAD_H
#define PIVOLSTHREAD_H


#include "pivols.h"

#include <QReadWriteLock>
#include <QThread>

#define PACKET_SIZE 792

class pivolsthread: public QThread
{
    unsigned char *buffer1;
    unsigned char *buffer2;
    QReadWriteLock *lock;
    bool buffer_flag;
    bool stopped;

public:
    pivolsthread(unsigned char *buf1, unsigned char *buf2, QReadWriteLock *l);

protected:
    void run();
};

#endif // PIVOLSTHREAD_H
