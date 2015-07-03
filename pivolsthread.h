#ifndef Pivolsthread_H
#define Pivolsthread_H

#include <QThread>
#include <QReadWriteLock>
#include "pivols.h"

#define PACKET_SIZE 792
#define VI_SIZE 768
#define BUF_SIZE 228096
#define PICTURE_SIZE 221184


class Pivolsthread: public QThread
{
    Q_OBJECT

    unsigned char *buffer1;
    unsigned char *buffer2;

    bool buffer_flag;
    bool stop;

    ~Pivolsthread();
public:
    explicit Pivolsthread(QObject *parent = 0);
protected:
    void run();
signals:
    void stopWork();
    void sendResult(unsigned char *buf, bool flag);
    void finished();
public slots:
    void stopped();
};

#endif // Pivolsthread_H
