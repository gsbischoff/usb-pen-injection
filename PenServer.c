#include "NetworkHeader.h"

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

	closesocket(ClientSock);
	closesocket(ServerSock);
	return(0);
}