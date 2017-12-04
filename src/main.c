/**
 * main.c
 *
 * Copyright (C) 2017 Nickolas Burr <nickolasburr@gmail.com>
 */

#include "main.h"

volatile int looping = 1;

int main (int argc, char **argv) {
	char *serial;
	int err, index, lindex, power;
	long long frame;
	struct timespec ts;
	IOUSBDeviceInterface **devif;
	SerialDeviceInterface *serif;

	lindex = (argc - 1);

	if (!lindex) {
		usage();

		exit(EXIT_FAILURE);
	}

	/**
	 * @todo: Add option, argument validation.
	 */
	char *arg = argv[lindex];

	/**
	 * Handle options based on bitmask.
	 */
	switch (get_mask_from_value(arg)) {
		/**
		 * 1. sbctl list, ls [OPTIONS]
		 */
		case 0x1:
			/**
			 * Alloc for SerialDeviceInterface instance.
			 */
			serif = ALLOC(sizeof(SerialDeviceInterface));
			serif->length = get_total_usb_devices(&err);

			if (err) {
				fprintf(stdout, "Error: Could not get total number of USB devices.\n");

				exit(EXIT_FAILURE);
			}

			serif->devices = ALLOC(sizeof(io_service_t *) * serif->length);

			/**
			 * Get USB devices, set serif->devices[index].
			 */
			get_usb_devices(&err, serif->devices);

			if (err) {
				fprintf(stdout, "Error: Could not get all USB devices.\n");

				exit(EXIT_FAILURE);
			}

			for (index = 0; index < serif->length; index += 1) {
				devif = get_usb_device_interface(&err, serif->devices[index]);

				if (err) {
					fprintf(stdout, "Error: Could not get next USB device interface.\n");

					exit(EXIT_FAILURE);
				}

				fprintf(stdout, "%2d: ", index);

				serial = get_serial_number(&err, serif->devices[index]);

				if (err) {
					fprintf(stdout, "Error: Could not get next USB device serial number.\n");

					exit(EXIT_FAILURE);
				}

				if (is_null(serial)) {
					serial = "Unknown";
				}

				fprintf(stdout, "Serial -> %s, ", serial);

				power = get_bus_power(&err, devif);

				if (err) {
					fprintf(stdout, "Error: Could not get next USB device power.\n");

					exit(EXIT_FAILURE);
				}

				fprintf(stdout, "Power -> %dmA", power);

				/**
				 * Add trailing newline.
				 */
				fprintf(stdout, "\n");

				(*devif)->Release(devif);
			}

			break;
		/**
		 * 2. sbctl show [OPTIONS]
		 *
		 * @todo: Build out show functionality.
		 */
		case 0x2:
			fprintf(stdout, "Information for device.\n");

			break;
		default:
			fprintf(stdout, "Invalid option %s\n", arg);

			exit(EXIT_FAILURE);
	}

	/**
	 * @todo: Add GC tasks, on_error goto.
	 */

	return 0;
}
