/*
	gpio.h

	GPIO Interface for Raspberry Pi (Broadcom 2835)
*/

#ifndef GPIO_H
#define GPIO_H

typedef enum {
	GPIO_MODE_IN = 0,
	GPIO_MODE_OUT
} GPIOMode;

typedef enum {
	LOW = 0,
	HIGH = 1
} GPIOValue;

/**
	Initialize GPIO functionality.
	This must be called before calling any other functions in this header!

	Returns 1 on success, 0 on error.
	Use gpio_getLastError() to get a description of the error.
*/
int gpio_init();

/**
	Properly close GPIO functionality.
	This should be called when GPIO is no longer needed.

	Returns 1 on success, 0 on error.
	Use gpio_getLastError() to get a description of the error.
*/
int gpio_deinit();

/**
	Set the current mode for the given pin.
	The pin number is the number on the BCM2835 board, which ranges from 0-53
	(54 total pins).

	Returns 1 on success, 0 on error.
*/
int gpio_setMode(unsigned int pin, GPIOMode mode);

/**
	Given that pin has been set to output mode, the pin is set to output
	either a LOW or HIGH value.

	Returns 1 on success, 0 on error.
*/
int gpio_write(unsigned int pin, GPIOValue val);

/**
	Given that pin has been set to input mode, the voltage on the pin is read
	into value (either LOW or HIGH).

	Returns 1 on success, 0 on error.
*/
int gpio_read(unsigned int pin, int *value);

/**
	Returns a string containing a descripiton of the last error. If no error
	has occurred since the last call to this function, then the string
	"No Error" is returned.
*/
const char *gpio_getLastError();

#endif

