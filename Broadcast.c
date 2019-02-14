#include "NetworkHeader.h"

void
HandleSend(SOCKET *s, struct sockaddr_in *to);
DWORD WINAPI
HandleRecieve(void *s);

// -----
//   Broadcast.c
//        This program is a test server and client using
//             UDP datagram sockets in broadcast mode
int
main(int argc, char **argv)
{
    WSADATA WSAData;
    WSAStartup(MAKEWORD(2, 2), &WSAData);

    SOCKET sock;
    struct sockaddr_in serverAddr;
    struct sockaddr_in clientAddr;
    int isServer = 0;

    /* Create a UDP socket */
    if((sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) < 0)
        DieWithError("socket() failed");

    /* Set socket to broadcast */
    int broadcastPermission = 1;
    if(setsockopt(sock, SOL_SOCKET, SO_BROADCAST, (void *) &broadcastPermission,
                  sizeof(broadcastPermission)) < 0)
        DieWithError("setsockopt() failed");

    /* Fill in our address */
    memset(&clientAddr, 0, sizeof(clientAddr));
    clientAddr.sin_family = AF_INET;
    clientAddr.sin_addr.s_addr = htonl(INADDR_ANY); // ResolveHost("localhost"); // laziness
    clientAddr.sin_port = htons(35001);        // Static port

    if(bind(sock, (struct sockaddr *) &clientAddr, sizeof(struct sockaddr)) < 0)
        DieWithError("bind() failed");

    for(int i = 0; i < argc; ++i)
    {
        if(argv[i][0] == '-')
        {
            switch(argv[i][1])
            {
                case 's':
                    isServer = 1;
                    break;
                default:
                    break;
            }
        }
    }

    struct sockaddr_in MatchedAddr = HandleInit(sock, isServer);

    char *MatchedStr = inet_ntoa(MatchedAddr.sin_addr);
    printf("Matched with host %s\n", MatchedStr);

    /* Create a thread to recieve all incoming messages */
    //HANDLE recvThread = CreateThread(0, 0, HandleRecieve, &sock, 0, 0);

    // HandleSend(&sock, &serverAddr);

    if(closesocket(sock))
        DieWithError("closesocket() failed");

    WSACleanup();
    
    return(0);
}

struct sockaddr_in
HandleInit(SOCKET sock, int isServer)
{
    struct sockaddr_in fromAddr = {0};
    int fromLen = sizeof(fromAddr);
    if(isServer)
    {
        char recvBuf[2];
        /* Server will wait for the broadcast signal */
        if(recvfrom(sock, recvBuf, sizeof(recvBuf), 0, &fromAddr, &fromLen) < 0)
            DieWithError("recvfrom() failed");

        if(recvBuf[0] == 0xFE && recvBuf[1] == 0xEF)
        {
            /* Give a response */
            if(sendto(sock, recvBuf, sizeof(recvBuf), 0, &fromAddr, sizeof(fromAddr)) < 0)
                DieWithError("sendto() failed");
            
            return fromAddr;
        }
    }
    else
    {
        struct sockaddr_in broadcastAddr;
        /* Fill in the broadcast address */
        memset(&broadcastAddr, 0, sizeof(struct sockaddr_in));
        broadcastAddr.sin_family = AF_INET;
        broadcastAddr.sin_addr.s_addr = inet_addr("172.255.255.255");    // laziness
        broadcastAddr.sin_port = htons(35001);                            // Static port

        char recvBuf[2] = { 0xFE, 0xEF };
        /* Client will send handshake message */
        if(sendto(sock, recvBuf, sizeof(recvBuf), 0, &broadcastAddr, sizeof(broadcastAddr)) < 0)
            DieWithError("sendto() failed");
        
        if(recvBuf[0] == 0xFE && recvBuf[1] == 0xEF)
        {
            /* Give a response */
            if(recvfrom(sock, recvBuf, sizeof(recvBuf), 0, &fromAddr, &fromLen) < 0)
                DieWithError("recvfrom() failed");
            
            return fromAddr;
        }
    }
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