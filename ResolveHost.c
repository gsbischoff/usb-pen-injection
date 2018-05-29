#include "NetworkHeader.h"

unsigned long ResolveHost(char *hostName)
{
	struct hostent *host;
	if ((host = gethostbyname(hostName)) == NULL)
	{
		DieWithError("gethostbyname() failed");
		//printf("gethostbyname() failed\n");
		exit(1);
	}
	else
	{
		return(*((unsigned long *) host->h_addr_list[0]));
	}
}