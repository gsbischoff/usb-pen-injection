#include "NetworkHeader.h"

// -----
//   UDPTest.c
//		This program is a test server and client 
//			using UDP datagram sockets
int
main(int argc, char **argv)
{
	WSADATA WSAData;
	WSAStartup(MAKEWORD(2, 2), &WSAData);
	
	int sock;
	struct sockaddr_in serverAddr;
	struct sockaddr_in clientAddr;

	/* Create a UDP socket */
	if((sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) < 0)
		DieWithError("socket() failed");

	if(argc != 3)
		return(0);

	if(argv[1][0] == '-')
	{
		switch(argv[1][1])
		{
			case 's':
			case 'r':
			default:
		}
	}
	
	return(0);
}

void
HandleSend(SOCKET s)
{

}

void
HandleRecieve(SOCKET s)
{

}