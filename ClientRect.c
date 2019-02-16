#include <windows.h>
#include <stdio.h>

#include <shtypes.h>
#include <shellscalingapi.h>

typedef struct
{
    int horizontalResolution;
    int verticalResolution;

    int horizontalDPI;
    int verticalDPI;
} resolution_settings;

resolution_settings
GetDeviceResolutionSettings()
{
    resolution_settings Result;

    // Get desktop dc
    HDC desktopDC = GetDC(NULL);

    SetProcessDpiAwareness(PROCESS_SYSTEM_DPI_AWARE);

    // Get native resolution
    Result.horizontalResolution = GetDeviceCaps(desktopDC, HORZRES);
    Result.verticalResolution = GetDeviceCaps(desktopDC, VERTRES); // Accidentally called 'VERZRES' in example in doc

    // Get native resolution
    Result.horizontalDPI = GetDeviceCaps(desktopDC, LOGPIXELSX);
    Result.verticalDPI = GetDeviceCaps(desktopDC, LOGPIXELSY);

    return(Result);
}

int
main()
{
    resolution_settings Settings = GetDeviceResolutionSettings();
    return(0);
}