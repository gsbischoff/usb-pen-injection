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

    if((sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) < 0)
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
                default:
                {

                }
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

    printf("     SIZE    TIME     RATE\n");
    struct sockaddr_in fromAddr;
    int fromLen = sizeof(fromAddr);

    if(isServer)
    {
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
                if(sendto(sock, Buffer, ChunkSize, 0, (struct sockaddr *) &hostAddr, sizeof(struct sockaddr)) < 0)
                    DieWithError("sendto() failed");

                char Recv;
                if(recvfrom(sock, &Recv, 1, 0, (struct sockaddr *) &fromAddr, &fromLen) < 0)
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
        /* Client will recieve message, check size, and  send a response */
        int bytesRecieved;
        while((bytesRecieved = recvfrom(sock, Buffer, scanf(Buffer), 0,
               (struct sockaddr *) &fromAddr, &fromLen)) >= 0)
        {
            printf("Got message of %9u bytes!\n", bytesRecieved);

            if(sendto(sock, Buffer, 1, 0, (struct sockaddr *) &fromAddr, fromLen) < 0)
                DieWithError("sendto() failed");
        }
    }
    
}