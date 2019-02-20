#include "NetworkHeader.h"
#include <stdlib.h>  /* for atoi() */
#include <string.h>  /* for memset() */

#include <ws2ipdef.h>

// THE FOLLOWING CODE IS FROM DONAHOO, CALVERT - TCP/IP Sockets in C

void DieWithError(char *errorMessage); /* External error handling function */

int main(int argc, char *argv[])
{
    int sock;                           /* Socket */
    struct sockaddr_in multicastAddr;   /* Multicast address */
    char *multicastIP;                  /* IP Multicast address */
    unsigned short multicastPort;       /* Server port */
    char *sendString;                   /* String to multicast */
    unsigned char multicastTTL;         /* TTL of multicast packets */
    unsigned int sendStringLen;         /* Length of string to multicast */

    if ((argc < 4) || (argc > 5))  /* Test for correct number of parameters */
    {
        fprintf(stderr,"Usage:  %s <Multicast Address> <Port> <Send String> [<TTL>]\n", argv[0]);
        exit(1);
    }

    struct WSAData data = { 0 };
	WORD wVersionRequested = MAKEWORD(1,1);

    if(WSAStartup(wVersionRequested, &data))
		DieWithError("WSAStartup() failed");

    multicastIP = argv[1];          /* First arg' multicast IP address */
    multicastPort = atoi(argv[2]);  /* Second arg' multicast port */
    sendString = argv[3];           /* Third arg' string to multicast */
    
    if (argc == 5)                      /* Is TTL specified on command line? */
        multicastTTL = atoi(argv[4]);   /* Command line specified TTL */
    else
        multicastTTL = 1;               /* Default TTL = 1 */

    /* Create socket for sending/receiving datagrams */
    if ((sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) < 0)
        DieWithError(" socket () failed");

    /* Set TTL of multicast packet */
    if (setsockopt(sock, IPPROTO_IP, IP_MULTICAST_TTL, (void *) &multicastTTL,
        sizeof(multicastTTL)) < 0)
        DieWithError(" setsockopt () failed");

    /* Construct local address structure */
    memset(&multicastAddr, 0, sizeof(multicastAddr));   /* Zero out structure */
    multicastAddr.sin_family = AF_INET;                 /* Internet address family */
    multicastAddr.sin_addr.s_addr = inet_addr(multicastIP); /* Multicast IP address */
    multicastAddr.sin_port = htons(multicastPort);          /* Multicast port */

    sendStringLen = strlen(sendString);  /* Find length of sendString  */
    for (;;) /* Run forever */
    {
        printf("Sending one...\n");
        
        /* Multicast sendString in datagram to clients every 3 seconds */
        if (sendto(sock, sendString, sendStringLen, 0, (struct sockaddr *)
            &multicastAddr, sizeof(multicastAddr)) != sendStringLen)
            DieWithError("sendto() sent a different number of bytes than expected");

        Sleep(3000);
    }
    /* NOT REACHED */
}