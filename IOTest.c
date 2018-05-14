#include <windows.h>
//#include <NetworkHeader.h>
//void DieWithError(char *errorMessage);

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
			printf("x: %d\ty: %d\n", Point.x, Point.y);

		Sleep(1);

		//Last.x = Point.x;
		//Last.y = Point.y;
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