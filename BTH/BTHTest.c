//#include "NetworkHeader.h"
//#include <ws2bth.h>
#define WIN32_LEAN_AND_MEAN

#include <windows.h>
#include <SetupAPI.h>
//#include <bluetoothapis.h>
#include <bthledef.h>

#include <stdio.h>

void DieWithError(char *errorMessage);

// -----
//   BTHTest.c
//		This program is a test server and client 
//			using UDP datagram sockets
int
main(int argc, char **argv)
{
	HDEVINFO hDevInfo;
	hDevInfo = SetupDiGetClassDevs(
		&GUID_BLUETOOTHLE_DEVICE_INTERFACE, 
		NULL, NULL, DIGCF_PRESENT);

	if(hDevInfo == INVALID_HANDLE_VALUE)
		DieWithError("SetupDiGetClassDevs() failed");

	SP_DEVINFO_DATA DeviceInfoData;
	ZeroMemory(&DeviceInfoData, sizeof(DeviceInfoData));
	DeviceInfoData.cbSize = sizeof(DeviceInfoData);

	for(DWORD MemberIndex = 0; 
		SetupDiEnumDeviceInfo(
			hDevInfo, 
			MemberIndex,
			&DeviceInfoData); 
		++MemberIndex)
	{
		//DeviceInfoData.
	}

	SetupDiDestroyDeviceInfoList(hDevInfo);
	DieWithError("Ended");
	
	return(0);
}

void DieWithError(char *errorMessage)
{
	char *errorText = NULL;
	int err = GetLastError();
	FormatMessage(
		FORMAT_MESSAGE_FROM_SYSTEM |
		FORMAT_MESSAGE_ALLOCATE_BUFFER |
		FORMAT_MESSAGE_IGNORE_INSERTS, 
		NULL,
		err,
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		(LPTSTR) &errorText,
		0,
		NULL);

	printf("%s: %s", errorMessage, errorText);
	LocalFree(errorText);
	errorText = NULL;
	exit(1);
}

