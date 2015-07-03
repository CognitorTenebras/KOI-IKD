#ifndef pivolsH
#define pivolsH

#include <windows.h>
#include <setupapi.h>
#include <winioctl.h>

#define IOCTL_RECIEVE_BUF	CTL_CODE (FILE_DEVICE_BUS_EXTENDER, 0x0, METHOD_OUT_DIRECT, FILE_ANY_ACCESS)
#define IOCTL_CONFIGURE		CTL_CODE (FILE_DEVICE_BUS_EXTENDER, 0x0, METHOD_BUFFERED, FILE_ANY_ACCESS)

#define CHANNEL_A 0
#define CHANNEL_B 1
#define BOTH_CHANNEL_AND_MARKER 2

static const GUID GUID_DEVINTERFACE_RECVI = {0x03EF0C78, 0xEF1F, 0x4B61, {0xB0, 0x90, 0x0B, 0x01, 0x54, 0x6F, 0xB7, 0x7C}};
//03ef0c78-ef1f-4b61-b090-0b01546fb77c

BOOL RegisterCard(unsigned int marker, unsigned char channel);
void ReleaseCard();
void Register_Buffer(void *buffer_a, void *buffer_b, unsigned int size_buf);
unsigned long ReadBufCard(unsigned char *index_buffer);
unsigned long ReadSgBufCard(void *buf, unsigned long size_buf);

typedef struct _SPARAM_CONFIGURE
{
   unsigned int marker;
   unsigned char channel;
} SPARAM_CONFIGURE;


#endif
