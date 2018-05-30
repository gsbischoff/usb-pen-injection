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

unsigned char *serialize_field(unsigned char *destBuffer, void *source, size_t size, size_t align);
unsigned char *unserialize_field(unsigned char *srcBuffer, void *dest, size_t size, size_t align);
unsigned char *expand(POINTER_PEN_INFO *Info);
void compress(unsigned char *buffer, POINTER_PEN_INFO *Info);

#define serialize(a, b) serialize_field(a, &b, sizeof(b), 0)
#define pad(a, b) serialize_field(a, &b, sizeof(b), 8)

#define unserialize(a, b) unserialize_field(a, &b, sizeof(b), 0)
#define unpad(a, b) unserialize_field(a, &b, sizeof(b), 8)

#endif