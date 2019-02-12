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
#if 0
	printf("0x%04x\t W: %llx\tL:%llx\n", Message, WParam, LParam);
#endif
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

int
main()
{
	// Try querying for number of input devices

	
	CURSORINFO CursorInfo;
	CursorInfo.cbSize = sizeof(CURSORINFO);

	// This block attempts to open RIDs using createfile
#if 0
	GetCursorInfo(&CursorInfo);

	//Try to get mouse RI
	int NumDevices;
	GetRawInputDeviceList(NULL, &NumDevices, sizeof(RAWINPUTDEVICELIST));

	RAWINPUTDEVICELIST *RIDeviceList = malloc(NumDevices * sizeof(RAWINPUTDEVICELIST));
	GetRawInputDeviceList(RIDeviceList, &NumDevices, sizeof(RAWINPUTDEVICELIST));

	for(int i = 0; i < NumDevices; ++i)
	{
		int DeviceNameSize;
		GetRawInputDeviceInfo(RIDeviceList[i].hDevice, RIDI_DEVICENAME, NULL, &DeviceNameSize);

		char *name = malloc(DeviceNameSize);
		GetRawInputDeviceInfo(RIDeviceList[i].hDevice, RIDI_DEVICENAME, name, &DeviceNameSize);

		HANDLE DeviceHandle;

		if((DeviceHandle = CreateFile(name, 0, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL)) != INVALID_HANDLE_VALUE)
		{
			printf("Something worked!\n");

			char buf[100];
			int BytesRead;

			if(ReadFile(DeviceHandle, buf, 100, &BytesRead, 0))
			{
				printf("Read succeeded!: ");

				for(int j = 0; j < 100; j++)
					printf("%02x", buf[j]);
			}
			else
			{
				printf("Ln %d\n", __LINE__);
				printerr(GetLastError());
			}

			CloseHandle(DeviceHandle);
		}
		else
		{
			printf("Ln %d\n", __LINE__);
			printerr(GetLastError());
		}

		free(name);
	}
#elif 0
	// Gets a list of RIDs and counts # of mice. Additionally, registers 2 RIDs
	// to a window (mouse and pointer), of which only mouse works :(
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
			printf("Window handle valid!\n");
			/*
			if(ShowWindow(WindowHandle, SW_HIDE))
				printf("Window Hidden!\n");
			else
				printf("Window... not hidden\n");
			*/

			//Try to get mouse RI
			int NumDevices;
			GetRawInputDeviceList(NULL, &NumDevices, sizeof(RAWINPUTDEVICELIST));

			RAWINPUTDEVICELIST *RIDeviceList = malloc(NumDevices * sizeof(RAWINPUTDEVICELIST));
			GetRawInputDeviceList(RIDeviceList, &NumDevices, sizeof(RAWINPUTDEVICELIST));

			printf("Got back %d devices\n", NumDevices);

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

			//RAWINPUTDEVICE *RIDevs = malloc(NumMice * sizeof(RAWINPUTDEVICE));

			// Add all mouses ONLY [loop through devices and only add mice]
			/*int i = 0;
			for(int j = 0; i < NumDevices; ++i)
			{
				if(RIDeviceList[i].dwType == RIM_TYPEMOUSE)
				{
					RID_DEVICE_INFO Info = {0};
					Info.cbSize = sizeof(RID_DEVICE_INFO);
					int size = sizeof(RID_DEVICE_INFO);

					// Query the extended info of this mouse
					GetRawInputDeviceInfo(RIDeviceList[i].hDevice, RIDI_DEVICEINFO, &Info, &size);

					// Fill in the device info to register with these values we got
					RIDevs[i].usUsagePage = 0;
					RIDevs[i].usUsage = 0;
					RIDevs[i].dwFlags = 0;
					RIDevs[i].hwndTarget = 0;

					i++;
				}
			}*/
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
/*				printf("MOUSE\n");
				printf("\tNAME:                  %s\n", Name);

				printf("\tdwId:                  %d\n", DeviceInfo.mouse.dwId);
				printf("\tdwNumberOfButtons:     %d\n", DeviceInfo.mouse.dwNumberOfButtons);
				printf("\tdwSampleRate:          %d\n", DeviceInfo.mouse.dwSampleRate);
				printf("\tfHasHorizontalWheel:   %d\n", DeviceInfo.mouse.fHasHorizontalWheel);
*/
			} break;

			case RIM_TYPEKEYBOARD:
			{
/*				printf("KEYBOARD\n");
				printf("\tNAME:                     %s\n", Name);

				printf("\tdwType:                   %d\n", DeviceInfo.keyboard.dwType);
				printf("\tdwSubType:                %d\n", DeviceInfo.keyboard.dwSubType);
				printf("\tdwKeyboardMode:           %d\n", DeviceInfo.keyboard.dwKeyboardMode);
				printf("\tdwNumberOfFunctionKeys:   %d\n", DeviceInfo.keyboard.dwNumberOfFunctionKeys);
				printf("\tdwNumberOfIndicators:     %d\n", DeviceInfo.keyboard.dwNumberOfIndicators);
				printf("\tdwNumberOfKeysTotal:      %d\n", DeviceInfo.keyboard.dwNumberOfKeysTotal);
*/
			} break;
			
			default:	// RIM_TYPEHID
			{
				printf("HID\n");
				printf("\tNAME:              %s\n", Name);

				printf("\tdwProductId:       %d\n", DeviceInfo.hid.dwProductId);
				printf("\tdwVersionNumber:   %d\n", DeviceInfo.hid.dwVersionNumber);
				printf("\tusUsagePage:       %d\n", DeviceInfo.hid.usUsagePage);
				printf("\tusUsage:           %d\n", DeviceInfo.hid.usUsage);

				switch(DeviceInfo.hid.usUsage)
				{
					case 0x1:
					{
						printf("\tDevice Type:       %s\n", DeviceInfo.hid.usUsagePage == 0x1 ? "Pointer" : "Consumer Audio Control");
					} break;

					case 0x2:
					{
						printf("\tDevice Type:       %s\n", "Mouse");
					} break;

					case 0x3:
					{
						printf("\tDevice Type:       %s\n", "Joystick");
					} break;

					case 0x4:
					{
						printf("\tDevice Type:       %s\n", "Gamepad");
					} break;

					case 0x5:
					{
						printf("\tDevice Type:       %s\n", "Keyboard");
					} break;

					case 0x7:
					{
						printf("\tDevice Type:       %s\n", "Keypad");
					} break;

					case 0x80:
					{
						printf("\tDevice Type:       %s\n", "System Control");
					} break;

					default:
					{
						printf("\tDevice Type:       ?\n", "");
					} break;
				}
			} break;
		}
		putc('\n', stdout);
/*
		// Try reopening device
		HANDLE dev = CreateFile(Name, GENERIC_READ, FILE_SHARE_READ, NULL, 
								OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);

		if(dev == INVALID_HANDLE_VALUE)
		{
			printf("\tOn CreateFile()\n\t");

			DWORD err = GetLastError();
			printerr(err);

			dev = RIDeviceList[i].hDevice;
		}
		else
		{
			printf("\tOpening device succeeded!\n");
		}
		// Lets try using ReadFile
		char data[64];
		int bytesRead = 0;
		BOOL bResult = ReadFile(dev, data, 64, &bytesRead, NULL);

		if(bResult)
			printf("\tRead succeeded!\n");
		else
		{
			printf("\tOn ReadFile()\n\t");

			DWORD err = GetLastError();
			printerr(err);
		}
*/
		free(Name);
	}

	free(RIDeviceList);
#endif
	return(0);
}