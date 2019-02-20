#include "NetworkHeader.h"

#include "ResolveHost.c"
#include "Serial.c"
#include "DieWithError.c"

#include <shtypes.h>
#include <shellscalingapi.h>
#include <time.h>

POINTER_FLAGS ContextualizeFlags(POINTER_PEN_INFO *, POINTER_PEN_INFO *);

// -----
//   PenClient.c
//        This program implements the connection between the two
//            computers using network sockets
int
main(int argc, char **argv)
{
    int sock;                        /* Socket descriptor for client */
    struct sockaddr_in ServerAddr;    /* Local address */
    struct sockaddr_in ClientAddr;
    struct sockaddr_in FromAddr;
    char *ServerHost;                    /* Server address in dotted quad */
    char *ServerPortString;                /* Server port */
    unsigned short ServerPort;             /* Server port */
    unsigned int fromLen;                /* Length of client address data structure */

    if(argc != 2)
    {
        printf("Usage: %s [<server Host>[:<port>]]", argv[0]);
        exit(1);
    }
    else
    {
        ServerHost = strtok(argv[1], ":");

        if((ServerPortString = strtok(NULL, ":")) != NULL)
            ServerPort = atoi(ServerPortString);
    }


    struct WSAData data = { 0 };
    WORD wVersionRequested = MAKEWORD(1,1);

    if(WSAStartup(wVersionRequested, &data))
        DieWithError("WSAStartup() failed");

    /* Fill in serving address */
    memset(&ServerAddr, 0, sizeof(ServerAddr));
    ServerAddr.sin_family        = AF_INET;
    ServerAddr.sin_addr.s_addr   = ResolveHost(ServerHost); //inet_addr(ServerHost);
    ServerAddr.sin_port          = htons(ServerPort);

    if((sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) < 0)
        DieWithError("socket() failed");

    /* Fill in our address */
    memset(&ClientAddr, 0, sizeof(ClientAddr));
    ClientAddr.sin_family        = AF_INET;
    ClientAddr.sin_addr.s_addr   = htonl(INADDR_ANY); //inet_addr(ServerHost);
    ClientAddr.sin_port          = htons(ServerPort);

    if(bind(sock, (struct sockaddr *) &ClientAddr, sizeof(ClientAddr)) < 0)
        DieWithError("bind() failed");

    {
        DEVMODEA DevMode;
        DevMode.dmSize = sizeof(DevMode);
        EnumDisplaySettings(0, ENUM_CURRENT_SETTINGS, &DevMode);

        printf("HEIGHT: %d\n", DevMode.dmPelsWidth);

        // Get desktop dc
        HDC desktopDC = GetDC(NULL);

        // Get native resolution
        int horizontalResolution = GetDeviceCaps(desktopDC, HORZRES);
        int verticalResolution = GetDeviceCaps(desktopDC, VERTRES); // Accidentally called 'VERZRES' in example in doc

        // Get native resolution
        int horizontalDPI = GetDeviceCaps(desktopDC, LOGPIXELSX);
        int verticalDPI = GetDeviceCaps(desktopDC, LOGPIXELSY);

        printf("RES: [%d,%d], DPI: [%d,%d]\n\n", 
            horizontalResolution, verticalResolution,
            horizontalDPI, verticalDPI);
    }

    unsigned char expandedBuf[178];

    POINTER_TYPE_INFO PointerInfo = {0};
    POINTER_TYPE_INFO LastPointerInfo = {0};
    PointerInfo.type = PT_PEN;

    HSYNTHETICPOINTERDEVICE SyntheticPointer =
        CreateSyntheticPointerDevice(PT_PEN, 1, POINTER_FEEDBACK_DEFAULT);

    if(!SyntheticPointer)
    {
        fprintf(stderr, "CreateSyntheticPointerDevice() failed");
        exit(1);
    }

    for(;;)
    {
        fromLen = sizeof(FromAddr);

        if(recvfrom(sock, expandedBuf, sizeof(expandedBuf), 0,
           (struct sockaddr *) &FromAddr, &fromLen) < 0)
            DieWithError("recvfrom() failed");

        deserialize(expandedBuf, &PointerInfo.penInfo);

        /* Skip using this input if it is out-of-order */
        if(PointerInfo.penInfo.pointerInfo.PerformanceCount < 
           LastPointerInfo.penInfo.pointerInfo.PerformanceCount)
            continue;

        POINT ThisPt = PointerInfo.penInfo.pointerInfo.ptPixelLocation;


        printf("(%d,%d) \r", ThisPt.x, ThisPt.y);

        // These bastards apparently must be zero
        PointerInfo.penInfo.pointerInfo.dwTime = 0;
        PointerInfo.penInfo.pointerInfo.PerformanceCount = 0;

        PointerInfo.penInfo.pointerInfo.pointerFlags =
            ContextualizeFlags(&PointerInfo.penInfo, &LastPointerInfo.penInfo);

        if(FromAddr.sin_addr.s_addr == ServerAddr.sin_addr.s_addr)
        {
            if(!InjectSyntheticPointerInput(SyntheticPointer, &PointerInfo, 1))
            {
                printf("InjectSyntheticPointerInput() failed");
            }
        }

        LastPointerInfo = PointerInfo;
    }
    DestroySyntheticPointerDevice(SyntheticPointer);

    closesocket(sock);
    WSACleanup();

    return(0);
}

enum
{
    PEN_STATE_MASK = (POINTER_FLAG_INRANGE | POINTER_FLAG_INCONTACT | POINTER_FLAG_DOWN | POINTER_FLAG_UP | POINTER_FLAG_UPDATE),

    PEN_HOVER    = (POINTER_FLAG_INRANGE | POINTER_FLAG_UPDATE),
    PEN_DOWN     = (POINTER_FLAG_INRANGE | POINTER_FLAG_INCONTACT | POINTER_FLAG_DOWN),
    PEN_CONTACT  = (POINTER_FLAG_INRANGE | POINTER_FLAG_INCONTACT | POINTER_FLAG_UPDATE),
    PEN_UP       = (POINTER_FLAG_INRANGE | POINTER_FLAG_UP),
    PEN_ENDHOVER = (POINTER_FLAG_UPDATE),
};

//  -----
//  Ensures compliance of the change of pointer flags with what will be accepted
//      by adding in transition states if missing from the last frame
POINTER_FLAGS
ContextualizeFlags(POINTER_PEN_INFO *PointerInfo, POINTER_PEN_INFO *LastPointerInfo)
{
    POINTER_FLAGS Mask = PEN_STATE_MASK;

    POINTER_FLAGS Result = PointerInfo->pointerInfo.pointerFlags;
    POINTER_FLAGS Last = LastPointerInfo->pointerInfo.pointerFlags & Mask;

    switch(Result & Mask)
    {
        case PEN_HOVER:
        {
            if((Last & PEN_CONTACT) == PEN_CONTACT)
            {
                Result &= ~PEN_HOVER;
                Result |=  PEN_UP;
            }
        } break;
        case PEN_DOWN:
        {
            if((Last & PEN_HOVER) == PEN_HOVER)
            {
                printf("ERROR: Pen went to DOWN state from not HOVERING! %u", Last);
            }
        } break;
        case PEN_CONTACT:
        {
            if((Last & PEN_HOVER) == PEN_HOVER)
            {
                Result &= ~PEN_CONTACT;
                Result |=  PEN_DOWN;
            }
        } break;
        case PEN_UP:
        {
            printf("UP!\n");
        } break;
        case POINTER_FLAG_UPDATE:
        {
            printf("Ending hover\n");
        } break;
        case POINTER_FLAG_UP:
        {
            printf("Ending hover!!\n");
        } break;
        default:
        {
            printf("SOMETHING ELSE: %0x\n", Result);
            {
                printf("  POINTER_FLAG_NEW            %x\n", (Result & 0x00000001) != 0); // New pointer
                printf("  POINTER_FLAG_INRANGE        %x\n", (Result & 0x00000002) != 0); // Pointer has not departed
                printf("  POINTER_FLAG_INCONTACT      %x\n", (Result & 0x00000004) != 0); // Pointer is in contact
                printf("  POINTER_FLAG_PRIMARY        %x\n", (Result & 0x00002000) != 0); // Pointer is primary
                printf("  POINTER_FLAG_CONFIDENCE     %x\n", (Result & 0x00004000) != 0); // Pointer is considered unlikely to be accidental
                printf("  POINTER_FLAG_CANCELED       %x\n", (Result & 0x00008000) != 0); // Pointer is departing in an abnormal manner
                printf("  POINTER_FLAG_DOWN           %x\n", (Result & 0x00010000) != 0); // Pointer transitioned to down state (made contact)
                printf("  POINTER_FLAG_UPDATE         %x\n", (Result & 0x00020000) != 0); // Pointer update
                printf("  POINTER_FLAG_UP             %x\n", (Result & 0x00040000) != 0); // Pointer transitioned from down state (broke contact)
                printf("  POINTER_FLAG_CAPTURECHANGED %x\n", (Result & 0x00200000) != 0); // Lost capture
                printf("  POINTER_FLAG_HASTRANSFORM   %x\n", (Result & 0x00400000) != 0);
            }
        } break;
    }

    return(Result);
}
