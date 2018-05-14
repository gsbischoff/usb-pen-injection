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

	printf("0x%04x\t W: %x\tL:%x\n", Message, WParam, LParam);

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
#else
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

	for(;;)
	{
		MSG Msg;
		if(PeekMessage(&Msg, NULL, 0, 0, PM_NOREMOVE))
			printf("Got message!\n");
		else
			printf("No messages\n");
		Sleep(500);
	}
#endif
	return(0);
}