/*
	gpio.c

	GPIO Interface for Raspberry Pi (Broadcom 2835)
*/

#include <stdint.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <string.h>

#include "gpio.h"

// Size of page
#define BCM2835_PAGE_SIZE  (4*1024)
// Size of memory block
#define BCM2835_BLOCK_SIZE (4*1024)

#define GPIO_BASE          0x20200000

#define GPIO_OFFSET_FSEL0  0x00000000
#define GPIO_OFFSET_FSEL1  0x00000004
#define GPIO_OFFSET_FSEL2  0x00000008
#define GPIO_OFFSET_FSEL3  0x0000000C
#define GPIO_OFFSET_FSEL4  0x00000010
#define GPIO_OFFSET_FSEL5  0x00000014

#define GPIO_OFFSET_SET0   0x0000001C
#define GPIO_OFFSET_SET1   0x00000020

#define GPIO_OFFSET_CLR0   0x00000028
#define GPIO_OFFSET_CLR1   0x0000002C

#define GPIO_OFFSET_LVL0   0x00000034
#define GPIO_OFFSET_LVL1   0x00000038

// Internal error handling data
#define GPIO_ERRSIZE 128
static int error = 0;
static char error_str[GPIO_ERRSIZE];

// Pointer to the base of the memory map to GPIO device space
static volatile uint8_t *gpio_base = 0;

static void generateError(const char *str) {
	error = 1;
	strncpy(error_str, str, GPIO_ERRSIZE);
}

int gpio_init() {
	int memfd = -1;
	memfd = open("/dev/mem", O_RDWR | O_SYNC);
	if (memfd < 0) {
		generateError("gpio_init: Failed to open /dev/mem");
		return 0;
	}

	// Create mapping to device memory, so we have permission from the kernel
	gpio_base = (volatile uint8_t *)mmap(NULL, BCM2835_BLOCK_SIZE,
			PROT_READ | PROT_WRITE, MAP_SHARED, memfd, GPIO_BASE);
	if (gpio_base == MAP_FAILED) {
		generateError("gpio_init: Failed to create memory mapping for GPIO_BASE");
		return 0;
	}

	// Don't need the file descriptor to memory anymore
	close(memfd);

	return 1;
}

int gpio_deinit() {
	uint8_t *gpio_temp = (uint8_t *)gpio_base;
	if (munmap(gpio_temp, BCM2835_BLOCK_SIZE) < 0) {
		generateError("gpio_deinit: Failed to unmap GPIO_BASE");
		return 0;
	} else {
		gpio_base = 0;
		return 1;
	}
}

int gpio_setMode(unsigned int pin, GPIOMode mode) {
	if (pin <= 53) {
		int fsel_offset = (pin / 10) * 4;
		int offset = (pin % 10) * 3;
		int i;
		uint32_t *fsel = (uint32_t *)(gpio_base + fsel_offset);

		switch (mode) {
			case GPIO_MODE_OUT:{
				uint32_t current = *fsel;
				*fsel = (~(0x7 << offset) & current) | ((uint32_t)01 << offset);
			 }	break; 

			case GPIO_MODE_IN:{
				uint32_t current = *fsel;
				*fsel = (~(0x7 << offset) & current); 
			}	break;

			default:
				generateError("gpio_setMode: Unknown GPIOMode requested\n");
				return 0;
				break;
		}

		return 1;
	} else {
		generateError("gpio_setMode: Bad pin number requested");
		return 0;
	}
}

int gpio_write(unsigned int pin, GPIOValue val) {
	if (pin <= 54) {
		switch (val) {
			case LOW: {
				int clr_offset = GPIO_OFFSET_CLR0 + (pin / 32) * 4;
				int offset = pin % 32;
				uint32_t *clr = (uint32_t *)(gpio_base + clr_offset);
				*clr = (uint32_t)1 << offset;
			}	break;

			case HIGH: {
				int set_offset = GPIO_OFFSET_SET0 + (pin / 32) * 4;
				int offset = pin % 32;
				uint32_t *set = (uint32_t *)(gpio_base + set_offset);
				*set = (uint32_t)1 << offset;
			}	break;

			default:
				generateError("gpio_write: Unsupported GPIOValue\n");
				return 0;
				break;
		}
		return 1;
	} else {
		generateError("gpio_write: Bad pin number requested");
		return 0;
	}
}

int gpio_read(unsigned int pin, int *value) {
	if (pin <= 53) {
		uint32_t *lvloffset = (uint32_t *) (gpio_base + GPIO_OFFSET_LVL0 + (pin / 32) * 4);
		uint32_t val = *lvloffset;
		uint32_t dummy = *lvloffset;
		val = (val >> (pin % 32)) & 0x1;
		*value = val;
	} else {
		generateError("gpio_read: Bad pin number requested");
		return 0;
	}
}

const char *gpio_getLastError() {
	if (error) {
		error = 0;
		return error_str;
	} else
		return "No error";
}

