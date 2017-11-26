/**
 * main.c
 *
 * Copyright (C) 2017 Nickolas Burr <nickolasburr@gmail.com>
 */

#include "main.h"

int main (int argc, char **argv) {
	char *serial;
	int sn_err, vendor_id, product_id;

	vendor_id  = atoi(argv[1]);
	product_id = atoi(argv[2]);

	serial = get_serial_number(&sn_err, vendor_id, product_id);

	/**
	 * If there were problems retrieving the
	 * serial number, run cleanup and exit.
	 */
	if (sn_err) {
		fprintf(stdout, "Error: Could not get serial number\n");

		exit(EXIT_FAILURE);
	}

	if (!is_null(serial)) {
		printf("Serial: %s\n", serial);
	} else {
		printf("No matching USB devices found.\n");
	}

	return 0;
}
