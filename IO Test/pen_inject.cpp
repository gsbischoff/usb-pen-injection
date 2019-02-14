#include <windows.h>
#include <stdio.h>
#include <winuser.h>

#define ArrayCount(arr) (sizeof(arr)/sizeof(arr[0]))

int
main(int ArgCount, char **Args)
{
    EnableMouseInPointer(TRUE);
    POINT MouseRecording[60 * 5];
    
    int Start = 0;
    int End = 0;

    // Get some "pen" input
    int Recording = 1;
    while(Recording)
    {
        GetCursorPos(MouseRecording + End);

        End = (End + 1) % ArrayCount(MouseRecording);

        if(End == Start)
            Start = (Start + 1) % ArrayCount(MouseRecording);

        printf("Start: %4u End: %4u\r", Start, End);

        Sleep(16);
    }

    printf("Injecting EHEEHEEE\n");

    POINTER_PEN_INFO PointerInfo = {0};
    
    
    // Attempt to inject pen input
    //
    #if 0
    HSYNTHETICPOINTERDEVICE SyntheticPointer =
        CreateSyntheticPointerDevice(PT_PEN, 1, POINTER_FEEDBACK_INDIRECT);
    
    if(!InjectSyntheticPointerInput(SyntheticPointer, &PointerInfo, 1))
    {
        printf("InjectSyntheticPointerInput() failed\n");
    }
    DestroySyntheticPointerDevice(SyntheticPointer);
    #endif
    return(0);
}