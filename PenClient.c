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

	if(WSAStartup(wVersionRequested, &data))
		DieWithError("WSAStartup() failed");

	if((sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0)
		DieWithError("sock() failed");

	memset(&ServerAddr, 0, sizeof(ServerAddr));
	ServerAddr.sin_family		= AF_INET;
	ServerAddr.sin_addr.s_addr	= inet_addr(ServerHost);
	ServerAddr.sin_port			= ServerPort;

	if(connect(sock, (struct sockaddr *) &ServerAddr, sizeof(ServerAddr)) < 0)
		DieWithError("connect() failed");

/*	for(;;)
	{
		char RecvBuffer[20];
		memset(RecvBuffer, 0, 20);
		if(recv(sock, RecvBuffer, 20, 0) <= 0)
			break;

		RecvBuffer[19] = '\0';
		printf("\'%s\'\n", RecvBuffer);
	}*/


	//int bunch[SENDTEST];
	int bunchsiz = SENDTEST * sizeof(int);
	int *bunch = malloc(bunchsiz);

//	if(recv(sock, (char *) &reciever, sizeof(reciever), 0) <= 0)
//		DieWithError("recv() failed");
	long long int count = 0;
	long long int temp = 0;
	long long int remain = bunchsiz;
	clock_t start, end;
	printf("Start of recieving\n");
	start = clock();

	for(;;)
	{
		if((temp = recv(sock, (char *) bunch, remain, 0)) <= 0)
			break;

		printf("Got %lld\n", temp);

		count += temp;
		remain -= temp;
	}

	end = clock();

	clock_t diff = end - start;
	printf("Recieved %lld MB in %.2f secs.\n", count / 1000000, ((double) diff) / CLOCKS_PER_SEC);

	free(bunch);
	closesocket(sock);

	WSACleanup();

	return(0);
}