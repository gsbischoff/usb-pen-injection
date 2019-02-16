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

    int scalingPercent;
} resolution_settings;

resolution_settings
GetDeviceResolutionSettings()
{
    resolution_settings Result;

    // Get the device mode settings for actual values of resolution
    DEVMODEA DevMode;
    DevMode.dmSize = sizeof(DevMode);
    EnumDisplaySettings(0, ENUM_CURRENT_SETTINGS, &DevMode);

    // Get desktop dc
    HDC desktopDC = GetDC(NULL);

    // Get native resolution
    Result.horizontalResolution = GetDeviceCaps(desktopDC, HORZRES);
    Result.verticalResolution = GetDeviceCaps(desktopDC, VERTRES); // Accidentally called 'VERZRES' in example in doc

/*
    // Get native resolution
    Result.horizontalDPI = GetDeviceCaps(desktopDC, LOGPIXELSX);
    Result.verticalDPI = GetDeviceCaps(desktopDC, LOGPIXELSY);
*/
    Result.scalingPercent = (DevMode.dmPelsWidth * 100) / Result.horizontalResolution;

    DevMode.dmPelsWidth;
    DevMode.dmPelsHeight;

    return(Result);
}

int
main()
{
    resolution_settings Settings = GetDeviceResolutionSettings();
    return(0);
}