/**
 * main.c
 *
 * Copyright (C) 2017 Nickolas Burr <nickolasburr@gmail.com>
 */

#include "main.h"

int main (int argc, char **argv) {
	CFStringRef obj;
	char serial[256];
	int vendor_id, product_id;

	vendor_id  = atoi(argv[1]);
	product_id = atoi(argv[2]);

	obj = get_serial(vendor_id, product_id);

	if (obj) {
		if (CFStringGetCString(obj, serial, 256, CFStringGetSystemEncoding())) {
			printf("Serial: %s\n", serial);
		}
	} else {
		printf("No matching USB devices found.\n");
	}

	return 0;
}
