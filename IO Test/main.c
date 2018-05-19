#include <stdio.h>
#include <Windows.h>


int
main(int argc, char **argv)
{
	//printf("Trying macro: %d\n", GET_POINTERID_WPARAM(0));
	int res = EnableMouseInPointer(TRUE);

	printf("EnableMouseInPointer() %s\n", IsMouseInPointerEnabled() ? "succeeded" : "failed");

	//printf("Trying macro: %d\n", GET_POINTERID_WPARAM(0));
	
	Sleep(1000);

	POINTER_DEVICE_INFO PointerDevices = { 0 };

	UINT32 a;

	if(GetPointerDevices(&a, NULL)) //NULL, &PointerDevices))
	{
		printf("Succeeded! a = %d\n", a);
	}
	else
	{
		printf("Failed!\n");

		DWORD err = GetLastError();

		char *buf;

		DWORD result = FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM
									|FORMAT_MESSAGE_ALLOCATE_BUFFER
									|FORMAT_MESSAGE_IGNORE_INSERTS,
									NULL,
									err,
									MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
									(LPTSTR) &buf,
									0,
									NULL);

		printf("Error: %s\n", buf);

		LocalFree(buf);

	}


	return(0);
}