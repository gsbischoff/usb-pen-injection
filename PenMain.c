//#include "NetworkHeader.h"
#include <windows.h>
#include <stdio.h>

LRESULT CALLBACK
WindowProc(HWND Window, UINT Message, WPARAM WParam, LPARAM LParam)
{
	LRESULT Result = 0;

	switch(Message)
	{
		case WM_INPUT:
		{
			// Find out size of input
			int InputSize;
			GetRawInputData((HRAWINPUT) LParam, RID_INPUT, NULL, &InputSize, sizeof(RAWINPUTHEADER));

			// Allocate and fill in a RAWINPUT struct of that size
			RAWINPUT *RI = malloc(InputSize);
			GetRawInputData((HRAWINPUT) LParam, RID_INPUT, RI, &InputSize, sizeof(RAWINPUTHEADER));

			switch(RI->data.keyboard.VKey)
			{
				default:
				{

				} break;
			}
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
	RAWINPUTDEVICE RID = {0};

	
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

		RID.usUsagePage = 0x1;
		RID.usUsage = 0x6; // Keyboard
		RID.dwFlags = RIDEV_INPUTSINK; //RIDEV_INPUTSINK; //RIDEV_INPUTSINK;
		RID.hwndTarget = WindowHandle;
		//RID.hwndTarget = GetConsoleWindow();

		RegisterRawInputDevices(&RID, 1, sizeof(RAWINPUTDEVICE));
		
		if(WindowHandle)
		{
			printf("Success!\n");

			//BOOL bRes = RegisterPointerDeviceNotifications(WindowHandle, TRUE);

			//if(!bRes)
			//	printerr(GetLastError());

			MSG Message;
			for(;;)
			{
				BOOL MessageResult = GetMessage(&Message, 0, 0, 0);
				//BOOL MessageResult = GetMessage(&Message, GetConsoleWindow(), 0, 0);
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

			//printerr(GetLastError());
		}
	}
	return(0);
}