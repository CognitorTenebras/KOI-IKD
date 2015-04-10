#include "stdafx.h"
#include "pivols.h"

HANDLE hndl_card[MAX_CARDS] = {NULL};
void *dbuf[MAX_CARDS][2];
unsigned char ind_buf[MAX_CARDS] = {0};
unsigned int size_buffer[MAX_CARDS] = {0};
int cards_count = 0;
OVERLAPPED pvls_Ovlp[MAX_CARDS];
bool p_first_read[MAX_CARDS] = {true};

BOOL RegisterCard(unsigned long marker, unsigned char channel, unsigned long timeout, unsigned long speed, int card)
{
   SP_DEVINFO_DATA devInfoData;
   SP_DEVICE_INTERFACE_DATA  devInterfaceData;
   PSP_INTERFACE_DEVICE_DETAIL_DATA functionClassDeviceData;
   SPARAM_CONFIGURE param_configure;
   unsigned long dwReturnBytes;

   ULONG requiredLength = 0;

   HDEVINFO hwDeviceInfo = SetupDiGetClassDevs (  (LPGUID) &GUID_DEVINTERFACE_RECVI,
                                            NULL,
											NULL,
											//DIGCF_ALLCLASSES);
											DIGCF_PRESENT|DIGCF_INTERFACEDEVICE);

   if (hwDeviceInfo != INVALID_HANDLE_VALUE)
   {

	  devInterfaceData.cbSize = sizeof(devInterfaceData);

      if (SetupDiEnumDeviceInterfaces ( hwDeviceInfo, 0, (LPGUID) &GUID_DEVINTERFACE_RECVI, card, &devInterfaceData))
	  {
		 SetupDiGetInterfaceDeviceDetail ( hwDeviceInfo, &devInterfaceData, NULL, 0, &requiredLength, NULL);

		 ULONG predictedLength = requiredLength;

		 functionClassDeviceData = (PSP_INTERFACE_DEVICE_DETAIL_DATA) malloc(predictedLength);

		 functionClassDeviceData->cbSize = sizeof (SP_INTERFACE_DEVICE_DETAIL_DATA);

		 devInfoData.cbSize = sizeof(devInfoData);

		 if(SetupDiGetInterfaceDeviceDetail(hwDeviceInfo, &devInterfaceData, functionClassDeviceData, predictedLength, &requiredLength, &devInfoData))
		 {
            hndl_card[card] = CreateFile(functionClassDeviceData->DevicePath,
								   GENERIC_WRITE | GENERIC_READ,
                                   0,
								   NULL,
								   OPEN_EXISTING,
								   FILE_FLAG_OVERLAPPED,
								   NULL);


            if(hndl_card[card]==INVALID_HANDLE_VALUE)
                return 0;

			param_configure.marker = marker;
            param_configure.timeout = timeout;
			param_configure.channel = channel;
            param_configure.speed = speed;

            DeviceIoControl(hndl_card[card], IOCTL_CONFIGURE, &param_configure, sizeof(SPARAM_CONFIGURE), NULL, 0, &dwReturnBytes, NULL);

			free(functionClassDeviceData);
			SetupDiDestroyDeviceInfoList(hwDeviceInfo);

            HANDLE hEvent;
            hEvent = CreateEvent(NULL, TRUE, FALSE, NULL);

            ZeroMemory(&pvls_Ovlp[card], sizeof(OVERLAPPED));
            pvls_Ovlp[card].hEvent = hEvent;

			return 1;
		 }
	  }
   }

   return 0;
}

void ReleaseCard(int card)
{
   CloseHandle(pvls_Ovlp[card].hEvent);
   CloseHandle(hndl_card[card]);
}

void Register_Buffer(void *buffer_a, void *buffer_b, unsigned int size_buf, int card)
{
   dbuf[card][0] = buffer_a;
   dbuf[card][1] = buffer_b;
   size_buffer[card] = size_buf;

   ind_buf[card] = 0;
   p_first_read[card] = true;
}

unsigned long ReadBufCard(unsigned char *index_buffer, int card, int time_out_milliseconds)
{
   unsigned long dwReturnBytes = 0;
   if(hndl_card[card]==NULL)
       return 0;
   if(dbuf[card][0]==NULL)
       return 0;

   if(p_first_read[card])
   {
      DeviceIoControl(hndl_card[card], IOCTL_RECIEVE_BUF, NULL, 0, dbuf[card][ind_buf[card]], size_buffer[card], NULL, &pvls_Ovlp[card]);
      p_first_read[card] = false;
   }
   if(GetOverlappedResult(hndl_card[card], &pvls_Ovlp[card], &dwReturnBytes, FALSE)==FALSE)
   {
       if(GetLastError()==ERROR_IO_INCOMPLETE)
       {
           switch(WaitForSingleObject(pvls_Ovlp[card].hEvent, time_out_milliseconds))
           {
           case WAIT_OBJECT_0:
               GetOverlappedResult(hndl_card[card], &pvls_Ovlp[card], &dwReturnBytes, FALSE);
               ResetEvent(pvls_Ovlp[card].hEvent);
               break;
           case WAIT_TIMEOUT:
               ResetEvent(pvls_Ovlp[card].hEvent);
               CancelIo(hndl_card[card]);
               *index_buffer = ind_buf[card];
               DeviceIoControl(hndl_card[card], IOCTL_RECIEVE_BUF, NULL, 0, dbuf[card][ind_buf[card]], size_buffer[card], NULL, &pvls_Ovlp[card]);
               return 0;
           default:
               ResetEvent(pvls_Ovlp[card].hEvent);
               *index_buffer = ind_buf[card];
               DeviceIoControl(hndl_card[card], IOCTL_RECIEVE_BUF, NULL, 0, dbuf[card][ind_buf[card]], size_buffer[card], NULL, &pvls_Ovlp[card]);
               return 0;//WAIT_ABANDONED, WAIT_FAILED
           }
       } else
       {
           return 0;
       }
   }

   *index_buffer = ind_buf[card];
   ind_buf[card] ^= 1;

   DeviceIoControl(hndl_card[card], IOCTL_RECIEVE_BUF, NULL, 0, dbuf[card][ind_buf[card]], size_buffer[card], NULL, &pvls_Ovlp[card]);

   return dwReturnBytes;
}

unsigned long ReadSgBufCard(void *buf, unsigned long size_buf, int card)
{
   unsigned long dwReturnBytes = 0;

   DeviceIoControl(hndl_card[card], IOCTL_RECIEVE_BUF, NULL, 0, buf, size_buf, &dwReturnBytes, NULL);

   return dwReturnBytes;
}

unsigned long ReadCard(void *buffer, unsigned int size, int card)
{
    unsigned long dwReturnBytes = 0;
    if(hndl_card[card]==NULL)
    {
        return 0;
    }

    DeviceIoControl(hndl_card[card], IOCTL_RECIEVE_BUF, NULL, 0, buffer, size, NULL, &pvls_Ovlp[card]);
    if(GetOverlappedResult(hndl_card[card], &pvls_Ovlp[card], &dwReturnBytes, FALSE)==FALSE)
    {
        if(GetLastError()==ERROR_IO_INCOMPLETE)
        {
            switch(WaitForSingleObject(pvls_Ovlp[card].hEvent, INFINITE))
            {
            case WAIT_OBJECT_0:
                GetOverlappedResult(hndl_card[card], &pvls_Ovlp[card], &dwReturnBytes, FALSE);
                ResetEvent(pvls_Ovlp[card].hEvent);
                return dwReturnBytes;
            case WAIT_TIMEOUT:
                ResetEvent(pvls_Ovlp[card].hEvent);
                CancelIo(hndl_card[card]);
                return 0;
            default:
                ResetEvent(pvls_Ovlp[card].hEvent);
                return 0;//WAIT_ABANDONED, WAIT_FAILED
            }
        } else
        {
            return 0;
        }
    } else
    {
        return dwReturnBytes;
    }
}

int GetCardsCount()
{
    HDEVINFO hdev = SetupDiGetClassDevs(&GUID_DEVINTERFACE_RECVI, 0, 0, DIGCF_PRESENT | DIGCF_DEVICEINTERFACE);
    if(hdev == INVALID_HANDLE_VALUE)
    {
        return 0;
    }
    SP_DEVICE_INTERFACE_DATA did = {0};
    did.cbSize = sizeof(did);
    for(cards_count = 0; SetupDiEnumDeviceInterfaces(hdev, 0, &GUID_DEVINTERFACE_RECVI, cards_count, &did); cards_count++);
    SetupDiDestroyDeviceInfoList(hdev);
    return cards_count;
}

BOOL CheckCardState(unsigned int *availableData, bool *flagDataLost, bool *flagHalfFilledBuf, bool *flagAlmostFilledBuf, int card)
{
    if(hndl_card[card]==NULL)
        return 0;
    if(((int)availableData|(int)flagDataLost|(int)flagHalfFilledBuf|(int)flagAlmostFilledBuf)==NULL)
        return 0;

    unsigned long result;
    unsigned long retSize;
    unsigned char flags;
    DeviceIoControl(hndl_card[card], IOCTL_STATUS_REQUEST, NULL, 0, &result, 4, NULL, &pvls_Ovlp[card]);

    if(GetOverlappedResult(hndl_card[card], &pvls_Ovlp[card], &retSize, FALSE)==TRUE)
    {
        if(retSize!=4)
            return 0;
        flags = result>>24;
        if(availableData)
            *availableData = result & 0xFFFFFF;
        if(flagDataLost)
            *flagDataLost = (flags&(STAT_FLAG_DATA_LOST|STAT_FLAG_DATA_LOST_IN_CARD))!=0;
        if(flagHalfFilledBuf)
            *flagHalfFilledBuf = (flags&STAT_FLAG_HALF_FILLED_BUF)!=0;
        if(flagAlmostFilledBuf)
            *flagAlmostFilledBuf = (flags&STAT_FLAG_ALMOST_FULL_BUF)!=0;
        return 1;
    } else if(GetLastError()==ERROR_IO_INCOMPLETE)
    {
        switch(WaitForSingleObject(pvls_Ovlp[card].hEvent, INFINITE))
        {
        case WAIT_OBJECT_0:
            GetOverlappedResult(hndl_card[card], &pvls_Ovlp[card], &retSize, FALSE);
            ResetEvent(pvls_Ovlp[card].hEvent);
            flags = result>>24;
            if(availableData)
                *availableData = result & 0xFFFFFF;
            if(flagDataLost)
                *flagDataLost = (flags&STAT_FLAG_DATA_LOST)!=0;
            if(flagHalfFilledBuf)
                *flagHalfFilledBuf = (flags&STAT_FLAG_HALF_FILLED_BUF)!=0;
            if(flagAlmostFilledBuf)
                *flagAlmostFilledBuf = (flags&STAT_FLAG_ALMOST_FULL_BUF)!=0;
            return 1;
        case WAIT_TIMEOUT:
            ResetEvent(pvls_Ovlp[card].hEvent);
            CancelIo(hndl_card[card]);
            return 0;
        default:
            ResetEvent(pvls_Ovlp[card].hEvent);
            return 0;//WAIT_ABANDONED, WAIT_FAILED
        }
    }
    return 0;
}
