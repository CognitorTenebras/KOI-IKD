#include "pivolsthread.h"
#include <QMessageBox>

pivolsthread::pivolsthread(unsigned char *buf1, unsigned char *buf2, QReadWriteLock *l)
{
   buffer1=buf1;
   buffer2=buf2;
   lock=l;
   stopped=false;
   buffer_flag=false;
}

void pivolsthread::run()
{

    unsigned long count_read_byte=0;
    unsigned long read_byte=PACKET_SIZE;

    if(RegisterCard(0xA116FD719D83,ALL_CHANNELS))
    {
        QMessageBox msgBox;
        msgBox.setText("Нет соединения с ПИВОЛС");
        msgBox.exec();
    }
    forever{
        if (stopped==true)
            break;
        if (buffer_flag==false)
        {
            lock->lockForWrite();
            count_read_byte = ReadCard(buffer1, read_byte);
            if(count_read_byte!=read_byte)
            {
                buffer1+=count_read_byte;
                read_byte-=count_read_byte;
            }
            else
            {
                lock->unlock();
                buffer_flag=true;
                count_read_byte=0;
                read_byte=PACKET_SIZE;
            }
        }
        else
        {
            lock->lockForWrite();
            count_read_byte = ReadCard(buffer2, read_byte);
            if(count_read_byte!=read_byte)
            {
                buffer1+=count_read_byte;
                read_byte-=count_read_byte;
            }
            else
            {
                lock->unlock();
                buffer_flag=false;
                count_read_byte=0;
                read_byte=PACKET_SIZE;
            }
        }
    }
    ReleaseCard();
    lock->unlock();

}
