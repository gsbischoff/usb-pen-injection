#include <windows.h>
#include <stdio.h>
int
main()
{
    RECT Rect = {0};
    if(!GetClientRect(WindowFromDC(GetDC(NULL)), &Rect))
    {
        printf("failed!");
    }
    else
    {
        printf("(%d,%d)\n", Rect.bottom, Rect.right);
    }

    {
        int _dpiX;
        int _dpiY;
        HDC hdc = GetDC(NULL); 
        if (hdc) 
        { 
            _dpiX = GetDeviceCaps(hdc, LOGPIXELSX); 
            _dpiY = GetDeviceCaps(hdc, LOGPIXELSY); 
            ReleaseDC(NULL, hdc); 

            printf("Caps: (%d,%d)\n", _dpiX, _dpiY);

            // Retrieve the horizontal and vertical resolution of the current display setting. 
            int cxScreen = GetSystemMetrics(SM_CXSCREEN); 
            int cyScreen = GetSystemMetrics(SM_CYSCREEN); 

            printf("Met: (%d,%d)\n", cxScreen, cyScreen);
        } 

    }
    return(0);
}