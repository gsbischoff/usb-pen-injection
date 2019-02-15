#include "NetworkHeader.h"
#include <time.h>

char Buffer[200ULL * 1024ULL * 1024ULL];

int
main(int ArgCount, char **Args)
{
    WSADATA Data;
    WSAStartup(MAKEWORD(1,1), &Data);

    int sock;
    int isServer = 0;
    char *hostIP;

    if((sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0)
        DieWithError("socket() failed");

    struct sockaddr_in clientAddr;
    memset(&clientAddr, 0, sizeof(struct sockaddr_in));
    clientAddr.sin_family = AF_INET;                 /* Internet address family */
    clientAddr.sin_addr.s_addr = htonl(INADDR_ANY);  /* Any incoming interface */
    clientAddr.sin_port = htons(2020);               /* Port */

    for(int Index = 1;
        Index < ArgCount;
        ++Index)
    {
        if(Args[Index][0] == '-')
        {
            switch(Args[Index][1])
            {
                case 's':
                {
                    isServer = 1;
                } break;

                case 'h':
                {
                    if(ArgCount > Index + 1)
                    {
                        hostIP = Args[Index + 1];
                        printf("HostIP: %s\n", hostIP);
                    }
                    else
                    {
                        exit(1);
                    }                    
                } break;
            }
        }
    }
    if(bind(sock, (struct sockaddr *) &clientAddr, sizeof(struct sockaddr_in)) < 0)
        DieWithError("bind() failed");

    struct sockaddr_in hostAddr;
    memset(&clientAddr, 0, sizeof(struct sockaddr_in));
    hostAddr.sin_family = AF_INET;                 /* Internet address family */
    hostAddr.sin_addr.s_addr = inet_addr(hostIP);  /* Any incoming interface */
    hostAddr.sin_port = htons(2020);               /* Port */

    struct sockaddr_in fromAddr;
    int fromLen = sizeof(fromAddr);

    if(isServer)
    {
        if(listen(sock, 5) < 0)
            DieWithError("listen() failed");

        SOCKET connectedHost;
        if(connectedHost = accept(sock, (struct sockaddr *) &fromAddr, &fromLen))
            DieWithError("accept() failed");

        printf("     SIZE    TIME     RATE\n");
        clock_t Start, End;
        /* Server will send chunks of increasing size and wait for a acknowledgement Rate.exe -s 172.29.178.1 */
        for(long long ChunkSize = 1;
            ChunkSize < (100ULL * 1024ULL * 1024ULL);
            ChunkSize *= 2)
        {
            printf("%9llu ", ChunkSize);
            Start = clock();
            /* Send a few times each */
            for(int Index = 0;
                Index < 10;
                ++Index)
            {
                if(send(connectedHost, Buffer, ChunkSize, 0) < 0)
                    DieWithError("sendto() failed");

                char Recv;
                if(recv(connectedHost, &Recv, 1, 0) < 0)
                    DieWithError("recvfrom() failed");
            }
            End = clock();

            float Time = (((float)End - (float)Start) / 10000.f);
            
            printf("%3.3f ", Time);
            printf("%3.3f \n", ((float)ChunkSize / 1000000.f) / Time);
        }
    }
    else
    {
        printf("Starting client portion...\n");
        if(connect(sock, (struct sockaddr *) &hostAddr, sizeof(struct sockaddr)) < 0)
            DieWithError("connect() failed");

        /* Client will recieve message, check size, and  send a response */
        int bytesRecieved;
        for(long long ChunkSize = 1;
            ChunkSize < (100ULL * 1024ULL * 1024ULL);
            ChunkSize *= 2)
        {
            printf("%9llu ", ChunkSize);
            long long totalBytesRecv = 0;

            /* Send a few times each */
            for(int Index = 0;
                Index < 10;
                ++Index)
            {
                while((bytesRecieved = recv(sock, Buffer, sizeof(Buffer), 0)) >= 0)
                {
                    totalBytesRecv += bytesRecieved;

                    if(totalBytesRecv >= ChunkSize) break;
                }

                if(bytesRecieved < 0)
                    DieWithError("recv() failed");
                    
                printf("Got message of %9llu bytes!\n", totalBytesRecv);

                if(send(sock, Buffer, 1, 0) < 0)
                    DieWithError("send() failed");
            }
        }

    }
    
}