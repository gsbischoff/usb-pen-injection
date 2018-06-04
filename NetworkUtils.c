#include "NetworkHeader.h"

void DieWithError(char *errorMessage)
{
	char *errorText = NULL;
	int err = WSAGetLastError();
	FormatMessage(
		FORMAT_MESSAGE_FROM_SYSTEM
		|FORMAT_MESSAGE_ALLOCATE_BUFFER
		|FORMAT_MESSAGE_IGNORE_INSERTS, 
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
unsigned long ResolveHost(char *hostName)
{
	struct hostent *host;
	if ((host = gethostbyname(hostName)) == NULL)
	{
		DieWithError("gethostbyname() failed");
		exit(1);
	}
	else
	{
		return(*((unsigned long *) host->h_addr_list[0]));
	}
}
