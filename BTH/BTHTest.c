//#include "NetworkHeader.h"
//#include <ws2bth.h>
#define WIN32_LEAN_AND_MEAN

#include <windows.h>
#include <BluetoothAPIs.h>

#include <stdio.h>

void DieWithError(char *errorMessage);

// -----
//   BTHTest.c
//		This program is a test server and client 
//			using UDP datagram sockets
int
main(int argc, char **argv)
{
	HANDLE Radio;
	BLUETOOTH_RADIO_INFO RadioInfo;
	RadioInfo.dwSize = sizeof(BLUETOOTH_RADIO_INFO);

	BLUETOOTH_FIND_RADIO_PARAMS RadioParams;
	RadioParams.dwSize = sizeof(RadioParams);


	HBLUETOOTH_RADIO_FIND RadioFind;
	if((RadioFind = BluetoothFindFirstRadio(&RadioParams, &Radio)) == NULL)
		DieWithError("BluetoothFindFirstRadio() failed");

	// Success
	if(BluetoothGetRadioInfo(Radio, &RadioInfo) != ERROR_SUCCESS)
		DieWithError("BluetoothGetRadioInfo() failed");

	BluetoothFindRadioClose(RadioFind);
	CloseHandle(Radio);


	
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

