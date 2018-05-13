#include <windows.h>
//#include <NetworkHeader.h>
//void DieWithError(char *errorMessage);

int
main()
{
	// Try querying for number of input devices

	UINT32 deviceCount;

	if(GetPointerDevices(&deviceCount, 0) == 0)
		printf("GetPointerDevices() failed");
	else
		printf("deviceCount = %d\n", deviceCount);

	return(0);
}