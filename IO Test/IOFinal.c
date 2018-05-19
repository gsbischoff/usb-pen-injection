#include <windows.h>
#include <stdio.h>

void printerr(DWORD err);

LRESULT CALLBACK
WindowProc(HWND Window, UINT Message, WPARAM WParam, LPARAM LParam);

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
		printf("Successfully registered window class!\n");

		HWND WindowHandle = 
			CreateWindowEx(
				WS_EX_TRANSPARENT,
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

			// Add the nth HID
			int HIDIndex = 16; // 16 is my pointer

			if(argc == 2)
			{
				HIDIndex = atoi(argv[1]);
				printf("Got a device index of %d...\n", HIDIndex);
			}
			int HIDNum = 0;

			RAWINPUTDEVICE RID[1] = {0};

			RID_DEVICE_INFO RIDI[1] = {0};
			UINT InfoStructSize = sizeof(RID_DEVICE_INFO);
			for(int i = 0; i < NumDevices; ++i)
			{
				if(RIDeviceList[i].dwType == RIM_TYPEHID)
				{

					if(i == HIDIndex)
					{
						UINT res = GetRawInputDeviceInfo(RIDeviceList[i].hDevice, RIDI_DEVICEINFO, RIDI, &InfoStructSize);
						if(res <= 0)
							continue;

						printf("RID set!\n");
						RID[0].usUsagePage = RIDI[0].hid.usUsagePage;
						RID[0].usUsage = RIDI[0].hid.usUsage;	// mouse : 0x2, pointer: 0x1
						RID[0].dwFlags = RIDEV_INPUTSINK; //RIDEV_INPUTSINK;
						RID[0].hwndTarget = WindowHandle;
					}
				}
			}

			free(RIDeviceList);

			// Usage page & usage of Pointer
			// RID[0].usUsagePage = 0x1;
			// RID[0].usUsage = 0x2;	// mouse : 0x2, pointer: 0x1
			// RID[0].dwFlags = RIDEV_INPUTSINK; //RIDEV_INPUTSINK;
			// RID[0].hwndTarget = WindowHandle;


			BOOL Result = TRUE;
			Result = RegisterRawInputDevices(RID, 1, sizeof(RAWINPUTDEVICE));

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
					//printf("RIDI[0] usagepage %d, usage %d\n", RIDI[0].hid.usUsagePage, RIDI[0].hid.usUsage);
					// Page 13, Usage 2
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
			//printf(".");
			// Get the header
			/*
			RAWINPUTHEADER Header = {0};
			int HeaderSize = sizeof(RAWINPUTHEADER);
			GetRawInputData((HRAWINPUT) LParam, RID_HEADER, &Header, &HeaderSize, HeaderSize);

			int size;
			GetRawInputData((HRAWINPUT) LParam, RID_INPUT, NULL, &size, sizeof(RAWINPUTHEADER));
*/

			int InputSize;
			GetRawInputData((HRAWINPUT) LParam, RID_INPUT, NULL, &InputSize, sizeof(RAWINPUTHEADER));

			RAWINPUT *RI = malloc(InputSize); // = malloc(InputSize);
			GetRawInputData((HRAWINPUT) LParam, RID_INPUT, RI, &InputSize, sizeof(RAWINPUTHEADER));

			// We have the info as RI, we can create a raw handle to it by
			// using the function GlobalAlloc

/*			HGLOBAL HRI = GlobalAlloc(GHND, InputSize);

			// Accesses the memory of this handle so we can initialize it
			RAWINPUT *pRI = GlobalLock(HRI);

			// Initialize our handle's memory with our Rawinput data
			if(pRI)
			{
				//printf("Handle gave back memory!\n");
				memcpy(pRI, RI, InputSize);
			}

			GlobalUnlock(HRI);*/

			RAWINPUT *Rawtest = malloc(InputSize); // = malloc(InputSize);
			memcpy(Rawtest, RI, InputSize);
			HRAWINPUT HRI = (HRAWINPUT) GlobalHandle(Rawtest);

			// Try using our new handle!
			RAWINPUT *Rooty = malloc(InputSize);
			if(HRI)
				GetRawInputData((HRAWINPUT) HRI, RID_INPUT, Rooty, &InputSize, sizeof(RAWINPUTHEADER));
			else
				printf("DIDNT WORK\n\n");

			GlobalFree(HRI);
			
			char *raw = (char *) RI;
			char *raw2 = (char *) Rooty;
			switch(RI->header.dwType)
			{
				case RIM_TYPEHID:
				{
					printf("DD ");
					for(int i = 0; i < InputSize; ++i)
						printf("%02x", raw[i]);
					printf("\nRH ");
					for(int i = 0; i < InputSize; ++i)
						printf("%02x", raw2[i]);
					//printf("HID");
				} break;

				case RIM_TYPEKEYBOARD:
				{
					printf("KEYBOARD");
				} break;

				default:
				{
					printf("MOUSE");
				} break;
			}

			printf("\n");
			//printf("\r");
			//fflush(stdout);
			/*char *pRI = (char *) &RI;
			for(int i = 0; i < sizeof(RAWINPUT); ++i)
			{
				printf("%02x", (unsigned char) pRI[i]);
			}*/
			//printf("RawInput!\n");

			Result = DefWindowProc(Window, Message, WParam, LParam);
		} break;

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