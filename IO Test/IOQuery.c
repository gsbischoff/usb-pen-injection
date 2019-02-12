#include <windows.h>
#include <Hidsdi.h>
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

		if(RIDeviceList[i].dwType != RIM_TYPEHID || DeviceInfo.hid.usUsagePage != 13)
		{
			free(Name);
			continue;
		}
		POINTER_DEVICE_INFO PointerInfo = {0};
		if(GetPointerDevice(RIDeviceList[i].hDevice, &PointerInfo))
		{
			printf("  Got pointer info!\n");
			wprintf(L"  PRODSTR: %s\n", PointerInfo.productString);
			GetRawPointerDeviceData(PointerInfo.pointerDeviceType)
		}
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
		free(Name);
	}

	free(RIDeviceList);

	return(0);
}