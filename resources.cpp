#include "resources.h"

unsigned short encode(unsigned short a)
{
    return (a<<8|a>>8);
}
