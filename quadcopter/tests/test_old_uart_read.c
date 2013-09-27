/*
	test_uart.c

	Test for UART
*/

#include <stdio.h>
#include <string.h>
#include <stdint.h>

#include "uart.h"

static void ignoreInput();

int main(int argc, char **argv) {
	if (!uart_init(57600, UART_PAREVEN)) {
		fprintf(stderr, "uart_init(): %s\n", uart_getLastError());
		return -1;
	}

	int  done = 0,
	     bytes;
	char buffer[51],
	     *pos;

	while (!done) {
		// Receive
		bytes = uart_read(buffer, 1);
		buffer[bytes] = '\0';
		if (bytes > 0) {
			// Convert CR to LF
			char *pos;
			while ((pos = strchr(buffer, '\r')) != NULL)
				*pos = '\n';
			printf("%s", buffer);
		}

		if (memchr(buffer, '`', bytes) != NULL)
			done = 1;
	}

	if (!uart_deinit()) {
		fprintf(stderr, "uart_deinit(): %s\n", uart_getLastError());
		return -1;
	}

	printf("Done!\n");
	return 0;
}

void ignoreInput() {
	char c;
	while ((c = getchar()) != '\n' && c != EOF);
}

