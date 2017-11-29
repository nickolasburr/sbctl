/**
 * main.c
 *
 * Copyright (C) 2017 Nickolas Burr <nickolasburr@gmail.com>
 */

#include "main.h"

int main (int argc, char **argv) {
	char *serial;
	int err, vendor_id, product_id, count, power, speed;
	long long frame;
	io_service_t dev;
	IOUSBDeviceInterface **devif;

	vendor_id  = atoi(argv[1]);
	product_id = atoi(argv[2]);

	/**
	 * Get USB device object.
	 */
	dev = get_usb_device(&err, vendor_id, product_id);

	if (err) {
		fprintf(stdout, "Error: Could not get USB device.\n");

		exit(EXIT_FAILURE);
	}

	/**
	 * Get USB device interface.
	 */
	devif = get_usb_device_interface(&err, dev);

	if (err) {
		fprintf(stdout, "Error: Could not get USB device interface.\n");

		exit(EXIT_FAILURE);
	}

	power = get_bus_power(&err, devif);

	if (err) {
		fprintf(stdout, "Error: Could not get available bus power.\n");

		exit(EXIT_FAILURE);
	}

	fprintf(stdout, "Power: %d\n", power);

	speed = get_device_speed(&err, devif);

	if (err) {
		fprintf(stdout, "Error: Could not get device speed.\n");

		exit(EXIT_FAILURE);
	}

	/**
	 * Output USB device speed level.
	 */
	switch (speed) {
		case kUSBDeviceSpeedLow:
			fprintf(stdout, "USB Speed: Low\n");

			break;
		case kUSBDeviceSpeedFull:
			fprintf(stdout, "USB Speed: Full\n");

			break;
		case kUSBDeviceSpeedHigh:
			fprintf(stdout, "USB Speed: High\n");

			break;
		case kUSBDeviceSpeedSuper:
			fprintf(stdout, "USB Speed: Super\n");

			break;
		case kUSBDeviceSpeedSuperPlus:
			fprintf(stdout, "USB Speed: Super Plus\n");

			break;
		default:
			fprintf(stdout, "USB Speed: Unknown\n");
	}

	count = 20;

	while (count--) {
		frame = get_bus_frame(&err, devif);

		if (err) {
			fprintf(stdout, "Error: Could not get bus frame.\n");

			exit(EXIT_FAILURE);
		}

		fprintf(stdout, "Frame: %lld\n", frame);
	}

	return 0;
}
