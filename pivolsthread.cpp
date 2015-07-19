#include "Pivolsthread.h"
#include "resources.h"

#include <QMessageBox>

Pivolsthread::Pivolsthread(QObject *parent):
    QThread(parent)
{
   buffer_flag=false;
   stop=false;

   buffer1= new unsigned char [BUF_SIZE];
   memset(buffer1,0x0,BUF_SIZE);
   buffer2= new unsigned char [BUF_SIZE];
   memset(buffer2,0x0,BUF_SIZE);
}

Pivolsthread::~Pivolsthread()
{
    delete []buffer1;
    delete []buffer2;
}

void Pivolsthread::run()
{
    unsigned long count_read_byte=0;
    unsigned long read_byte=BUF_SIZE;
    unsigned short shift=0;
    lock2.lock();
    forever{
        if (stop) break;
        if (buffer_flag==false)
        {
            int countErr=0;
            while(read_byte!=0)
            {
                count_read_byte = ReadSgBufCard(buffer1+shift, read_byte);//заполнение буфера из пиволса
                shift = count_read_byte;
                read_byte-=count_read_byte;
                if(countErr>3)
                {
                    QMessageBox::warning(0,"Ошибка Пиволс", "Ошибка получения данных с платы ПИВОЛС",
                                         QMessageBox::Cancel,QMessageBox::Ok);
                    emit stopWork();
                    break;
                }
                countErr++;
            }
            lock1.lock();//ждем пока можно будет отправить
            emit sendResult(buffer1,buffer_flag);
            buffer_flag=true;
            count_read_byte=0;
            shift=0;
            read_byte=BUF_SIZE;
        }
        else
        {

            int countErr=0;
            while(read_byte!=0)
            {
                count_read_byte = ReadSgBufCard(buffer2+shift, read_byte);
                shift = count_read_byte;
                read_byte-=count_read_byte;
                if(countErr>3)
                {
                    QMessageBox::warning(0,"Ошибка Пиволс", "Ошибка получения данных с платы ПИВОЛС",
                                         QMessageBox::Cancel,QMessageBox::Ok);
                    emit stopWork();
                    break;
                }
                countErr++;
            }
            lock2.lock();
            emit sendResult(buffer2,buffer_flag);
            buffer_flag=false;
            count_read_byte=0;
            shift=0;
            read_byte=BUF_SIZE;
        }
    }
    if(lock1.tryLock())//если закрыто то открыть, но тут что то не так))
        lock1.unlock();
    if(lock2.tryLock())
        lock2.unlock();
    emit finished();
}

void Pivolsthread::stopped()
{
    stop=true;
}
