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
	char *usb_type = "usb";
	char *thun_type = "thun";
	char *usb_speed_spec, *thun_speed_spec;
	int err, index, lindex, power;
	int usb_speed, thun_speed;
	unsigned long address, bus;
	unsigned long usb_port, thun_port;
	unsigned long long frame;
	io_service_t device, port;
	IOUSBDeviceInterface **devif;
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
				fprintf(stderr, "Error: Could not get total number of USB devices.\n");

				exit(EXIT_FAILURE);
			}

			usbif->devices = ALLOC(sizeof(io_service_t *) * usbif->length);

			/**
			 * Get USB devices, set usbif->devices[index].
			 */
			USB_get_devices(&err, usbif->devices);

			if (err) {
				fprintf(stderr, "Error: Could not get all USB devices.\n");

				exit(EXIT_FAILURE);
			}

			fprintf(stdout, LIST_HEADER);

			/**
			 * List USB devices.
			 */
			for (index = 0; index < usbif->length; index += 1) {
				/**
				 * Get device object.
				 */
				device = usbif->devices[index];

				/**
				 * Get device interface.
				 */
				devif = USB_get_device_interface(&err, device);

				if (err) {
					fprintf(stderr, "Error: Could not get next USB device interface.\n");

					exit(EXIT_FAILURE);
				}

				fprintf(stdout, "|");

				/**
				 * ex., Type: USB
				 */
				fprintf(stdout, "%1s%-*.4s", "", 6, usb_type);

				/**
				 * Get device locationID for bus number.
				 */
				bus = USB_get_device_location_id(&err, devif);

				if (err) {
					fprintf(stderr, "Error: Could not get next USB device location ID.\n");

					exit(EXIT_FAILURE);
				}

				/**
				 * Format locationID into hex for strtoul.
				 */
				snprintf(bus_buf, 5, "%#lx", bus);
				fprintf(stdout, "%1s%-*.3d", "", 5, (int) strtoul(bus_buf, NULL, 0));

				/**
				 * Get device assigned address.
				 */
				address = USB_get_device_address(&err, devif);

				if (err) {
					fprintf(stderr, "Error: Could not get next USB device address.\n");

					exit(EXIT_FAILURE);
				}

				/**
				 * ex., Address: 01
				 */
				fprintf(stdout, "%1s%-*.2lu", "", 9, address);

				/**
				 * Get device port number.
				 */
				usb_port = USB_get_device_port_number(&err, device);

				if (err) {
					fprintf(stderr, "Error: Could not get next USB port number.\n");

					exit(EXIT_FAILURE);
				}

				/**
				 * ex., Port: 02
				 */
				if (!is_error((int) usb_port, -1)) {
					fprintf(stdout, "%1s%-*.2lu", "", 6, usb_port);
				} else {
					fprintf(stdout, "%1s%-*.3s", "", 6, lines);
				}

				/**
				 * Get bus power available to device.
				 */
				power = USB_get_bus_power(&err, devif);

				if (err) {
					fprintf(stderr, "Error: Could not get next USB device power.\n");

					exit(EXIT_FAILURE);
				}

				/**
				 * ex., Power: 250
				 */
				fprintf(stdout, "%1s%-*d", "", 12, power);

				usb_speed = USB_get_device_speed(&err, devif);

				if (err) {
					fprintf(stderr, "Error: Could not get next USB device speed class.\n");

					exit(EXIT_FAILURE);
				}

				usb_speed_spec = USB_get_device_speed_as_spec(&err, usb_speed);

				if (err) {
					fprintf(stderr, "Error: Could not get next USB device speed specification.\n");

					exit(EXIT_FAILURE);
				}

				/**
				 * ex., Speed: 480
				 */
				fprintf(stdout, "%1s%-*.5s", "", 14, usb_speed_spec);

				serial = USB_get_device_serial_number(&err, device);

				if (err) {
					fprintf(stderr, "Error: Could not get next USB device serial number.\n");

					exit(EXIT_FAILURE);
				}

				if (is_null(serial)) {
					serial = lines;
				}

				fprintf(stdout, "%1s%-*.13s", "", 15, serial);

				vendor = USB_get_device_vendor_name(&err, device);

				if (err) {
					fprintf(stderr, "Error: Could not get next USB device vendor name.\n");

					exit(EXIT_FAILURE);
				}

				if (is_null(vendor)) {
					vendor = lines;
				}

				fprintf(stdout, "%1s%-*.6s", "", 8, vendor);

				product = USB_get_device_product_name(&err, device);

				if (err) {
					fprintf(stderr, "Error: Could not get next USB device product name.\n");

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

			Port_T *ports;

			ports = ALLOC(sizeof(Port_T));
			ports->length = THUN_get_total_ports(&err);

			if (err) {
				fprintf(stderr, "Error: Could not get total number of Thunderbolt ports.\n");

				exit(EXIT_FAILURE);
			}

			ports->ports = ALLOC(sizeof(io_service_t *) * ports->length);

			/**
			 * Get Thunderbolt ports.
			 */
			THUN_get_ports(&err, ports);

			/**
			 * List PCI entities using Thunderbolt.
			 */
			for (index = 0; index < ports->length; index += 1) {
				/**
				 * Get port object.
				 */
				port = ports->ports[index];

				fprintf(stdout, "|");

				/**
				 * Thunderbolt type.
				 */
				fprintf(stdout, "%1s%-*.4s", "", 6, thun_type);

				/**
				 * Placeholder for Bus, Address columns.
				 */
				fprintf(stdout, "%1s%-*.3s", "", 5, lines);
				fprintf(stdout, "%1s%-*.3s", "", 9, lines);

				thun_port = THUN_get_port_number(&err, port);

				if (err) {
					fprintf(stderr, "Error: Could not get next Thunderbolt port number.\n");

					exit(EXIT_FAILURE);
				}

				/**
				 * Port number.
				 */
				fprintf(stdout, "%1s%-*.2lu", "", 6, thun_port);

				/**
				 * Placeholders for Power, Speed, Serial Number,
				 * Vendor, and Product Description.
				 */
				fprintf(stdout, "%1s%-*s", "", 12, lines);
				fprintf(stdout, "%1s%-*.5s", "", 14, lines);
				fprintf(stdout, "%1s%-*.13s", "", 15, lines);
				fprintf(stdout, "%1s%-*.6s", "", 8, lines);
				fprintf(stdout, "%1s%-*.19s", "", 20, lines);


				fprintf(stdout, "|");

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
			fprintf(stderr, "Invalid option %s\n\n", arg);

			ARGV_usage();

			exit(EXIT_FAILURE);
	}

	/**
	 * @todo: Add GC tasks, on_error goto.
	 */

	return 0;
}
