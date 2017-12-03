/**
 * main.c
 *
 * Copyright (C) 2017 Nickolas Burr <nickolasburr@gmail.com>
 */

#include "main.h"

volatile int looping = 1;

int main (int argc, char **argv) {
	char *serial;
	int index;
	int err, vendor_id, product_id, power, speed;
	long long frame;
	struct timespec ts;
	io_service_t dev;
	IOUSBDeviceInterface **devif;
	SerialDeviceInterface *serif;

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
			fprintf(stdout, "Speed: Low\n");

			break;
		case kUSBDeviceSpeedFull:
			fprintf(stdout, "Speed: Full\n");

			break;
		case kUSBDeviceSpeedHigh:
			fprintf(stdout, "Speed: High\n");

			break;
		case kUSBDeviceSpeedSuper:
			fprintf(stdout, "Speed: Super\n");

			break;
		case kUSBDeviceSpeedSuperPlus:
			fprintf(stdout, "Speed: Super Plus\n");

			break;
		default:
			fprintf(stdout, "Speed: Unknown\n");
	}

	/**
	 * Alloc for SerialDeviceInterface instance.
	 */
	serif = ALLOC(sizeof(SerialDeviceInterface));
	serif->total = get_num_usb_devices(&err);

	if (err) {
		fprintf(stdout, "Error: Could not get total number of USB devices.\n");

		exit(EXIT_FAILURE);
	}

	serif->devices = ALLOC(sizeof(io_service_t) * serif->total);

	/**
	 * Get USB devices, set serif->devices[index].
	 */
	get_usb_devices(&err, serif);

	if (err) {
		fprintf(stdout, "Error: Could not get all USB devices.\n");

		exit(EXIT_FAILURE);
	}

	index = 0;

	while (index < serif->total) {
		int tpower;
		IOUSBDeviceInterface **tdevif;

		tdevif = get_usb_device_interface(&err, serif->devices[index]);

		if (err) {
			fprintf(stdout, "Error: Could not get single USB device interface.\n");

			exit(EXIT_FAILURE);
		}

		tpower = get_bus_power(&err, tdevif);

		if (err) {
			fprintf(stdout, "Error: Could not get next USB device power.\n");

			exit(EXIT_FAILURE);
		}

		fprintf(stdout, "tPower: %d\n", tpower);

		index++;
	}

	signal(SIGINT, on_signal);
	signal(SIGHUP, on_signal);

	ts.tv_sec = 0;
	/* 0.0025s */
	ts.tv_nsec = 2500000L;

	while (looping) {
		frame = get_bus_frame(&err, devif);

		if (err) {
			fprintf(stdout, "Error: Could not get bus frame.\n");

			exit(EXIT_FAILURE);
		}

		fprintf(stdout, "\rFrame: %lld", frame);
		fflush(stdout);

		nanosleep(&ts, NULL);
	}

	return 0;
}
