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

			RAWINPUTDEVICE RID[1] = {0};
			
			// Usage page & usage of Mouse
			RID[0].usUsagePage = 0xd;
			RID[0].usUsage = 0x2; // mouse : 0x2, pointer: 0x1
			RID[0].dwFlags = RIDEV_INPUTSINK; //RIDEV_INPUTSINK;
			RID[0].hwndTarget = WindowHandle;

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
					// Page 13, Usage 2 [surface ptr]
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
			// Find out size of input
			int InputSize;
			GetRawInputData((HRAWINPUT) LParam, RID_INPUT, NULL, &InputSize, sizeof(RAWINPUTHEADER));

			// Allocate and fill in a RAWINPUT struct of that size
			RAWINPUT *RI = malloc(InputSize); // = malloc(InputSize);
			GetRawInputData((HRAWINPUT) LParam, RID_INPUT, RI, &InputSize, sizeof(RAWINPUTHEADER));

			// We have the info as RI, we can create a raw handle to it by
			// using the function GlobalAlloc

			// GlobalAlloc(GHND, InputSize) and GlobalHandle(void *) both return handles
			// GlobalLock(HGLOBAL h) returns the memory of the handle
			// GlobalHandle(HGLOBAL h) locks back up that memory

			if(1)
			{
				// Test to see if GetRawInputData locks a handle
				HGLOBAL handle = GlobalAlloc(GHND, InputSize);

				RAWINPUT *rawdata = GlobalLock(handle);


				if(rawdata)
				{
					printf("Successfully created our handle...\n");

					RAWINPUT t = {0};

					t.data.mouse.ulButtons = 0xFFFF;
					memcpy(rawdata, &t, InputSize);

					// Close it and test our ruse...
					handle = GlobalHandle(rawdata);

					if(handle)
					{
						printf("Memory \'safe\'...\n");

						RAWINPUT *test = malloc(InputSize);

						if(GetRawInputData((HRAWINPUT) handle, RID_INPUT, test, &InputSize, sizeof(RAWINPUTHEADER)) >= 0)
						{
							printf("We got something...\n");
							printerr(GetLastError());

							printf("%d", test->data.mouse.ulButtons);
						}
						else
						{
							printf("Nooo\n");
							printerr(GetLastError());
						}
					}

				}
			}
			printf("pssst\n");


			printf("LParam: %d\n", (UINT) LParam);
			
			
			switch(RI->header.dwType)
			{
				case RIM_TYPEHID:
				{
					/*printf("DD ");
					for(int i = 0; i < InputSize; ++i)
						printf("%02x", raw[i]);
					printf("\nRH ");
					for(int i = 0; i < InputSize; ++i)
						printf("%02x", raw2[i]);*/
					//printf("HID");
				} break;

				case RIM_TYPEKEYBOARD:
				{
					printf("KEYBOARD");
				} break;

				default:
				{
					//printf("MOUSE");
				} break;
			}

			free(RI);

			/*char *pRI = (char *) &RI;
			for(int i = 0; i < sizeof(RAWINPUT); ++i)
			{
				printf("%02x", (unsigned char) pRI[i]);
			}*/

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