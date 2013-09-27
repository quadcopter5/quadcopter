/*
	test_endian.c

	Test for Endian detection (endian.c)
*/

#include <stdio.h>
#include <string.h>
#include <stdint.h>

#include "endianness.h"

int main(int argc, char **argv) {
	uint32_t number  = 0x55667788;
	uint16_t number2 = 0x2255,
	         temp;
	Endian end;

	printf("Value of number before swapEndian(): 0x%08X\n", number);
	swapEndian(&number, &number, sizeof(number));
	printf("Value of number after swapEndian():  0x%08X\n", number);

	printf("\n");

	end = getHostEndian();
	switch (end) {
		case ENDIAN_BIG:
			printf("Host is Big Endian (value %d)\n", end);
			break;
		case ENDIAN_LITTLE:
			printf("Host is Little Endian (value %d)\n", end);
			break;
		case ENDIAN_UNSUPPORTED:
			printf("Host is Unsupported Endian (value %d)\n", end);
			break;
	}

	printf("Value of number2: 0x%04x\n", number2);

	hostToBE(&temp, &number2, sizeof(number2));
	printf("  Result of hostToBE(): 0x%04X\n", temp);
	hostToLE(&temp, &number2, sizeof(number2));
	printf("  Result of hostToLE(): 0x%04X\n", temp);
	BEToHost(&temp, &number2, sizeof(number2));
	printf("  Result of BEToHost(): 0x%04X\n", temp);
	LEToHost(&temp, &number2, sizeof(number2));
	printf("  Result of LEToHost(): 0x%04X\n", temp);

	printf("Done!\n");
	return 0;
}

