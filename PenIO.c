#include "PenIO.h"

static void
SetupPen()
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
			RAWINPUTDEVICE RID = {0};

			// Page and usage of surface pen
			RID.usUsagePage = 0xC;
			RID.usUsage = 0x2;	// mouse : 0x2, pointer: 0x1
			RID.dwFlags = RIDEV_INPUTSINK; //RIDEV_INPUTSINK;
			RID.hwndTarget = WindowHandle;

			BOOL Result = TRUE;
			Result = RegisterRawInputDevices(RID, 1, sizeof(RAWINPUTDEVICE));

			if(!Result)
			{
				printf("Registering the RIDs failed!\n");

				DWORD err = GetLastError();
				printerr(err);

				break;
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

			DWORD err = GetLastError();
			printerr(err);
		}
	}
	else
	{
		printf("Failed to register window!\n");

		DWORD err = GetLastError();
		printerr(err);
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
			int InputSize;
			GetRawInputData((HRAWINPUT) LParam, RID_INPUT, NULL, &InputSize, sizeof(RAWINPUTHEADER));

			RAWINPUT *RI = malloc(InputSize); // = malloc(InputSize);
			GetRawInputData((HRAWINPUT) LParam, RID_INPUT, RI, &InputSize, sizeof(RAWINPUTHEADER));
			
			char *raw = (char *) RI;
			switch(RI->header.dwType)
			{
				case RIM_TYPEHID:
				{
					for(int i = 0; i < InputSize; ++i)
						printf("%02x", raw[i]);
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

			printf("\t\t");
			printf("\r");
			fflush(stdout);

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