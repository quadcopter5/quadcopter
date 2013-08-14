/**
	Philip Romano
	Tests for GPIO
*/

#include <stdio.h>
#include "gpio.h"

static void ignoreInput();

int main(int argc, char **argv) {
	if (!gpio_init()) {
		fprintf(stderr, "%s\n", gpio_getLastError());
		return 1;
	}
	printf("GPIO initialized\n");

	/*printf("Select pin number: ");
	scanf("%d", &pin);
	ignoreInput();*/

	if (!gpio_setMode(17, GPIO_MODE_OUT)
			|| !gpio_setMode(18, GPIO_MODE_OUT)
			|| !gpio_setMode(27, GPIO_MODE_OUT)
			|| !gpio_setMode(22, GPIO_MODE_OUT)
			|| !gpio_setMode(23, GPIO_MODE_OUT)
			|| !gpio_setMode(24, GPIO_MODE_OUT)
			|| !gpio_setMode(25, GPIO_MODE_OUT)
			|| !gpio_setMode(4, GPIO_MODE_OUT)) {
		fprintf(stderr, "%s\n", gpio_getLastError());
		return 1;
	}
	printf("GPIO pins set to output\n");

	if (!gpio_write(17,HIGH)
			|| !gpio_write(18, HIGH)
			|| !gpio_write(27, HIGH)
			|| !gpio_write(22, HIGH)
			|| !gpio_write(23, HIGH)
			|| !gpio_write(24, HIGH)
			|| !gpio_write(25, HIGH)
			|| !gpio_write(4, HIGH)) {
		fprintf(stderr, "%s\n", gpio_getLastError());
		return 1;
	}
	printf("GPIO pins should now have HIGH voltage");

	ignoreInput();

	if (!gpio_write(17, LOW)
			|| !gpio_write(18, LOW)
			|| !gpio_write(27, LOW)
			|| !gpio_write(22, LOW)
			|| !gpio_write(23, LOW)
			|| !gpio_write(24, LOW)
			|| !gpio_write(25, LOW)
			|| !gpio_write(4, LOW)) {
		fprintf(stderr, "%s\n", gpio_getLastError());
		return 1;
	}
	printf("GPIO pins should now have LOW voltage\n");

	int pin;
	for (pin = 22; pin < 25; ++pin){
		if (!gpio_setMode(pin, GPIO_MODE_IN)){
			fprintf(stderr, "%s\n", gpio_getLastError());		  
			return 1;
		}	
	}

	printf("Pins set to input mode\n");
	ignoreInput();

	int val;	
	for (pin = 22; pin < 25; ++pin) {
		int i;
		for (i = 0; i < 10000; ++i) {
			if (!gpio_read(pin, &val)) {
				fprintf(stderr, "%s\n", gpio_getLastError());
				return 1;
			}
			printf("value for pin %d : %d\n", pin, val);
		}
		ignoreInput();
	}

	if (!gpio_deinit()) {
		fprintf(stderr, "%s\n", gpio_getLastError());
		return 1;
	}
	printf("Done!\n");
	return 0;
}

void ignoreInput() {
	char c;
	while ((c = getchar()) != '\n' && c != EOF);
}

