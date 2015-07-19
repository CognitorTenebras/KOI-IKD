
#include "resources.h"

QMutex lock1, lock2;

unsigned short encode(unsigned short a)
{
    return ((a&0xFF)<<8|(a&0xFF00)>>8);
}
