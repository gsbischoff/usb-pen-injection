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
	printf("Length of struct: %zu %zu %zu %zu %zu %zu %zu %zu\n",
		sizeof(POINTER_INPUT_TYPE         ),
		sizeof(POINTER_FLAGS              ),
		sizeof(HANDLE                     ),
		sizeof(HWND                       ),
		sizeof(POINT                      ),
		sizeof(POINTER_BUTTON_CHANGE_TYPE ),
		sizeof(PEN_FLAGS),
		sizeof(PEN_MASK ));
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

	//POINT Point;


	// TODO: test recieving cursor movements over UDP, then change to sending the PEN_INFO struct
	for(;;)
	{
		//Point.x = 0;
		//Point.y = 0;

		fromLen = sizeof(FromAddr);

		if(recvfrom(sock, (char *) &recvBuffer, sizeof(recvBuffer), 0,
			(struct sockaddr *) &FromAddr, &fromLen) < 0)
			DieWithError("recvfrom() failed");


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
		memcpy(&touchInfo.pointerInfo, &PenInfo.pointerInfo, sizeof(POINTER_INFO));

		// Don't have penFlag for eraser! Touch has no flags!
		touchInfo.touchMask = TOUCH_MASK_PRESSURE; // only injecting pressure
		touchInfo.pressure = PenInfo.pressure;

		InjectTouchInput(1, &touchInfo);
	}
}