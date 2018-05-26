#define WIN32_LEAN_AND_MEAN

#include <windows.h>
#include <stdlib.h>
#include <stdio.h>

void printerr(DWORD err);
LRESULT CALLBACK WindowProc(HWND Window, UINT Message, WPARAM WParam, LPARAM LParam);
DWORD __stdcall ThreadProc(LPVOID lpParameter);
HANDLE ThreadHandle;
UINT32 PointerId;


int
main(int argc, char **argv)
{
	WNDCLASS WindowClass = {0};

	WindowClass.style = CS_OWNDC;
	WindowClass.lpfnWndProc = WindowProc;
	WindowClass.hInstance = GetModuleHandle(0);
	WindowClass.lpszClassName = "WindowClass";

	if(RegisterClass(&WindowClass))
	{
		HWND WindowHandle = 
			CreateWindowEx(
				WS_EX_TRANSPARENT,
				WindowClass.lpszClassName,
				"Dummy",
				WS_OVERLAPPEDWINDOW | WS_VISIBLE,
				CW_USEDEFAULT,
				CW_USEDEFAULT,
				CW_USEDEFAULT,
				CW_USEDEFAULT,
				0, //HWND_MESSAGE,
				0,
				GetModuleHandle(0),
				0);
		
		if(WindowHandle)
		{
			printf("Success!\n");

			#if 0
			{
				printf("Testing RI devices\n\n");

				// Lets try GetPointerDevice() using the RAWINPUTDEVICE list results
				int NumRIDevices;
				GetRawInputDeviceList(NULL, &NumRIDevices, sizeof(RAWINPUTDEVICELIST));

				RAWINPUTDEVICELIST *RIDeviceList = malloc(sizeof(RAWINPUTDEVICELIST) * NumRIDevices);
				GetRawInputDeviceList(RIDeviceList, &NumRIDevices, sizeof(RAWINPUTDEVICELIST));

				for(int i = 0; i < NumRIDevices; ++i)
				{
					printf("Device %u:\n", i);
					POINTER_DEVICE_INFO PDI;
					
					if(!GetPointerDevice(RIDeviceList->hDevice, &PDI))
						printerr(GetLastError());
					else
						printf("Device %u GetPointerDevice() worked!\n", i);
				}
			}
			#endif

			BOOL bRes = RegisterPointerDeviceNotifications(WindowHandle, TRUE);
			// WM_POINTERDEVICEINRANGE
			// WM_POINTERDEVICEOUTOFRANGE

			if(!bRes)
				printerr(GetLastError());

			// Create a thread to check hte pens state while it is in frame.
			//ThreadHandle = CreateThread(0, 0, ThreadProc, NULL, CREATE_SUSPENDED, 0);

/*
			if(!ThreadHandle)
			{
				printerr(GetLastError());
				exit(1);
			}
*/
		
			MSG Message;
			for(;;)
			{
				BOOL MessageResult = GetMessage(&Message, 0, 0, 0);
				if(MessageResult > 0)
				{
					//printf("\r%5x", Message.message);
					//fflush(stdout);
					UINT msgnum = Message.message;

					TranslateMessage(&Message);
					DispatchMessage(&Message);
				}
				else
				{
					break;
				}
			}
		}
		else
		{
			printf("Failed!\n");

			printerr(GetLastError());
		}
	}
	return(0);
}

LRESULT CALLBACK
WindowProc(HWND Window, UINT Message, WPARAM WParam, LPARAM LParam)
{
	LRESULT Result = 0;

	switch(Message)
	{
		case WM_POINTERUPDATE:
		{
			printf("Pointer input! %u\n", GET_POINTERID_WPARAM(WParam));
			Result = DefWindowProc(Window, Message, WParam, LParam);
		} break;

		case WM_POINTERDEVICEINRANGE:
		{
			printf("Ptr in range! %u\n", IS_POINTER_INRANGE_WPARAM(WParam));
			//InterlockedExchange(&PointerId, GET_POINTERID_WPARAM(WParam));
			UINT32 pi = GET_POINTERID_WPARAM(WParam);
			//ResumeThread(ThreadHandle);

			POINTER_INFO Info = {0};
			if(GetPointerInfo(pi, &Info))
			{
				UINT32 pressure = Info.pointerId;
				printf("l: %u\n", pressure);
				//TouchInfo.rcContactRaw;
			}
			else
			{
				printf("LINE %u\n", __LINE__);
				printerr(GetLastError());
			}
		} break;

		case WM_POINTERDEVICEOUTOFRANGE:
		{
			printf("Ptr out of range!: %u\n", IS_POINTER_INRANGE_WPARAM(WParam));
			//SuspendThread(ThreadHandle);
		} break;

		default:
		{
			Result = DefWindowProc(Window, Message, WParam, LParam);
		} break;
	}

	return Result;
}

void
printerr(DWORD err)
{
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

DWORD __stdcall
ThreadProc(LPVOID lpParameter)
{
	for(;;)
	{
		printf("Thread started!\n");
		Sleep(500);
	}
}