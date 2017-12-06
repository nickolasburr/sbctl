/**
 * main.c
 *
 * Copyright (C) 2017 Nickolas Burr <nickolasburr@gmail.com>
 */

#include "main.h"

volatile int looping = 1;

int main (int argc, char **argv) {
	char bus_buf[5];
	char *serial, *product, *vendor;
	char *lines = "---";
	int err, index, lindex, power;
	unsigned long address;
	unsigned long bus;
	unsigned long long frame;
	struct timespec ts;
	IOUSBDeviceInterface **devif;
	SerialDeviceInterface *serif;

	lindex = (argc - 1);

	if (!lindex) {
		usage();

		exit(EXIT_FAILURE);
	}

	/**
	 * @todo: Add command, option, option argument[s] validation.
	 */
	char *arg = argv[lindex];

	/**
	 * Handle commands, options based on bitmask.
	 */
	switch (get_command_bitmask(arg)) {
		/**
		 * 1. sbctl list, ls [OPTIONS]
		 */
		case MASK_CMD_LIST:
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

			fprintf(stdout, "%s\n", LIST_HEADER);

			for (index = 0; index < serif->length; index += 1) {
				io_service_t device;

				/**
				 * Get device object.
				 */
				device = serif->devices[index];

				/**
				 * Get device interface.
				 */
				devif = get_usb_device_interface(&err, device);

				if (err) {
					fprintf(stdout, "Error: Could not get next USB device interface.\n");

					exit(EXIT_FAILURE);
				}

				/**
				 * Get device bus number.
				 */
				bus = get_bus_number(&err, devif);

				if (err) {
					fprintf(stdout, "Error: Could not get next USB device bus number.\n");

					exit(EXIT_FAILURE);
				}

				/**
				 * Format bus into hex for strtol.
				 */
				snprintf(bus_buf, 5, "%#lx", bus);
				fprintf(stdout, "%2s%-*d", "", 5, (int) strtoul(bus_buf, NULL, 0));

				address = get_device_address(&err, devif);

				if (err) {
					fprintf(stdout, "Error: Could not get next USB device address.\n");

					exit(EXIT_FAILURE);
				}

				fprintf(stdout, "%1s%-*lu", "", 7, address);

				power = get_bus_power(&err, devif);

				if (err) {
					fprintf(stdout, "Error: Could not get next USB device power.\n");

					exit(EXIT_FAILURE);
				}

				fprintf(stdout, "%1s%-*d", "", 4, power);

				serial = get_device_serial_number(&err, device);

				if (err) {
					fprintf(stdout, "Error: Could not get next USB device serial number.\n");

					exit(EXIT_FAILURE);
				}

				if (is_null(serial)) {
					serial = lines;
				}

				fprintf(stdout, "%-8s%-5s", "", serial);

				vendor = get_device_vendor_name(&err, device);

				if (err) {
					fprintf(stdout, "Error: Could not get next USB device vendor name.\n");

					exit(EXIT_FAILURE);
				}

				if (is_null(vendor)) {
					vendor = lines;
				}

				fprintf(stdout, "%-11s%-.5s", "", vendor);

				product = get_device_product_name(&err, device);

				if (err) {
					fprintf(stdout, "Error: Could not get next USB device product name.\n");

					exit(EXIT_FAILURE);
				}

				if (is_null(product)) {
					product = lines;
				}

				fprintf(stdout, "%-6s%-5s", "", product);

				/**
				 * Add trailing newline.
				 */
				fprintf(stdout, "\n");

				(*devif)->Release(devif);
			}

			break;
		/**
		 * 2. sbctl get [OPTIONS]
		 *
		 * @todo: Build out get functionality.
		 */
		case MASK_CMD_GET:
			fprintf(stdout, "Information for device.\n");

			break;
		default:
			fprintf(stdout, "Invalid option %s\n\n", arg);

			usage();

			exit(EXIT_FAILURE);
	}

	/**
	 * @todo: Add GC tasks, on_error goto.
	 */

	return 0;
}
