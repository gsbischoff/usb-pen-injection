#include "NetworkHeader.h"
#include <ws2ipdef.h>

// THE FOLLOWING CODE IS FROM DONAHOO, CALVERT - TCP/IP Sockets in C

#define MAXRECVSTRING 255 /* Longest string to receive */

void DieWithError(char *errorMessage);  /* External error handling function */

int main(int argc, char *argv[])
{
    int sock;                           /* Socket */
    struct sockaddr_in multicastAddr;   /* Multicast Address */
    char *multicastIP;                  /* IP Multicast Address */
    unsigned int multicastPort;         /* Port */
    char recvString[MAXRECVSTRING + 1]; /* Buffer for received string */
    int recvStringLen;                  /* Length of received string */
    struct ip_mreq multicastRequest;    /* Multicast address join structure */

    if (argc != 3)         /* Test for correct number of arguments */
    {
        fprintf(stderr, "Usage: %s <Multicast IP> <Multicast Port>In", argv[0]);
        exit(1);
    }

    struct WSAData data = { 0 };
	WORD wVersionRequested = MAKEWORD(1,1);

    if(WSAStartup(wVersionRequested, &data))
		DieWithError("WSAStartup() failed");

    multicastIP = argv[1];          /* First arg' multicast IP address (dotted quad) */
    multicastPort = atoi(argv[2]);  /* Second arg' multicast port */

    /* Create a best-effort datagram socket using UDP */
    if ((sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) < 0)
        DieWithError("socket () failed");

    /* Construct bind structure */
    memset(&multicastAddr, 0, sizeof(multicastAddr));   /* Zero out structure */
    multicastAddr.sin_family = AF_INET;                 /* Internet address family */
    multicastAddr.sin_addr.s_addr = htonl(INADDR_ANY);  /* Any incoming interface */
    multicastAddr.sin_port = htons(multicastPort);      /* Multicast port */

    /*  Bind  to the  multicast port  */
    if (bind(sock, (struct sockaddr *) &multicastAddr, sizeof(multicastAddr)) < 0)
        DieWithError("bind() failed");

    /* Specify the multicast group */
    multicastRequest.imr_multiaddr.s_addr = inet_addr(multicastIP);

    /* Accept multicast from any interface */
    multicastRequest.imr_interface.s_addr = htonl(INADDR_ANY);

    /* Join the multicast group */
    if (setsockopt(sock, IPPROTO_IP, IP_ADD_MEMBERSHIP, (void *) &multicastRequest,
        sizeof(multicastRequest)) < 0)
        DieWithError("setsockopt() failed");

    /* Receive a single datagram from the server */
    if ((recvStringLen = recvfrom(sock, recvString, MAXRECVSTRING, 0, NULL, 0)) < 0)
        DieWithError("recvfrom() failed");

    recvString[recvStringLen] = '\0';
    printf("Received: %s\n", recvString);  /* Print the received string */

    closesocket(sock);
    exit(0);
}