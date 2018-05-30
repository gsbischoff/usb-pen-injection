#include "NetworkHeader.h"

// -----
//   Compand.c
//		This program pads and changes into network order
//			the fields of a POINTER_PEN_INFO struct


// -----
//   serialize_field
//		Serially places 'size' bytes from 'source' into dest
//			and pads the result by 'align' bytes
unsigned char *
serialize_field(unsigned char *destBuffer, void *source, size_t size, size_t align)
{
	if(!align)
		align = size;

	// Only primitives
	char *cVal = source;

	for(int byte = 0; byte < size; ++byte)
		destBuffer[byte] = cVal[byte];

	return destBuffer + align;
}

unsigned char *
unserialize_field(unsigned char *srcBuffer, void *dest, size_t size, size_t align)
{				// Take from here ^^^^^, put  ^^^^, this big ^^, aligned ^^^^
	if(!align)
		align = size;

	// Only primitives
	char *cVal = dest; // (char *)

	for(int byte = 0; byte < size; ++byte)
		cVal[byte] = srcBuffer[byte];

	return srcBuffer + align;
}

void
compress(unsigned char *buffer, POINTER_PEN_INFO *Info)
{
	unsigned char *ptr;
	if(!buffer)
		return;

	if(buffer[0] != 0xff || buffer[1] != 0x00)
	{
		printf("check bits not set correctly! %d and %d\n", buffer[0], buffer[1]);
		return;
	}

	ptr = buffer + 2;

	// Embedded PEN_INFO struct
	ptr = unpad(ptr, Info->pointerInfo.pointerType);
	ptr = unpad(ptr, Info->pointerInfo.pointerId);
	ptr = unpad(ptr, Info->pointerInfo.frameId);
	ptr = unpad(ptr, Info->pointerInfo.pointerFlags);
	ptr = unpad(ptr, Info->pointerInfo.sourceDevice); // these two fields are of different sizes between machines
	ptr = unpad(ptr, Info->pointerInfo.hwndTarget);
	ptr = unpad(ptr, Info->pointerInfo.ptPixelLocation);
	ptr = unpad(ptr, Info->pointerInfo.ptHimetricLocation);
	ptr = unpad(ptr, Info->pointerInfo.ptPixelLocationRaw);
	ptr = unpad(ptr, Info->pointerInfo.ptHimetricLocationRaw);
	ptr = unpad(ptr, Info->pointerInfo.dwTime);
	ptr = unpad(ptr, Info->pointerInfo.historyCount);
	ptr = unpad(ptr, Info->pointerInfo.InputData);
	ptr = unpad(ptr, Info->pointerInfo.dwKeyStates);
	ptr = unpad(ptr, Info->pointerInfo.PerformanceCount);
	ptr = unpad(ptr, Info->pointerInfo.ButtonChangeType);

	// Remainder of POINTER_PEN_INFO
	ptr = unpad(ptr, Info->penFlags);
	ptr = unpad(ptr, Info->penMask);
	ptr = unpad(ptr, Info->pressure);
	ptr = unpad(ptr, Info->rotation);
	ptr = unpad(ptr, Info->tiltX);
	ptr = unpad(ptr, Info->tiltY);
}

unsigned char *
expand(POINTER_PEN_INFO *Info)
{
	// Since we are
	// 22 fields, 8 bytes each = 176 bytes + 2 bytes for 'byte order check field'
	unsigned char *buf = malloc(178); //buf[178];
	unsigned char *ptr;

	buf[0] = 0xff;
	buf[1] = 0x00;
	ptr = buf + 2;

	// Embedded PEN_INFO struct
	ptr = pad(ptr, Info->pointerInfo.pointerType);
	ptr = pad(ptr, Info->pointerInfo.pointerId);
	ptr = pad(ptr, Info->pointerInfo.frameId);
	ptr = pad(ptr, Info->pointerInfo.pointerFlags);
	ptr = pad(ptr, Info->pointerInfo.sourceDevice); // these two fields are of different sizes between machines
	ptr = pad(ptr, Info->pointerInfo.hwndTarget);
	ptr = pad(ptr, Info->pointerInfo.ptPixelLocation);
	ptr = pad(ptr, Info->pointerInfo.ptHimetricLocation);
	ptr = pad(ptr, Info->pointerInfo.ptPixelLocationRaw);
	ptr = pad(ptr, Info->pointerInfo.ptHimetricLocationRaw);
	ptr = pad(ptr, Info->pointerInfo.dwTime);
	ptr = pad(ptr, Info->pointerInfo.historyCount);
	ptr = pad(ptr, Info->pointerInfo.InputData);
	ptr = pad(ptr, Info->pointerInfo.dwKeyStates);
	ptr = pad(ptr, Info->pointerInfo.PerformanceCount);
	ptr = pad(ptr, Info->pointerInfo.ButtonChangeType);

	// Remainder of POINTER_PEN_INFO
	ptr = pad(ptr, Info->penFlags);
	ptr = pad(ptr, Info->penMask);
	ptr = pad(ptr, Info->pressure);
	ptr = pad(ptr, Info->rotation);
	ptr = pad(ptr, Info->tiltX);
	ptr = pad(ptr, Info->tiltY);

	//printf("ptr is %p\n", (void *) (ptr - buf));
	return(buf);
}

/*
int
main()
{
	POINTER_PEN_INFO pen = {0};
	pen.pressure = 500;
	printf("Pressure is at %u [initially]\n", pen.pressure);

	unsigned char *buf = expand(&pen);

	POINTER_PEN_INFO outPen = {0};

	compress(buf, &outPen);

	printf("Pressure is at %u\n", outPen.pressure);


	free(buf);
	return(0);
}*/
