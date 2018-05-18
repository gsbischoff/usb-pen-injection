#include <windows.h>
#include <stdio.h>
//#include <NetworkHeader.h>

LRESULT CALLBACK
WindowProc(	HWND Window, 
			UINT Message, 
			WPARAM WParam, 
			LPARAM LParam)
{
	LRESULT Result = 0;

	printf("0x%04x\t W: %llx\tL:%llx\n", Message, WParam, LParam);

	switch(Message)
	{
		case WM_GETMINMAXINFO:
		{
			//MINMAXINFO Info = LParam;

			Result = DefWindowProc(Window, Message, WParam, LParam);
		} break;

		case WM_NCCREATE:
		{
			//MINMAXINFO *Info = LParam;

			Result = DefWindowProc(Window, Message, WParam, LParam);
		} break;

		case WM_NCCALCSIZE:
		{
			RECT *Rect = (RECT *) LParam;
			printf("Rect: l: %d, r: %d, t: %d, b: %d\n", Rect->left, Rect->right, Rect->top, Rect->bottom);
			Result = DefWindowProc(Window, Message, WParam, LParam);
		} break;

		case WM_CREATE:
		{
			CREATESTRUCT *Info = (CREATESTRUCT *) LParam;
			Result = DefWindowProc(Window, Message, WParam, LParam);
		} break;

		case WM_DWMNCRENDERINGCHANGED:
		{
			//MINMAXINFO Info = WParam;
			Result = DefWindowProc(Window, Message, WParam, LParam);
		} break;

		default:
		{
			Result = DefWindowProc(Window, Message, WParam, LParam);
		} break;
	}

	return Result;
}

int
main()
{
	// Try querying for number of input devices

	
	CURSORINFO CursorInfo;
	CursorInfo.cbSize = sizeof(CURSORINFO);

#if 0
	GetCursorInfo(&CursorInfo);

	POINT Point;
	POINT Last = {0};
	while(GetCursorPos(&Point))
	{
		if(Last.x != Point.x || Last.y != Point.y)
		{
			putc('\r', stdout);
			fflush(stdout);
			printf("x: %d\ty: %d\t", Point.x, Point.y);
			//fflush(stdout);
		}

		Sleep(10);

		Last = Point;
	}
#elif 0
	UINT32 deviceCount;

	if(GetPointerDevices(&deviceCount, 0) == 0)
		printf("GetPointerDevices() failed");
	else
		printf("deviceCount = %d\n", deviceCount);

	//GetModuleHandle();
	// Make a window!
	WNDCLASS WindowClass = {0};

	WindowClass.style = CS_OWNDC;
	WindowClass.lpfnWndProc = WindowProc;
	WindowClass.hInstance = GetModuleHandle(0);
	WindowClass.lpszClassName = "WindowClass";

	if(RegisterClass(&WindowClass))
	{
		printf("Successful registering window!\n");

		HWND WindowHandle = CreateWindowEx(WS_EX_TRANSPARENT,
											WindowClass.lpszClassName,
											"Doot",
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
			if(ShowWindow(WindowHandle, SW_HIDE))
				printf("Window Hidden!\n");
			else
				printf("Window... not hidden\n");

			MSG Message;
			for(;;)
			{
				BOOL MessageResult = GetMessage(&Message, 0, 0, 0);
				if(MessageResult > 0)
				{
					printf(".");
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
#else
	// Query number of RIDs
	int NumDevices = 0;
	GetRawInputDeviceList(NULL, &NumDevices, sizeof(RAWINPUTDEVICELIST));

	// Make an appropriately-sized array of structures and fill them in
	RAWINPUTDEVICELIST *RIDeviceList = calloc(NumDevices, sizeof(RAWINPUTDEVICELIST));
	GetRawInputDeviceList(RIDeviceList, &NumDevices, sizeof(RAWINPUTDEVICELIST));

	printf("NumDevices: %d\n", NumDevices);

	for(int i = 0; i < NumDevices; ++i)
	{
		// Query name size
		int NameSize = 0;
		GetRawInputDeviceInfo(RIDeviceList[i].hDevice, RIDI_DEVICENAME, NULL, &NameSize);

		// Make enough spacee for the name string and retrieve it
		char *Name = malloc(NameSize);
		GetRawInputDeviceInfo(RIDeviceList[i].hDevice, RIDI_DEVICENAME, Name, &NameSize);

		// Fill in RID_DEVICE_INFO struct
		RID_DEVICE_INFO DeviceInfo = {0};
		int RIDI_size = sizeof(RID_DEVICE_INFO);
		GetRawInputDeviceInfo(RIDeviceList[i].hDevice, RIDI_DEVICEINFO, &DeviceInfo, &RIDI_size);

		printf("Device %d is a ", i);
		switch(RIDeviceList[i].dwType)
		{
			case RIM_TYPEMOUSE:
			{
				printf("MOUSE\n");
				printf("\tNAME:                  %s\n", Name);

				printf("\tdwId:                  %d\n", DeviceInfo.mouse.dwId);
				printf("\tdwNumberOfButtons:     %d\n", DeviceInfo.mouse.dwNumberOfButtons);
				printf("\tdwSampleRate:          %d\n", DeviceInfo.mouse.dwSampleRate);
				printf("\tfHasHorizontalWheel:   %d\n", DeviceInfo.mouse.fHasHorizontalWheel);
			} break;

			case RIM_TYPEKEYBOARD:
			{
				printf("KEYBOARD\n");
				printf("\tNAME:                     %s\n", Name);

				printf("\tdwType:                   %d\n", DeviceInfo.keyboard.dwType);
				printf("\tdwSubType:                %d\n", DeviceInfo.keyboard.dwSubType);
				printf("\tdwKeyboardMode:           %d\n", DeviceInfo.keyboard.dwKeyboardMode);
				printf("\tdwNumberOfFunctionKeys:   %d\n", DeviceInfo.keyboard.dwNumberOfFunctionKeys);
				printf("\tdwNumberOfIndicators:     %d\n", DeviceInfo.keyboard.dwNumberOfIndicators);
				printf("\tdwNumberOfKeysTotal:      %d\n", DeviceInfo.keyboard.dwNumberOfKeysTotal);
			} break;
			
			default:	// RIM_TYPEHID
			{
				printf("HID\n");
				printf("\tNAME:              %s\n", Name);

				printf("\tdwProductId:       %d\n", DeviceInfo.hid.dwProductId);
				printf("\tdwVersionNumber:   %d\n", DeviceInfo.hid.dwVersionNumber);
				printf("\tusUsagePage:       %d\n", DeviceInfo.hid.usUsagePage);
				printf("\tusUsage:           %d\n", DeviceInfo.hid.usUsage);
			} break;
		}
		putc('\n', stdout);

		free(Name);
	}

	free(RIDeviceList);
#endif
	return(0);
}