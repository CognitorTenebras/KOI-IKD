//#pragma once

//#include "stdafx.h"
#include "pivols.h"

HANDLE hndl_card = NULL;

void *dbuf[2];
unsigned char ind_buf = 0;
unsigned int size_buffer = 0;

OVERLAPPED pvls_Ovlp;

bool p_first_read = true;

BOOL RegisterCard(unsigned int marker, unsigned char channel)
{

	SP_DEVINFO_DATA devInfoData;
	SP_DEVICE_INTERFACE_DATA  devInterfaceData;
	PSP_INTERFACE_DEVICE_DETAIL_DATA functionClassDeviceData;
   SPARAM_CONFIGURE param_configure;
   DWORD dwReturnBytes;

   ULONG requiredLength = 0;
   int deviceNumber = 0; // Can be other values if more than 1 device connected to driver

   HDEVINFO hwDeviceInfo = SetupDiGetClassDevs (  (LPGUID) &GUID_DEVINTERFACE_RECVI,
                                            NULL,
											NULL,
											//DIGCF_ALLCLASSES);
											DIGCF_PRESENT|DIGCF_INTERFACEDEVICE);

   if (hwDeviceInfo != INVALID_HANDLE_VALUE)
   {

	  devInterfaceData.cbSize = sizeof(devInterfaceData);

	  if (SetupDiEnumDeviceInterfaces ( hwDeviceInfo, 0, (LPGUID) &GUID_DEVINTERFACE_RECVI, deviceNumber, &devInterfaceData))
	  {
		 SetupDiGetInterfaceDeviceDetail ( hwDeviceInfo, &devInterfaceData, NULL, 0, &requiredLength, NULL);

		 ULONG predictedLength = requiredLength;

		 functionClassDeviceData = (PSP_INTERFACE_DEVICE_DETAIL_DATA) malloc(predictedLength);

		 functionClassDeviceData->cbSize = sizeof (SP_INTERFACE_DEVICE_DETAIL_DATA);

		 devInfoData.cbSize = sizeof(devInfoData);

		 if(SetupDiGetInterfaceDeviceDetail(hwDeviceInfo, &devInterfaceData, functionClassDeviceData, predictedLength, &requiredLength, &devInfoData))
		 {

			hndl_card = CreateFile(functionClassDeviceData->DevicePath,
								   GENERIC_WRITE | GENERIC_READ,
								   FILE_SHARE_WRITE | FILE_SHARE_READ,
								   NULL,
								   OPEN_EXISTING,
								   FILE_FLAG_OVERLAPPED,
								   NULL);


			param_configure.marker = marker;
			param_configure.channel = channel;

			DeviceIoControl(hndl_card, IOCTL_CONFIGURE, &param_configure, sizeof(SPARAM_CONFIGURE), NULL, 0, &dwReturnBytes, NULL);

			free(functionClassDeviceData);
			SetupDiDestroyDeviceInfoList(hwDeviceInfo);

			return 1;
		 }
	  }
   }

   return 0;
}

void ReleaseCard()
{
   CloseHandle(pvls_Ovlp.hEvent);
   CloseHandle(hndl_card);
}

void Register_Buffer(void *buffer_a, void *buffer_b, unsigned int size_buf)
{
   dbuf[0] = buffer_a;
   dbuf[1] = buffer_b;
   size_buffer = size_buf;

   HANDLE hEvent;
   hEvent = CreateEvent(NULL, FALSE, FALSE, NULL);


   ZeroMemory(&pvls_Ovlp, sizeof(OVERLAPPED));
   pvls_Ovlp.hEvent = hEvent;

   ind_buf = 0;
   p_first_read = true;
}

unsigned long ReadBufCard(unsigned char *index_buffer)
{
   DWORD dwReturnBytes = 0;

   if(p_first_read)
   {
	  DeviceIoControl(hndl_card, IOCTL_RECIEVE_BUF, NULL, 0, dbuf[ind_buf], size_buffer, NULL, &pvls_Ovlp);
	  p_first_read = false;
   }
//   WaitForSingleObject(hEvent, 2000);

   GetOverlappedResult(hndl_card, &pvls_Ovlp, &dwReturnBytes, TRUE);

   *index_buffer = ind_buf;
   ind_buf = (ind_buf == 0)? 1: 0;

   DeviceIoControl(hndl_card, IOCTL_RECIEVE_BUF, NULL, 0, dbuf[ind_buf], size_buffer, NULL, &pvls_Ovlp);

   return dwReturnBytes;
}

unsigned long ReadSgBufCard(void *buf, unsigned long size_buf)
{
   unsigned long dwReturnBytes = 0;

   DeviceIoControl(hndl_card, IOCTL_RECIEVE_BUF, NULL, 0, buf, size_buf, &dwReturnBytes, NULL);

   return dwReturnBytes;
}
