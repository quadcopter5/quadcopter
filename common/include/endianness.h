/*
	endianness.h

	Functions to determine system endianness during run-time, and to facilitate
	swapping byte orders.

	Only supported byte orders are Big Endian and Little Endian.
*/

#ifndef ENDIAN_H
#define ENDIAN_H

/**
	Endian enumeration - type returned by getHostEndian()
	Value -2 is reserved for "not yet determined"
*/
typedef enum _Endian {
	ENDIAN_NOTDETERMINED = -2,

	ENDIAN_UNSUPPORTED   = -1,
	ENDIAN_LITTLE        = 0,
	ENDIAN_BIG           = 1
} Endian;

/**
	Returns the endianness of the running system.
	
	This function will only run the endian test code the first time the
	function is run within the program. From then on, it will return the
	previously determined endianness to avoid unnecessary calculations.
*/
Endian getHostEndian();

/**
	Reverse the byte order of src, storing the result into dest.

	The function is written such that src and dest can refer to overlapping or
	the same memory regions. For example,

		uint32_t num;
		swapEndian(&num, &num, 4);

	will correctly result in num holding the reversed bytes of the original
	value of num.
*/
void swapEndian(void *dest, const void *src, size_t numbytes);

/**
	Convert src from host endianness to Big Endian, storing the result in dest.
*/
void hostToBE(void *dest, const void *src, size_t numbytes);

/**
	Convert src from host endianness to Little Endian, storing the result in
	dest.
*/
void hostToLE(void *dest, const void *src, size_t numbytes);

/**
	Convert src from Big Endian to host endianness, storing the result in dest.
*/
void BEToHost(void *dest, const void *src, size_t numbytes);

/**
	Convert src from Little Endian to host endianness, storing the result in
	dest.
*/
void LEToHost(void *dest, const void *src, size_t numbytes);

#endif

