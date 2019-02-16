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

    if(0)
    {
        int _dpiX;
        int _dpiY;
        HDC hdc = GetDC(NULL); 
        if (hdc) 
        { 
            _dpiX = GetDeviceCaps(hdc, LOGPIXELSX); 
            _dpiY = GetDeviceCaps(hdc, LOGPIXELSY); 

            printf("LOG DPI: (%d,%d)\n", _dpiX, _dpiY);

            _dpiX = GetDeviceCaps(hdc, HORZSIZE); 
            _dpiY = GetDeviceCaps(hdc, VERTSIZE); 

            printf("Caps: (%d,%d)\n", _dpiX, _dpiY);
            ReleaseDC(NULL, hdc); 

            // Retrieve the horizontal and vertical resolution of the current display setting. 
            int cxScreen = GetSystemMetrics(SM_CXSCREEN); 
            int cyScreen = GetSystemMetrics(SM_CYSCREEN); 

            printf("Met: (%d,%d)\n", cxScreen, cyScreen);
        } 

    }
    return(0);
}