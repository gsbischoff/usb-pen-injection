//#define WIN32_LEAN_AND_MEAN

#include "NetworkHeader.h"

void printerr(DWORD err);
LRESULT CALLBACK WindowProc(HWND Window, UINT Message, WPARAM WParam, LPARAM LParam);
DWORD __stdcall ThreadProc(LPVOID lpParameter);
HANDLE ThreadHandle;
UINT32 NumHits;
SOCKET sendSock;
struct sockaddr_in *clnt;
//UINT32 PointerId;

int
initPoint(SOCKET s, struct sockaddr_in *c)
{
	sendSock = s;
	clnt = c;
	WNDCLASS WindowClass = {0};

	WindowClass.style = CS_OWNDC;
	WindowClass.lpfnWndProc = WindowProc;
	WindowClass.hInstance = GetModuleHandle(0);
	WindowClass.lpszClassName = "WindowClass";


	printf("ptr info siz: %zu\n", sizeof(POINTER_INFO));
	if(RegisterClass(&WindowClass))
	{
		HWND WindowHandle = 
			CreateWindowEx(
				WS_EX_TRANSPARENT,
				WindowClass.lpszClassName,
				"Dummy",
				WS_OVERLAPPEDWINDOW | WS_VISIBLE,
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

			BOOL bRes = RegisterPointerDeviceNotifications(WindowHandle, TRUE);

			if(!bRes)
				printerr(GetLastError());

			// Create a thread to send the pen's state while it is in frame.
			ThreadHandle = CreateThread(0, 0, ThreadProc, NULL, CREATE_SUSPENDED, 0);
				
			if(!ThreadHandle)
			{
				printerr(GetLastError());
				exit(1);
			}
			ResumeThread(ThreadHandle);
		
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
			UINT32 PointerID = GET_POINTERID_WPARAM(WParam);
			//ResumeThread(ThreadHandle);
			InterlockedIncrement(&NumHits);

			POINTER_PEN_INFO Info = {0};
			if(IS_POINTER_INRANGE_WPARAM(WParam) 
			   && GetPointerPenInfo(PointerID, &Info))
			{
				// PointerInfo struct + pressure (set touchmask + flags appropriately)
				unsigned char *spreadStruct = serialize(&Info);

				/*printf("(%5d,%5d) %5u\r", 
					Info.pointerInfo.ptPixelLocation.x, 
					Info.pointerInfo.ptPixelLocation.y, 
					Info.pressure);*/

				// sendInput(&Info, ...)
				// Or add to thread's work queue (maybe use GetPointerPenInfoHistory()?)
				if(sendto(sendSock, spreadStruct, 178, 0,
					(struct sockaddr *) clnt, sizeof(struct sockaddr)) < 0)
					DieWithError("sendto() failed");
			}

			Result = DefWindowProc(Window, Message, WParam, LParam);
		} break;

/*		case WM_POINTERDEVICEINRANGE:
		{
			//printf("Ptr in range! %u\n", IS_POINTER_INRANGE_WPARAM(WParam));
			//InterlockedExchange(&PointerId, GET_POINTERID_WPARAM(WParam));
		} break;

		case WM_POINTERDEVICEOUTOFRANGE:
		{
			//printf("Ptr out of range!: %u\n", IS_POINTER_INRANGE_WPARAM(WParam));
			//SuspendThread(ThreadHandle);
		} break;*/

		case WM_DESTROY:
		{
			PostQuitMessage(0);
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

DWORD __stdcall
ThreadProc(LPVOID lpParameter)
{
	for(;;)
	{
		printf("%03u hits.\n", InterlockedExchange(&NumHits, 0)); //

		Sleep(1000);

		// Add a global variable that will indicate we have a new, unsent struct
		// Set it to 0 once we've sent it (Interlocked)
	}
}