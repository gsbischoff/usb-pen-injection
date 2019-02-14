#ifndef PEN_H
#define PEN_H

#ifndef CS_OWNDC
#include <windows.h>
#endif

LRESULT CALLBACK WindowProc(HWND Window, UINT Message, WPARAM WParam, LPARAM LParam);

static void SetupPen(void);
static void printerr(DWORD err);


#endif