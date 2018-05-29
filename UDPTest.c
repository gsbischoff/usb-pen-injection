#include "NetworkHeader.h"

void
HandleSend(SOCKET *s, struct sockaddr_in *to);
DWORD WINAPI
HandleRecieve(void *s);

// -----
//   UDPTest.c
//		This program is a test server and client 
//			using UDP datagram sockets
int
main(int argc, char **argv)
{
	WSADATA WSAData;
	WSAStartup(MAKEWORD(2, 2), &WSAData);

	SOCKET sock;
	struct sockaddr_in serverAddr;
	struct sockaddr_in clientAddr;

	/* Create a UDP socket */
	if((sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) < 0)
		DieWithError("socket() failed");

	/* Fill in our address */
	memset(&clientAddr, 0, sizeof(clientAddr));
	clientAddr.sin_family = AF_INET;
	clientAddr.sin_addr.s_addr = htonl(INADDR_ANY); // ResolveHost("localhost"); // laziness
	clientAddr.sin_port = htons(35001);		// Static port

	if(bind(sock, (struct sockaddr *) &clientAddr, sizeof(struct sockaddr)) < 0)
		DieWithError("bind() failed");

	if(argc != 3)
		return(0);

	char *serverHost = NULL;

	for(int i = 0; i < argc; ++i)
	{
		if(argv[i][0] == '-')
		{
			switch(argv[i][1])
			{
				case 's':
					serverHost = argv[i + 1];
					break;
				default:
					break;
			}
		}
	}

	/* Fill in our other client's address */
	memset(&serverAddr, 0, sizeof(struct sockaddr_in));
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_addr.s_addr = ResolveHost("localhost");//(serverHost ? ResolveHost(serverHost) : ResolveHost("localhost"));	// laziness
	serverAddr.sin_port = htons(35001);						// Static port

	/* Create a thread to recieve all incoming messages */
	HANDLE recvThread = CreateThread(0, 0, HandleRecieve, &sock, 0, 0);

	HandleSend(&sock, &serverAddr);
	
	return(0);
}

void
HandleSend(SOCKET *s, struct sockaddr_in *to)
{
	struct sockaddr_in fromAddr;
	int fromLen = sizeof(struct sockaddr);
	char sendBuffer[256];

	for(;;)
	{
		if(!fgets(sendBuffer, sizeof(sendBuffer), stdin))
			exit(1);

		if(sendto(*s, sendBuffer, strlen(sendBuffer), 0, (struct sockaddr *) 
				to, sizeof(struct sockaddr_in)) < 0)
				DieWithError("sendto() failed");
	}
}

DWORD WINAPI
HandleRecieve(void *s)
{
	SOCKET sock = *((SOCKET *)s);
	struct sockaddr_in fromAddr;
	int fromLen = sizeof(struct sockaddr);
	char recvBuffer[256];
	int recvMsgSize;

	for(;;)
	{
		fromLen = sizeof(struct sockaddr);
		if((recvMsgSize = recvfrom(sock, recvBuffer, sizeof(recvBuffer), 0, 
			(struct sockaddr *) &fromAddr, &fromLen)) < 0)
			DieWithError("recvfrom() failed");

		recvBuffer[recvMsgSize] = '\0';

		printf("%s: %s", inet_ntoa(fromAddr.sin_addr), recvBuffer);
	}
}