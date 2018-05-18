#include <windows.h>
#include <stdio.h>

void printerr(DWORD err);

LRESULT CALLBACK
WindowProc(HWND Window, UINT Message, WPARAM WParam, LPARAM LParam);

int
main()
{
	WNDCLASS WindowClass = {0};

	WindowClass.style = CS_OWNDC;
	WindowClass.lpfnWndProc = WindowProc;
	WindowClass.hInstance = GetModuleHandle(0);
	WindowClass.lpszClassName = "WindowClass";

	if(RegisterClass(&WindowClass))
	{
		printf("Successful registering window class!\n");

		HWND WindowHandle = CreateWindowEx(WS_EX_TRANSPARENT,
											WindowClass.lpszClassName,
											"Dummy",
											WS_OVERLAPPEDWINDOW,// | WS_VISIBLE,
											CW_USEDEFAULT,
											CW_USEDEFAULT,
											CW_USEDEFAULT,
											CW_USEDEFAULT,
											0,
											0,
											GetModuleHandle(0),
											0);

		if(WindowHandle)
		{
			printf("Window handle valid!\n");

			//Try to get mouse RI
			int NumDevices;
			GetRawInputDeviceList(NULL, &NumDevices, sizeof(RAWINPUTDEVICELIST));

			RAWINPUTDEVICELIST *RIDeviceList = malloc(NumDevices * sizeof(RAWINPUTDEVICELIST));
			GetRawInputDeviceList(RIDeviceList, &NumDevices, sizeof(RAWINPUTDEVICELIST));

			// Count number of decives [specifcally mice] to add
			int NumMice = 0;
			for(int i = 0; i < NumDevices; ++i)
				if(RIDeviceList[i].dwType == RIM_TYPEMOUSE)
					NumMice++;

			RAWINPUTDEVICE RID[2] = {0};

			HWND Console = GetConsoleWindow();

			// Usage page & usage of Pointer
			RID[0].usUsagePage = 0x1;
			RID[0].usUsage = 0x1;
			RID[0].dwFlags = RIDEV_INPUTSINK; //RIDEV_INPUTSINK;
			RID[0].hwndTarget = WindowHandle;

			// Mouse
			RID[1].usUsagePage = 0x1;
			RID[1].usUsage = 0x2;
			RID[1].dwFlags = RIDEV_INPUTSINK; // hwnd MUST be specified
			RID[1].hwndTarget = WindowHandle;

			BOOL Result;
			Result = RegisterRawInputDevices(RID, 2, sizeof(RAWINPUTDEVICE));

			if(!Result)
			{
				printf("Registering the RIDs failed!\n");
				DWORD err = GetLastError();

				printerr(err);
			}

			MSG Message;
			for(;;)
			{
				BOOL MessageResult = GetMessage(&Message, 0, 0, 0);
				if(MessageResult > 0)
				{
					
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
			printf("No window!");
		}
	}
	else
	{
		printf("Failed to register window!\n");
	}				
}


LRESULT CALLBACK
WindowProc(	HWND Window, 
			UINT Message, 
			WPARAM WParam, 
			LPARAM LParam)
{
	LRESULT Result = 0;

	switch(Message)
	{
		case WM_INPUT:
		{
			//MINMAXINFO Info = LParam;
			printf(".");
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