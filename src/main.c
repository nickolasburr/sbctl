/**
 * main.c
 *
 * Copyright (C) 2017 Nickolas Burr <nickolasburr@gmail.com>
 */

#include "main.h"

int main (int argc, char **argv) {
	char *power, *serial;
	int err;
	int vendor_id, product_id;
	io_service_t usb_dev;
	IOUSBDeviceInterface **di;
	IOReturn rtn;
	UInt8 speed;

	vendor_id  = atoi(argv[1]);
	product_id = atoi(argv[2]);

	/**
	 * Get USB device object.
	 */
	usb_dev = get_usb_device(&err, vendor_id, product_id);

	if (err) {
		fprintf(stdout, "Error: Could not get USB device.\n");

		exit(EXIT_FAILURE);
	}

	di = get_usb_device_interface(&err, usb_dev);

	if (err) {
		fprintf(stdout, "Error: Could not get USB device interface.\n");

		exit(EXIT_FAILURE);
	}

	(*di)->GetDeviceSpeed(di, &speed);

	/**
	 * Get serial number string.
	 */
	serial = get_serial_number(&err, usb_dev);

	if (err) {
		fprintf(stdout, "Error: Could not get serial number.\n");

		exit(EXIT_FAILURE);
	}

	if (!is_null(serial)) {
		fprintf(stdout, "Serial: %s\n", serial);
		fprintf(stdout, "Speed: %d\n", (int) speed);
	} else {
		fprintf(stdout, "No matching USB devices found.\n");
	}

	return 0;
}
