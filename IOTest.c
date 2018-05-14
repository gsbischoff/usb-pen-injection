#include <windows.h>
#include <stdio.h>
//#include <NetworkHeader.h>

int
main()
{
	// Try querying for number of input devices

	
	CURSORINFO CursorInfo;
	CursorInfo.cbSize = sizeof(CURSORINFO);

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

	/*
	UINT32 deviceCount;

	if(GetPointerDevices(&deviceCount, 0) == 0)
		printf("GetPointerDevices() failed");
	else
		printf("deviceCount = %d\n", deviceCount);*/

	return(0);
}