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
	char *dev_type = "usb";
	int err, index, lindex, power;
	unsigned long address;
	unsigned long bus;
	unsigned long long frame;
	struct timespec ts;
	USB_T *usbif;
	Thun_T *thunif;

	lindex = (argc - 1);

	if (!lindex) {
		ARGV_usage();

		exit(EXIT_FAILURE);
	}

	/**
	 * @todo: Add command, option, option argument[s] validation.
	 */
	char *arg = argv[lindex];

	/**
	 * Handle commands, options based on bitmask.
	 */
	switch (ARGV_get_command_bitmask(arg)) {
		/**
		 * 1. sbctl list, ls [OPTIONS]
		 */
		case MASK_CMD_LIST:
			/**
			 * Alloc for USB_T instance.
			 */
			usbif = ALLOC(sizeof(USB_T));
			usbif->length = USB_get_total_devices(&err);

			if (err) {
				fprintf(stdout, "Error: Could not get total number of USB devices.\n");

				exit(EXIT_FAILURE);
			}

			usbif->devices = ALLOC(sizeof(io_service_t *) * usbif->length);

			/**
			 * Get USB devices, set usbif->devices[index].
			 */
			USB_get_devices(&err, usbif->devices);

			if (err) {
				fprintf(stdout, "Error: Could not get all USB devices.\n");

				exit(EXIT_FAILURE);
			}

			fprintf(stdout, LIST_HEADER);

			for (index = 0; index < usbif->length; index += 1) {
				io_service_t device;
				IOUSBDeviceInterface **devif;

				/**
				 * Get device object.
				 */
				device = usbif->devices[index];

				/**
				 * Get device interface.
				 */
				devif = USB_get_device_interface(&err, device);

				if (err) {
					fprintf(stdout, "Error: Could not get next USB device interface.\n");

					exit(EXIT_FAILURE);
				}

				/**
				 * Get device locationID for bus number.
				 */
				bus = USB_get_device_location_id(&err, devif);

				if (err) {
					fprintf(stdout, "Error: Could not get next USB device location ID.\n");

					exit(EXIT_FAILURE);
				}

				fprintf(stdout, "|");

				/**
				 * @note: dev_type is "usb" until we add additional
				 *        serial types (Thunderbolt, Ethernet, etc).
				 */
				fprintf(stdout, "%1s%-*.4s", "", 6, dev_type);

				/**
				 * Format locationID into hex for strtol.
				 */
				snprintf(bus_buf, 5, "%#lx", bus);
				fprintf(stdout, "%1s%-*.3d", "", 5, (int) strtoul(bus_buf, NULL, 0));

				address = USB_get_device_address(&err, devif);

				if (err) {
					fprintf(stdout, "Error: Could not get next USB device address.\n");

					exit(EXIT_FAILURE);
				}

				fprintf(stdout, "%1s%-*.2lu", "", 9, address);

				power = USB_get_bus_power(&err, devif);

				if (err) {
					fprintf(stdout, "Error: Could not get next USB device power.\n");

					exit(EXIT_FAILURE);
				}

				fprintf(stdout, "%1s%-*d", "", 12, power);

				serial = USB_get_device_serial_number(&err, device);

				if (err) {
					fprintf(stdout, "Error: Could not get next USB device serial number.\n");

					exit(EXIT_FAILURE);
				}

				if (is_null(serial)) {
					serial = lines;
				}

				fprintf(stdout, "%1s%-*.13s", "", 15, serial);

				vendor = USB_get_device_vendor_name(&err, device);

				if (err) {
					fprintf(stdout, "Error: Could not get next USB device vendor name.\n");

					exit(EXIT_FAILURE);
				}

				if (is_null(vendor)) {
					vendor = lines;
				}

				fprintf(stdout, "%1s%-*.6s", "", 8, vendor);

				product = USB_get_device_product_name(&err, device);

				if (err) {
					fprintf(stdout, "Error: Could not get next USB device product name.\n");

					exit(EXIT_FAILURE);
				}

				if (is_null(product)) {
					product = lines;
				}

				fprintf(stdout, "%1s%-*.19s", "", 20, product);

				fprintf(stdout, "|");

				/**
				 * Add trailing newline.
				 */
				fprintf(stdout, "\n");

				(*devif)->Release(devif);
			}

			thunif = ALLOC(sizeof(Thun_T));
			thunif->length = THUN_get_total_ports(&err);

			thunif->devices = ALLOC(sizeof(io_service_t *) * thunif->length);

			/**
			 * Get Thunderbolt ports.
			 */
			THUN_get_ports(&err, thunif->devices);

			fprintf(stdout, "THUN: %d\n", thunif->length);

			for (index = 0; index < thunif->length; index += 1) {
				io_service_t port;
				unsigned long adapter_type;

				/**
				 * Get port object.
				 */
				port = thunif->devices[index];

				adapter_type = THUN_get_adapter_type(&err, port);

				if (err) {
					fprintf(stdout, "Error: Could not get next Thunderbolt port adapter type.\n");

					exit(EXIT_FAILURE);
				}

				/**
				 * Format locationID into hex for strtol.
				 */
				fprintf(stdout, "%lu", adapter_type);

				/**
				 * Add trailing newline.
				 */
				fprintf(stdout, "\n");
			}

			fprintf(stdout, LIST_FOOTER);

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

			ARGV_usage();

			exit(EXIT_FAILURE);
	}

	/**
	 * @todo: Add GC tasks, on_error goto.
	 */

	return 0;
}
