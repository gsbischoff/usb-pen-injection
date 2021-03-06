#include "NetworkHeader.h"
#include "PenIO.h"

int
initPoint(SOCKET s, struct sockaddr_in *c);

// -----
//   PenServer.c
//		This program implements the connection from the serving
//			computers using network sockets
int
main(int argc, char **argv)
{
	int ServerSock;
	//int ClientSock;
	struct sockaddr_in ServerAddr;
	struct sockaddr_in ClientAddr;
	char *ClientHost;					/* Server address in dotted quad */
	char *ClientPortString;				/* Server port */
	unsigned short ServerPort = 35000;
	unsigned short ClientPort;

	if(argc != 2)
	{
		printf("Usage: %s [<client Host>[:<port>]]", argv[0]);
		exit(1);
	}
	else
	{
		ClientHost = strtok(argv[1], ":");

		if((ClientPortString = strtok(NULL, ":")) != NULL)
			ClientPort = atoi(ClientPortString);
	}

	WSADATA data = { 0 };
	WORD wVersionRequested = MAKEWORD(1,1);

	if(WSAStartup(wVersionRequested, &data) < 0)
		DieWithError("WSAStartup() failed");

	memset(&ClientAddr, 0, sizeof(ClientAddr));
	ClientAddr.sin_family		= AF_INET;
	ClientAddr.sin_addr.s_addr	= ResolveHost(ClientHost);
	ClientAddr.sin_port			= htons(ClientPort);

	if((ServerSock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) < 0)
		DieWithError("socket() failed");

	memset(&ServerAddr, 0, sizeof(ServerAddr));
	ServerAddr.sin_family		= AF_INET;
	ServerAddr.sin_addr.s_addr	= htonl(INADDR_ANY);
	ServerAddr.sin_port			= htons(ServerPort);

	if(bind(ServerSock, (struct sockaddr *) &ServerAddr, sizeof(ServerAddr)) < 0)
		DieWithError("bind() failed");

	int ClientAddrLen = sizeof(ClientAddr);


	printf("Press enter to begin...\n");

	getc(stdin);

	printf("Sending...\n");

	initPoint(ServerSock, &ClientAddr);

	//POINT Last = { 0 };

/*	for(;;)
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
	}*/

	//closesocket(ClientSock);
	closesocket(ServerSock);

	WSACleanup();
	return(0);
}