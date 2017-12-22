/**
 * main.c
 *
 * Copyright (C) 2017 Nickolas Burr <nickolasburr@gmail.com>
 */

#include "main.h"

volatile int looping = 1;

int main (int argc, char **argv) {
	char bus_buf[5];
	char *serial = NULL;
	const char *product = NULL;
	char *vendor = NULL;
	char *cmd_arg = NULL;
	char *lines = "---";
	char *pci_spec = "pci";
	char *pci_mode = "pci";
	char *pci_type = "pci";
	char *usb_spec = "usb";
	char *usb_mode = "usb";
	char *usb_type = "usb";
	char *thun_spec = "pci";
	char *thun_mode = "thun";
	char *thun_type = "pci";
	char *usb_speed_spec = NULL;
	char *thun_speed_spec = NULL;
	char *tb_name = NULL;
	char *ts_name = NULL;
	char *ts_vendor = NULL;
	int count, err, index, lindex, power;
	int usb_speed, thun_speed;
	unsigned int tb_vers;
	unsigned long address, usb_lid, dev_id;
	unsigned long usb_port, thun_port;
	unsigned long tb_bus, tp_bus, ts_bus;
	unsigned long long frame;
	io_service_t device, port, bridge, swit;
	IOUSBDeviceInterface **devif;
	USB_T *usbif;
	Thun_T *thunif;
	Port_T *ports;
	Bridge_T *bridges;
	Switch_T *switches;

	lindex = (argc - 1);

	if (!lindex) {
		ARGV_usage();

		exit(EXIT_FAILURE);
	}

	/**
	 * @todo: Add command, option, option argument[s] validation.
	 */
	cmd_arg = argv[lindex];

	count = 0;

	/**
	 * Handle commands, options based on bitmask.
	 */
	switch (ARGV_get_command_bitmask(cmd_arg)) {
		/**
		 * 1. sbctl list, ls [OPTIONS]
		 */
		case MASK_CMD_LIST:
			/**
			 * USB hubs, buses, etc.
			 */

			usbif = ALLOC(sizeof(usbif));
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

			for (index = 0; index < usbif->length; index += 1) {
				/**
				 * Get device object.
				 */
				device = usbif->devices[index];

				/**
				 * Get device interface.
				 */
				devif = USB_get_device_interface(&err, &device);

				if (err) {
					fprintf(stderr, "Error: Could not get next USB device interface.\n");

					exit(EXIT_FAILURE);
				}

				fprintf(stdout, "|");

				fprintf(stdout, "%1s%-*.3d", "", 5, count++);

				/**
				 * USB spec.
				 */
				fprintf(stdout, "%1s%-*.4s", "", 6, usb_spec);

				/**
				 * ex., Mode: USB.
				 */
				fprintf(stdout, "%1s%-*.4s", "", 6, usb_mode);

				/**
				 * Placeholder for Type (device, hub, bridge, etc.)
				 */
				fprintf(stdout, "%1s%-*.4s", "", 6, lines);

				/**
				 * Get device locationID for bus number.
				 */
				usb_lid = USB_get_device_location_id(&err, devif);

				if (err) {
					fprintf(stderr, "Error: Could not get next USB device location ID.\n");

					exit(EXIT_FAILURE);
				}

				/**
				 * Format locationID into hex for strtoul.
				 */
				snprintf(bus_buf, 5, "%#lx", usb_lid);
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
				 * ex., Address: 001
				 */
				fprintf(stdout, "%1s%-*.3lu", "", 9, address);

				/**
				 * Get device port number.
				 */
				usb_port = USB_get_device_port_number(&err, &device);

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

				serial = USB_get_device_serial_number(&err, &device);

				if (err) {
					fprintf(stderr, "Error: Could not get next USB device serial number.\n");

					exit(EXIT_FAILURE);
				}

				if (is_null(serial)) {
					serial = lines;
				}

				/**
				 * ex., Serial: 00000000
				 */
				fprintf(stdout, "%1s%-*.13s", "", 15, serial);

				dev_id = USB_get_device_id(&err, devif);

				if (err) {
					fprintf(stderr, "Error: Could not get next USB device ID.\n");

					exit(EXIT_FAILURE);
				}

				/**
				 * ex., Device ID: 016
				 */
				fprintf(stdout, "%1s%-*.3lu", "", 12, dev_id);

				vendor = USB_get_device_vendor_name(&err, &device);

				if (err) {
					fprintf(stderr, "Error: Could not get next USB device vendor name.\n");

					exit(EXIT_FAILURE);
				}

				if (is_null(vendor)) {
					vendor = lines;
				}

				/**
				 * ex., Vendor: Apple
				 */
				fprintf(stdout, "%1s%-*.6s", "", 8, vendor);

				product = USB_get_device_product_name(&err, &device);

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

			FREE(usbif->devices);
			FREE(usbif);

			/**
			 * PCI, Thunderbolt.
			 */

			ports = ALLOC(sizeof(ports));
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
				fprintf(stdout, "%1s%-*.3d", "", 5, count++);

				/**
				 * Thunderbolt Spec, Mode, Type.
				 */
				fprintf(stdout, "%1s%-*.4s", "", 6, thun_spec);
				fprintf(stdout, "%1s%-*.4s", "", 6, thun_mode);
				fprintf(stdout, "%1s%-*.4s", "", 6, "port");

				/**
				 * Placeholder for Bus, Address columns.
				 */
				fprintf(stdout, "%1s%-*.3lu", "", 5, THUN_get_port_bus_number(&err, &port));
				fprintf(stdout, "%1s%-*.3s", "", 9, lines);

				thun_port = THUN_get_port_number(&err, &port);

				if (err) {
					fprintf(stderr, "Error: Could not get next Thunderbolt port number.\n");

					exit(EXIT_FAILURE);
				}

				/**
				 * Port number.
				 */
				fprintf(stdout, "%1s%-*.2lu", "", 6, thun_port);

				/**
				 * Placeholder for Power.
				 */
				fprintf(stdout, "%1s%-*s", "", 12, lines);

				tb_vers = THUN_get_port_thunderbolt_version(&err, &port);

				if (err) {
					fprintf(stderr, "Error: Could not get next Thunderbolt port version.\n");
				}

				/**
				 * Show speed rating based on Thunderbolt version.
				 */
				switch (tb_vers) {
					case THUN_V1:
						fprintf(stdout, "%1s%-*.5d", "", 14, THUN_V1_SPEED);

						break;
					case THUN_V2:
						fprintf(stdout, "%1s%-*.5d", "", 14, THUN_V2_SPEED);

						break;
					case THUN_V3:
						fprintf(stdout, "%1s%-*.5d", "", 14, THUN_V3_SPEED);

						break;
					default:
						fprintf(stdout, "%1s%-*.5s", "", 14, lines);
				}

				/**
				 * Placeholder for Serial Number.
				 */
				fprintf(stdout, "%1s%-*.13s", "", 15, lines);

				dev_id = THUN_get_port_device_id(&err, &port);

				if (err) {
					fprintf(stderr, "Error: Could not get next Thunderbolt port device ID.\n");

					exit(EXIT_FAILURE);
				}

				/**
				 * Device ID.
				 */
				fprintf(stdout, "%1s%-*.3lu", "", 12, dev_id);

				/**
				 * Placeholders for Vendor and Product Description.
				 */
				fprintf(stdout, "%1s%-*.6s", "", 8, lines);

				product = THUN_get_port_description(&err, &port);

				if (err) {
					fprintf(stdout, "Error: Could not get next Thunderbolt port description.\n");

					exit(EXIT_FAILURE);
				}

				fprintf(stdout, "%1s%-*.19s", "", 20, product);


				fprintf(stdout, "|");

				/**
				 * Add trailing newline.
				 */
				fprintf(stdout, "\n");
			}

			FREE(ports->ports);
			FREE(ports);

			bridges = ALLOC(sizeof(bridges));
			bridges->length = THUN_get_total_bridges(&err);

			if (err) {
				fprintf(stderr, "Error: Could not get total number of PCI-PCI Thunderbolt bridges.\n");

				exit(EXIT_FAILURE);
			}

			bridges->bridges = ALLOC(sizeof(io_service_t *) * bridges->length);

			/**
			 * Get Thunderbolt ports.
			 */
			THUN_get_bridges(&err, bridges);

			/**
			 * List PCI-PCI Thunderbolt bridges.
			 */
			for (index = 0; index < bridges->length; index += 1) {
				/**
				 * Get bridge object.
				 */
				bridge = bridges->bridges[index];

				fprintf(stdout, "|");
				fprintf(stdout, "%1s%-*.3d", "", 5, count++);

				/**
				 * Thunderbolt Spec, Mode, Type.
				 */
				fprintf(stdout, "%1s%-*.4s", "", 6, thun_spec);
				fprintf(stdout, "%1s%-*.4s", "", 6, thun_mode);
				fprintf(stdout, "%1s%-*.4s", "", 6, "bridge");

				/**
				 * Get bridge bus number.
				 */
				tb_bus = THUN_get_bridge_bus_number(&err, &bridge);

				if (err) {
					fprintf(stderr, "Error: Could not get next Thunderbolt bridge bus number.\n");

					exit(EXIT_FAILURE);
				}

				fprintf(stdout, "%1s%-*.3lu", "", 5, tb_bus);

				/**
				 * Placeholder for Address, Port.
				 */
				fprintf(stdout, "%1s%-*.3s", "", 9, lines);
				fprintf(stdout, "%1s%-*.3s", "", 6, lines);

				/**
				 * Placeholders for Power, Speed, Serial Number, and Device ID.
				 */
				fprintf(stdout, "%1s%-*s", "", 12, lines);
				fprintf(stdout, "%1s%-*.5s", "", 14, lines);
				fprintf(stdout, "%1s%-*.13s", "", 15, lines);
				fprintf(stdout, "%1s%-*.3s", "", 12, lines);

				/**
				 * Placeholder for Vendor.
				 */
				fprintf(stdout, "%1s%-*.6s", "", 8, lines);

				/**
				 * Get Thunderbolt bridge name.
				 */
				tb_name = THUN_get_bridge_name(&err, &bridge);

				if (err) {
					fprintf(stderr, "Error: Could not get next Thunderbolt bridge name.\n");

					exit(EXIT_FAILURE);
				}

				fprintf(stdout, "%1s%-*.19s", "", 20, tb_name);

				fprintf(stdout, "|");

				/**
				 * Add trailing newline.
				 */
				fprintf(stdout, "\n");
			}

			FREE(bridges->bridges);
			FREE(bridges);

			switches = ALLOC(sizeof(switches));
			switches->length = THUN_get_total_all_switches(&err);

			if (err) {
				fprintf(stderr, "Error: Could not get total number of Thunderbolt switches.\n");

				exit(EXIT_FAILURE);
			}

			switches->switches = ALLOC(sizeof(io_service_t *) * switches->length);

			THUN_get_all_switches(&err, switches);

			/**
			 * List Thunderbolt switches.
			 */
			for (index = 0; index < switches->length; index += 1) {
				/**
				 * Get switch object.
				 */
				swit = switches->switches[index];

				fprintf(stdout, "|");
				fprintf(stdout, "%1s%-*.3d", "", 5, count++);

				/**
				 * Thunderbolt Spec, Mode, Type.
				 */
				fprintf(stdout, "%1s%-*.4s", "", 6, thun_spec);
				fprintf(stdout, "%1s%-*.4s", "", 6, thun_mode);
				fprintf(stdout, "%1s%-*.4s", "", 6, "switch");

				/**
				 * Placeholder for Bus, Address, Port.
				 */
				fprintf(stdout, "%1s%-*.3s", "", 5, lines);
				fprintf(stdout, "%1s%-*.3s", "", 9, lines);
				fprintf(stdout, "%1s%-*.3s", "", 6, lines);

				/**
				 * Placeholders for Power, Speed, Serial Number, and Device ID.
				 */
				fprintf(stdout, "%1s%-*s", "", 12, lines);
				fprintf(stdout, "%1s%-*.5s", "", 14, lines);
				fprintf(stdout, "%1s%-*.13s", "", 15, lines);
				fprintf(stdout, "%1s%-*.3s", "", 12, lines);

				ts_vendor = THUN_get_switch_vendor(&err, &swit);

				if (err) {
					fprintf(stderr, "Error: Could not get next Thunderbolt switch vendor name.\n");
				}

				fprintf(stdout, "%1s%-*.5s", "", 8, ts_vendor);

				ts_name = THUN_get_switch_name(&err, &swit);

				if (err) {
					fprintf(stderr, "Error: Could not get next Thunderbolt switch model name.\n");

					exit(EXIT_FAILURE);
				}

				fprintf(stdout, "%1s%-*.19s", "", 20, ts_name);

				fprintf(stdout, "|");

				/**
				 * Add trailing newline.
				 */
				fprintf(stdout, "\n");
			}

			FREE(switches->switches);
			FREE(switches);

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
		/**
		 * 3. sbctl set [OPTIONS]
		 *
		 * @todo: Build out set functionality.
		 */
		case MASK_CMD_SET:
			fprintf(stdout, "Setting properties on device.\n");

			break;
		default:
			fprintf(stderr, "Invalid option %s\n\n", cmd_arg);

			ARGV_usage();

			exit(EXIT_FAILURE);
	}

	/**
	 * @todo: Add GC tasks, on_error goto.
	 */

	return 0;
}
