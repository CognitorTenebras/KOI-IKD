#ifndef pivolsH
#define pivolsH

#include <windows.h>
#include <setupapi.h>
#include <winioctl.h>

#define MAX_CARDS 4

#define IOCTL_RECIEVE_BUF	CTL_CODE (FILE_DEVICE_CONTROLLER, 0x800, METHOD_OUT_DIRECT, FILE_ANY_ACCESS)
#define IOCTL_CONFIGURE		CTL_CODE (FILE_DEVICE_CONTROLLER, 0x800, METHOD_BUFFERED,   FILE_ANY_ACCESS)
#define IOCTL_STATUS_REQUEST	CTL_CODE (FILE_DEVICE_CONTROLLER, 0x881, METHOD_BUFFERED,   FILE_READ_DATA | FILE_WRITE_DATA)

#define ALL_CHANNELS 6
#define TEST_CHANNEL 7
#define STAT_FLAG_DATA_LOST_IN_CARD 0x8
#define STAT_FLAG_DATA_LOST 0x4
#define STAT_FLAG_HALF_FILLED_BUF 0x1
#define STAT_FLAG_ALMOST_FULL_BUF 0x2   //непрочитано 7 из 8 МБ

// {9F0359C2-53A3-42D5-B2FC-A6D4660DECFC}
static const GUID GUID_DEVINTERFACE_RECVI =
{ 0x9f0359c2, 0x53a3, 0x42d5, { 0xb2, 0xfc, 0xa6, 0xd4, 0x66, 0xd, 0xec, 0xfc } };


BOOL RegisterCard(unsigned long marker, unsigned char channel, unsigned long timeout = 1000, unsigned long speed = 1, int card = 0);
void ReleaseCard(int card = 0);
unsigned long ReadCard(void *buffer, unsigned int size, int card = 0);
BOOL CheckCardState(unsigned int *availableData, bool *flagDataLost = NULL, bool *flagHalfFilledBuf = NULL, bool *flagAlmostFilledBuf = NULL, int card = 0);
int GetCardsCount();

typedef struct _SPARAM_CONFIGURE
{
   unsigned long marker;
   unsigned long timeout;
   unsigned long speed;
   unsigned char channel;
} SPARAM_CONFIGURE;

#endif
