#include <windows.h>

#define WIN32_LEAN_AND_MEAN

void printerr(DWORD err);
LRESULT CALLBACK WindowProc(HWND Window, UINT Message, WPARAM WParam, LPARAM LParam);


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
				WS_OVERLAPPEDWINDOW,
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

			BOOL bRes = RegisterPointerDeviceNotifications(WindowHandle, TRUE);

			if(!bRes)
				printerr(GetLastError());

			MSG Message;
			for(;;)
			{
				BOOL MessageResult = GetMessage(&Message, 0, 0, 0);
				if(MessageResult > 0)
				{
					printf("Got stuff!\n");
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
			//GetPointer
			printf("Pointer input!\n");
			Result = DefWindowProc(Window, Message, WParam, LParam);
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