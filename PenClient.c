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
			ServerPort = htons(atoi(ServerPortString));
	}

	printf("IP is %s, port is %d!\n", ServerHost, ntohs(ServerPort));

	struct WSAData data = { 0 };
	WORD wVersionRequested = MAKEWORD(1,1);

	printf("1...\n");

	if(WSAStartup(wVersionRequested, &data))
		DieWithError("WSAStartup() failed");

	if((sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0)
		DieWithError("sock() failed");

		printf("2...\n");

	memset(&ServerAddr, 0, sizeof(ServerAddr));
	ServerAddr.sin_family		= AF_INET;
	ServerAddr.sin_addr.s_addr	= inet_addr(ServerHost);
	ServerAddr.sin_port			= ServerPort;

	if(connect(sock, (struct sockaddr *) &ServerAddr, sizeof(ServerAddr)) < 0)
		DieWithError("connect() failed");
		printf("3...\n");

	POINT Point = { 0 };
	printf("About to recieve...\n");

	for(;;)
	{
		Point.x = 0;
		Point.y = 0;

		if(recv(sock, (char *) &Point, sizeof(POINT), 0) <= 0)
			break;

		SetCursorPos(Point.x, Point.y)
	}

	closesocket(sock);

	WSACleanup();

	return(0);
}