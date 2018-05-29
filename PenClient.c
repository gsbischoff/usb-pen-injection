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
	struct sockaddr_in FromAddr;	/* Local address */
	char *ServerHost;					/* Server address in dotted quad */
	char *ServerPortString;				/* Server port */
	unsigned short ServerPort; 		/* Server port */
	unsigned int clntLen;				/* Length of client address data structure */

	if(argc != 2)
	{
		printf("Usage: %s [<server IP>[:<port>]]", argv[0]);
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

	//if((sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0)
	if((sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) < 0)
		DieWithError("sock() failed");

	memset(&ServerAddr, 0, sizeof(ServerAddr));
	ServerAddr.sin_family		= AF_INET;
	ServerAddr.sin_addr.s_addr	= ResolveHost(ServerHost); //inet_addr(ServerHost);
	ServerAddr.sin_port			= htons(ServerPort);

	//if(connect(sock, (struct sockaddr *) &ServerAddr, sizeof(ServerAddr)) < 0)
	//	DieWithError("connect() failed");

	//POINTER_PEN_INFO recvBuffer = {0};

	POINT Point;

	// TODO: test recieving cursor movements over UDP, then change to sending the PEN_INFO struct
	for(;;)
	{
		Point.x = 0;
		Point.y = 0;

		int FromLen = sizeof(struct sockaddr);

		if(recvfrom(sock, (char *) &Point, sizeof(Point), 0,
			(struct sockaddr *) &FromAddr, &FromLen) < 0)
			DieWithError("recvfrom() failed");

		//if(recv(sock, (char *) &recvBuffer, sizeof(POINTER_PEN_INFO), 0) <= 0)
		//	break;

		//InjectTouch(recvBuffer);
		SetCursorPos(Point.x, Point.y);
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
		memcpy(&touchInfo.pointerInfo, &PenInfo.pointerInfo, sizeof(POINTER_INFO));

		// Don't have penFlag for eraser! Touch has no flags!
		touchInfo.touchMask = TOUCH_MASK_PRESSURE; // only injecting pressure
		touchInfo.pressure = PenInfo.pressure;

		InjectTouchInput(1, &touchInfo);
	}
}