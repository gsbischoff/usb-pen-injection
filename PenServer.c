#include "NetworkHeader.h"
#include "PenIO.h"

// -----
//   PenServer.c
//		This program implements the connection from the serving
//			computers using network sockets
int
main(int argc, char **argv)
{
	if(argc != 2)
		printf("Usage: %s <port>\n", argv[0]);

	int ServerSock;
	int ClientSock;
	struct sockaddr_in ServerAddr;
	struct sockaddr_in ClientAddr;
	int ServerPort;

	ServerPort = atoi(argv[1]);

	WSADATA data = { 0 };
	WORD wVersionRequested = MAKEWORD(1,1);

	if(WSAStartup(wVersionRequested, &data) < 0)
		DieWithError("WSAStartup() failed");

	if((ServerSock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) < 0)
		DieWithError("socket() failed");

	memset(&ServerAddr, 0, sizeof(ServerAddr));
	ServerAddr.sin_family		= AF_INET;
	ServerAddr.sin_addr.s_addr	= htonl(INADDR_ANY);
	ServerAddr.sin_port			= htons(ServerPort);

	if(bind(ServerSock, (struct sockaddr *) &ServerAddr, sizeof(ServerAddr)) < 0)
		DieWithError("bind() failed");

	//if(listen(ServerSock, 1) < 0)
	//	DieWithError("listen() failed");

	int ClientAddrLen = sizeof(ClientAddr);

	//if((ClientSock = accept(ServerSock, (struct sockaddr *) &ClientAddr, &ClientAddrLen)) < 0)
	//	DieWithError("accept() failed");

	//printf("Recieved connection from %s!\n", inet_ntoa(ClientAddr.sin_addr));

	printf("Press enter to begin...\n");

	getc(stdin);

	printf("Sending...\n");

	POINT Last = { 0 };

	for(;;)
	{
		// Get current pointer info
		POINT Point;
		GetCursorPos(&Point);

		// If the point has changed, send
		if(Last.x != Point.x || Last.y != Point.y)
		{
			//send(ClientSock, (char *) &Point, sizeof(POINT), 0);
			if(sendto(ServerSock, (char *) &Point, sizeof(Point), 0,
				(struct sockaddr *) &ClientAddr, sizeof(struct sockaddr)) < 0)
				DieWithError("send() failed");
			//printf(". ");
		}
		Last = Point;

		// Wait a bit before repeating...
		Sleep(1);
	}

	closesocket(ClientSock);
	closesocket(ServerSock);

	WSACleanup();
	return(0);
}