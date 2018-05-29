#ifndef NETWORKHEADER_H
#define NETWORKHEADER_H


#include <stdio.h>      /* for printf() and fprintf() */
#include <ERRNO.H>
#include <Winsock2.h>
#include <windows.h>

#include <stdlib.h>     /* for atoi() */
#include <string.h>     /* for memset() */

#define SERVER_HOST "141.166.206.223"  /* wallis IP address */
#define SERVER_PORT "35001"

#define SA struct sockaddr

/* Miscellaneous constants */
#define	MAXLINE		4096	/* max text line length */
#define	MAXSOCKADDR  128	/* max socket address structure size */
#define	BUFFSIZE	8192	/* buffer size for reads and writes */
#define	LISTENQ		1024	/* 2nd argument to listen() */
#define SHORT_BUFFSIZE  100     /* For messages I know are short */
void DieWithError(char *errorMessage); /*Error handling function */
unsigned long ResolveHost(char *hostName);
void InjectTouch(POINTER_PEN_INFO PenInfo);

#define	SENDTEST	40000000

typedef struct 
{
	POINTER_INPUT_TYPE         pointerType;
	UINT32                     pointerId;
	UINT32                     frameId;
	POINTER_FLAGS              pointerFlags;
	//HANDLE                     sourceDevice; // these two fields are of different sizes between machines
	//HWND                       hwndTarget;
	UINT64                     sourceDevice; 
	UINT64                     hwndTarget;
	POINT                      ptPixelLocation;
	POINT                      ptHimetricLocation;
	POINT                      ptPixelLocationRaw;
	POINT                      ptHimetricLocationRaw;
	DWORD                      dwTime;
	UINT32                     historyCount;
	INT32                      inputData;
	DWORD                      dwKeyStates;
	UINT64                     PerformanceCount;
	POINTER_BUTTON_CHANGE_TYPE ButtonChangeType;
} POINTER_INFO_EX;

typedef struct
{
  POINTER_INFO pointerInfo;
  PEN_FLAGS    penFlags;
  PEN_MASK     penMask;
  UINT32       pressure;
  UINT32       rotation;
  INT32        tiltX;
  INT32        tiltY;
} POINTER_PEN_INFO_EX;

// Size of pointer info
#ifndef __PI
#define __PI  \
		sizeof(POINTER_INPUT_TYPE         )	+ \
		sizeof(UINT32                     ) + \
		sizeof(UINT32                     ) + \
		sizeof(POINTER_FLAGS              ) + \
		sizeof(HANDLE                     ) + \
		sizeof(HWND                       ) + \
		sizeof(POINT                      ) + \
		sizeof(POINT                      ) + \
		sizeof(POINT                      ) + \
		sizeof(POINT                      ) + \
		sizeof(DWORD                      ) + \
		sizeof(UINT32                     ) + \
		sizeof(INT32                      ) + \
		sizeof(DWORD                      ) + \
		sizeof(UINT64                     ) + \
		sizeof(POINTER_BUTTON_CHANGE_TYPE )

#define PPINFO __PI + \
		sizeof(PEN_FLAGS) + \
		sizeof(PEN_MASK ) + \
		sizeof(UINT32   ) + \
		sizeof(UINT32   ) + \
		sizeof(INT32    ) + \
		sizeof(INT32    )
#endif
#endif