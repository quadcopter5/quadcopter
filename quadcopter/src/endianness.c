/*
	endianness.c

	Functions to determine system endianness during run-time, and to facilitate
	swapping byte orders.

	Only supported byte orders are Big Endian and Little Endian.
*/

#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#include "endianness.h"

static Endian hostendian = ENDIAN_NOTDETERMINED;

Endian getHostEndian() {
	if (hostendian == ENDIAN_NOTDETERMINED) {
		// Establish endianness of the running system
		uint32_t test;
		char *set = (char *)&test;
		set[0] = 0x11;
		set[1] = 0x22;
		set[2] = 0x33;
		set[3] = 0x44;

		if (test == 0x11223344)
			hostendian = ENDIAN_BIG;
		else if (test == 0x44332211)
			hostendian = ENDIAN_LITTLE;
		else
			hostendian = ENDIAN_UNSUPPORTED;
	}

	return hostendian;
}

void swapEndian(void *dest, const void *src, size_t numbytes) {
	char   *tempsrc = (char *)malloc(numbytes),
	       *swapped = (char *)dest;
	size_t i;

	memcpy(tempsrc, src, numbytes);

	for (i = 0; i < numbytes; ++i)
		swapped[i] = tempsrc[numbytes - i - 1];

	free(tempsrc);
}

inline void hostToBE(void *dest, const void *src, size_t numbytes) {
	if (getHostEndian() == ENDIAN_BIG)
		memcpy(dest, src, numbytes);
	else
		swapEndian(dest, src, numbytes);
}

inline void hostToLE(void *dest, const void *src, size_t numbytes) {
	if (getHostEndian() == ENDIAN_LITTLE)
		memcpy(dest, src, numbytes);
	else
		swapEndian(dest, src, numbytes);
}

inline void BEToHost(void *dest, const void *src, size_t numbytes) {
	hostToBE(dest, src, numbytes);
}

inline void LEToHost(void *dest, const void *src, size_t numbytes) {
	hostToLE(dest, src, numbytes);
}

