#include "NetworkHeader.h"

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

	if((ServerSock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0)
		DieWithError("socket() failed");

	memset(&ServerAddr, 0, sizeof(ServerAddr));
	ServerAddr.sin_family		= AF_INET;
	ServerAddr.sin_addr.s_addr	= htonl(INADDR_ANY);
	ServerAddr.sin_port			= htons(ServerPort);

	if(bind(ServerSock, (struct sockaddr *) &ServerAddr, sizeof(ServerAddr)) < 0)
		DieWithError("bind() failed");

	if(listen(ServerSock, 1) < 0)
		DieWithError("listen() failed");

	int ClientAddrLen = sizeof(ClientAddr);

	if((ClientSock = accept(ServerSock, (struct sockaddr *) &ClientAddr, &ClientAddrLen)) < 0)
		DieWithError("accept() failed");

	printf("Recieved connection from %s!\n", inet_ntoa(ClientAddr.sin_addr));


	//char SendBuffer[20];
	struct test foo = { 
		.a = 'a',
		.b = 'b',
		.c = 'c',
		.d = 'd',
		.e = 'e',
		.f = 'f',
		.g = 'g',
		.h = 'h',
	};
	//printf("b is %c", foo.b);
	//int bunch[SENDTEST];
	int bunchsiz = SENDTEST * sizeof(int);
	int *bunch = malloc(bunchsiz);
	printf("Bunch is %d MB\n", bunchsiz / 1000000);

	send(ClientSock, (char *) bunch, bunchsiz, 0);
/*	for(;;)
	{
		memset(SendBuffer, 0, 20);
		fgets(SendBuffer, 20, stdin);
		SendBuffer[strlen(SendBuffer)] = '\0';
		printf("Sending \"%s\"", SendBuffer);
		send(ClientSock, SendBuffer, strlen(SendBuffer), 0);
	}*/
	free(bunch);
	closesocket(ClientSock);
	closesocket(ServerSock);

	WSACleanup();
	return(0);
}