#include "NetworkHeader.h"

// THE FOLLOWING CODE IS FROM DONAHOO, CALVERT - TCP/IP Sockets in C

#define MAXRECVSTRING 255 /* Longest string to receive */

void DieWithError(char *errorMessage);  /* External error handling function */

int main(int argc, char *argv[])
{
    int sock;                           /* Socket */
    struct sockaddr_in broadcastAddr;   /* Broadcast Address */
    unsigned int broadcastPort;         /* Port */
    char recvString[MAXRECVSTRING + 1]; /* Buffer for received string */
    int recvStringLen;                  /* Length of received string */

    if (argc != 2)      /* Test for correct number of arguments */
    {
        fprintf(stderr, "Usage: %s <Broadcast Port>\n", argv[0]);
        exit(1);
    }

    struct WSAData data = { 0 };
	WORD wVersionRequested = MAKEWORD(1,1);

    if(WSAStartup(wVersionRequested, &data))
		DieWithError("WSAStartup() failed");

    broadcastPort = atoi(argv[1]);  /* First arg: broadcast port */

    /* Create a best-effort datagram socket using UDP */
    if ((sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) < 0)
        DieWithError("socket() failed");

    /* Construct bind structure */
    memset(&broadcastAddr, 0, sizeof(broadcastAddr));   /* Zero out structure */
    broadcastAddr.sin_family = AF_INET;                 /* Internet address family */
    broadcastAddr.sin_addr.s_addr = htonl(INADDR_ANY);  /* Any incoming interface */
    broadcastAddr.sin_port = htons(broadcastPort);      /* Broadcast port */

    /* Bind to the broadcast port */
    if (bind(sock, (struct sockaddr *) &broadcastAddr, sizeof(broadcastAddr)) < 0)
        DieWithError("bind() failed");

    /* Receive a single datagram from the server */
    if ((recvStringLen = recvfrom(sock, recvString, MAXRECVSTRING, 0, NULL, 0)) < 0)
        DieWithError("recvfrom() failed");

    recvString[recvStringLen] = '\0';
    printf("Received: %s\n", recvString);

    closesocket(sock);
    exit(0);
}