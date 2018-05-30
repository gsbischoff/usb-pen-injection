#include "NetworkHeader.h"
#include <time.h>

// -----
//   PenClient.c
//		This program implements the connection between the two
//			computers using network sockets
int
main(int argc, char **argv)
{
	int sock;						/* Socket descriptor for client */
	struct sockaddr_in ServerAddr;	/* Local address */
	struct sockaddr_in ClientAddr;
	struct sockaddr_in FromAddr;
	char *ServerHost;					/* Server address in dotted quad */
	char *ServerPortString;				/* Server port */
	unsigned short ServerPort; 		/* Server port */
	unsigned int fromLen;				/* Length of client address data structure */

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
	ServerAddr.sin_family		= AF_INET;
	ServerAddr.sin_addr.s_addr	= ResolveHost(ServerHost); //inet_addr(ServerHost);
	ServerAddr.sin_port			= htons(ServerPort);

	if((sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) < 0)
		DieWithError("socket() failed");

	/* Fill in our address */
	memset(&ClientAddr, 0, sizeof(ClientAddr));
	ClientAddr.sin_family		= AF_INET;
	ClientAddr.sin_addr.s_addr	= htonl(INADDR_ANY); //inet_addr(ServerHost);
	ClientAddr.sin_port			= htons(ServerPort);

	if(bind(sock, (struct sockaddr *) &ClientAddr, sizeof(ClientAddr)) < 0)
		DieWithError("bind() failed");

	POINTER_PEN_INFO recvBuffer = {0};
	unsigned char expandedBuf[178];

	//POINT Point;


	// TODO: test recieving cursor movements over UDP, then change to sending the PEN_INFO struct
	for(;;)
	{
		//Point.x = 0;
		//Point.y = 0;

		fromLen = sizeof(FromAddr);

		if(recvfrom(sock, expandedBuf, sizeof(expandedBuf), 0,
			(struct sockaddr *) &FromAddr, &fromLen) < 0)
			DieWithError("recvfrom() failed");

		compress(expandedBuf, &recvBuffer);
		unsigned long long chk = 0;
		for(int i = 0; i < 178; ++i)
			chk += expandedBuf[i];

		printf("%llu  ", chk);
		//printf(".");

		if(FromAddr.sin_addr.s_addr == ServerAddr.sin_addr.s_addr)
			InjectTouch(recvBuffer);
			//SetCursorPos(Point.x, Point.y);
	}

	closesocket(sock);
	WSACleanup();

	return(0);
}

void
InjectTouch(POINTER_PEN_INFO PenInfo)
{
	if(InitializeTouchInjection(1, TOUCH_FEEDBACK_DEFAULT))
	{
		POINTER_TOUCH_INFO touchInfo = {0};

		//memcpy(&touchInfo.pointerInfo, &PenInfo.pointerInfo, sizeof(POINTER_INFO));
		touchInfo.pointerInfo.pointerType			 = PenInfo.pointerInfo.pointerType;				
		touchInfo.pointerInfo.pointerId				 = PenInfo.pointerInfo.pointerId;				
		touchInfo.pointerInfo.frameId				 = PenInfo.pointerInfo.frameId;					
		touchInfo.pointerInfo.pointerFlags			 = PenInfo.pointerInfo.pointerFlags;				
		touchInfo.pointerInfo.sourceDevice 			 = PenInfo.pointerInfo.sourceDevice; 			
		touchInfo.pointerInfo.hwndTarget			 = PenInfo.pointerInfo.hwndTarget;				
		touchInfo.pointerInfo.ptPixelLocation		 = PenInfo.pointerInfo.ptPixelLocation;			
		touchInfo.pointerInfo.ptHimetricLocation	 = PenInfo.pointerInfo.ptHimetricLocation;		
		touchInfo.pointerInfo.ptPixelLocationRaw	 = PenInfo.pointerInfo.ptPixelLocationRaw;		
		touchInfo.pointerInfo.ptHimetricLocationRaw  = PenInfo.pointerInfo.ptHimetricLocationRaw;	
		touchInfo.pointerInfo.dwTime				 = PenInfo.pointerInfo.dwTime;					
		touchInfo.pointerInfo.historyCount			 = PenInfo.pointerInfo.historyCount;				
		touchInfo.pointerInfo.InputData				 = PenInfo.pointerInfo.InputData;				
		touchInfo.pointerInfo.dwKeyStates			 = PenInfo.pointerInfo.dwKeyStates;				
		touchInfo.pointerInfo.PerformanceCount		 = PenInfo.pointerInfo.PerformanceCount;			
		touchInfo.pointerInfo.ButtonChangeType		 = PenInfo.pointerInfo.ButtonChangeType;			

		printf(",");
		// Don't have penFlag for eraser! Touch has no flags!
		touchInfo.touchMask = TOUCH_MASK_PRESSURE; // only injecting pressure
		touchInfo.pressure = PenInfo.pressure;

		if(!InjectTouchInput(1, &touchInfo))
		{
			DieWithError("InjectTouchInput() failed");
			exit(1);
		}
	}
}