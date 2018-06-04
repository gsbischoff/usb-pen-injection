//#include "NetworkHeader.h"
#include <windows.h>
#include <ws2bth.h>
#include <stdio.h>

void HandleSend(SOCKET *s, struct sockaddr_in *to);
DWORD WINAPI HandleRecieve(void *s);
void DieWithError(char *errorMessage);

// -----
//   BTHTest.c
//		This program is a test server and client 
//			using UDP datagram sockets
int
main(int argc, char **argv)
{
	WSADATA WSAData;
	WSAStartup(MAKEWORD(2, 2), &WSAData);

	SOCKET sock;
	SOCKADDR_BTH serverAddr;
	SOCKADDR_BTH clientAddr;

	/* Create a UDP socket */
	if((sock = socket(AF_BTH, SOCK_STREAM, BTHPROTO_RFCOMM)) < 0)
		DieWithError("socket() failed");

	/* Fill in our address */
	memset(&clientAddr, 0, sizeof(clientAddr));
	clientAddr.addressFamily = AF_BTH;
	clientAddr.btAddr = 0; // ResolveHost("localhost"); // laziness
	clientAddr.serviceClassId = GUID_NULL;
	clientAddr.port = 0; //BT_PORT_ANY;	// 0 for clients on bind, BT_PORT_ANY for server

	if(bind(sock, (struct sockaddr *) &clientAddr, sizeof(struct sockaddr)) < 0)
		DieWithError("bind() failed");
#if 0
	if(argc != 3)
	{
		printf("Usage: %s -s <host>", argv[0]);
		exit(1);
	}

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

	if(!serverHost)
	{
		printf("Usage: %s -s <host>", argv[0]);
		exit(1);
	}

	/* Fill in our other client's address */
	memset(&serverAddr, 0, sizeof(struct sockaddr_in));
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_addr.s_addr = ResolveHost(serverHost);	// laziness
	serverAddr.sin_port = htons(35001);						// Static port

	/* Create a thread to recieve all incoming messages */
	HANDLE recvThread = CreateThread(0, 0, HandleRecieve, &sock, 0, 0);

	HandleSend(&sock, &serverAddr);

	if(closesocket(sock))
		DieWithError("closesocket() failed");
#endif
	WSACleanup();
	
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

void DieWithError(char *errorMessage)
{
	char *errorText = NULL;
	int err = GetLastError();
	FormatMessage(
		FORMAT_MESSAGE_FROM_SYSTEM |
		FORMAT_MESSAGE_ALLOCATE_BUFFER |
		FORMAT_MESSAGE_IGNORE_INSERTS, 
		NULL,
		err,
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		(LPTSTR) &errorText,
		0,
		NULL);

	printf("%s: %s", errorMessage, errorText);
	LocalFree(errorText);
	errorText = NULL;
	exit(1);
}