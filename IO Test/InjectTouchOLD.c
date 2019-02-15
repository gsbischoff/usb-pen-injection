
void
InjectPenInput(POINTER_PEN_INFO PenInfo, POINT *Lastpt, POINTER_FLAGS *lastMask)
{
    POINTER_TOUCH_INFO touchInfo; // = {0};
    memset(&touchInfo, 0, sizeof(POINTER_TOUCH_INFO));

    POINTER_FLAGS mask = POINTER_FLAG_INRANGE | POINTER_FLAG_INCONTACT | POINTER_FLAG_DOWN | POINTER_FLAG_UP | POINTER_FLAG_UPDATE;

    //memcpy(&touchInfo.pointerInfo, &PenInfo.pointerInfo, sizeof(POINTER_INFO));
    touchInfo.pointerInfo.pointerType             = PT_TOUCH;            
    touchInfo.pointerInfo.pointerId                 = 0; //PenInfo.pointerInfo.pointerId;                
//        touchInfo.pointerInfo.frameId                 = PenInfo.pointerInfo.frameId;                     
    touchInfo.pointerInfo.pointerFlags             = PenInfo.pointerInfo.pointerFlags & mask; //POINTER_FLAG_DOWN | POINTER_FLAG_INRANGE | POINTER_FLAG_INCONTACT; //;                
//        touchInfo.pointerInfo.sourceDevice              = PenInfo.pointerInfo.sourceDevice;             
//        touchInfo.pointerInfo.hwndTarget             = PenInfo.pointerInfo.hwndTarget;                
    touchInfo.pointerInfo.ptPixelLocation         = PenInfo.pointerInfo.ptPixelLocation;            
//        touchInfo.pointerInfo.ptHimetricLocation     = PenInfo.pointerInfo.ptHimetricLocation;        
//        touchInfo.pointerInfo.ptPixelLocationRaw     = PenInfo.pointerInfo.ptPixelLocationRaw;        
//        touchInfo.pointerInfo.ptHimetricLocationRaw  = PenInfo.pointerInfo.ptHimetricLocationRaw;    
    touchInfo.pointerInfo.dwTime                 = 0; //PenInfo.pointerInfo.dwTime;                    
//        touchInfo.pointerInfo.historyCount             = PenInfo.pointerInfo.historyCount;                
    touchInfo.pointerInfo.InputData                 = PenInfo.pointerInfo.InputData;                
//        touchInfo.pointerInfo.dwKeyStates             = PenInfo.pointerInfo.dwKeyStates;                
    touchInfo.pointerInfo.PerformanceCount         = 0; //PenInfo.pointerInfo.PerformanceCount;            
//        touchInfo.pointerInfo.ButtonChangeType         = PenInfo.pointerInfo.ButtonChangeType;

    /* When POINTER_FLAG_UP is set, ptPixelLocation of POINTER_INFO
        should be the same as the value of the previous touch injection
                frame with POINTER_FLAG_UPDATE. */
    if(touchInfo.pointerInfo.pointerFlags & POINTER_FLAG_UPDATE)
    {
        Lastpt->x = touchInfo.pointerInfo.ptPixelLocation.x;
        Lastpt->y = touchInfo.pointerInfo.ptPixelLocation.y;
    }
    if(touchInfo.pointerInfo.pointerFlags & POINTER_FLAG_UP)
    {
        if(Lastpt->x != touchInfo.pointerInfo.ptPixelLocation.x ||
            Lastpt->y != touchInfo.pointerInfo.ptPixelLocation.y)
        {
            Lastpt->x = touchInfo.pointerInfo.ptPixelLocation.x;
            Lastpt->y = touchInfo.pointerInfo.ptPixelLocation.y;
        }
    }            

    // Don't have penFlag for eraser! Touch has no flags!
    touchInfo.touchFlags = TOUCH_FLAG_NONE;
    touchInfo.touchMask = TOUCH_MASK_ORIENTATION | TOUCH_MASK_PRESSURE; // only injecting pressure
    touchInfo.orientation = PenInfo.rotation;
    touchInfo.pressure = PenInfo.pressure;

    if(*lastMask == POINTER_FLAG_INRANGE | POINTER_FLAG_UPDATE &&
        touchInfo.pointerInfo.pointerFlags == POINTER_FLAG_INRANGE | POINTER_FLAG_INCONTACT | POINTER_FLAG_UPDATE)
        touchInfo.pointerInfo.pointerFlags = POINTER_FLAG_INRANGE | POINTER_FLAG_INCONTACT | POINTER_FLAG_DOWN;

    if(!InjectTouchInput(1, &touchInfo))
    {
        printf("%x\n", touchInfo.pointerInfo.pointerFlags);
        switch(touchInfo.pointerInfo.pointerFlags)
        {
            case POINTER_FLAG_INRANGE | POINTER_FLAG_UPDATE:
                printf("Case 1\n");
                break;
            case POINTER_FLAG_INRANGE | POINTER_FLAG_INCONTACT | POINTER_FLAG_DOWN:
                printf("Case 2\n");
                break;
            case POINTER_FLAG_INRANGE | POINTER_FLAG_INCONTACT | POINTER_FLAG_UPDATE:
                printf("Case 3\n");
                printf("Last: %x\n", *lastMask);
                break;
            case POINTER_FLAG_INRANGE | POINTER_FLAG_UP:
                printf("Case 4\n");
                break;
            case POINTER_FLAG_UPDATE:
                printf("Case 5\n");
                break;
            case POINTER_FLAG_UP:
                printf("Case 6\n");
                break;
            default:
                printf("DEFAULT! %x\n", touchInfo.pointerInfo.pointerFlags);
                break;
        }
        /*
NRANGE | UPDATE    Touch hover starts or moves
INRANGE | INCONTACT | DOWN    Touch contact down
INRANGE | INCONTACT | UPDATE    Touch contact moves
INRANGE | UP    Touch contact up and transition to hover
UPDATE    Touch hover ends
U
        */
        DieWithError("InjectTouchInput() failed");
        exit(1);
    }
    *lastMask = touchInfo.pointerInfo.pointerFlags;

    touchInfo.pointerInfo.pointerFlags             = POINTER_FLAG_UP;
    InjectTouchInput(1, &touchInfo);
}