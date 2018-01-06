/**
 * main.c
 *
 * Copyright (C) 2017 Nickolas Burr <nickolasburr@gmail.com>
 */

#include "main.h"

volatile int looping = 1;

int main (int argc, char **argv) {
	char *target = NULL;
	char number, numbers[5];
	char bus_buf[5];
	char *serial = NULL;
	const char *product = NULL;
	char *vendor = NULL;
	char *cmd_arg = NULL;
	char *opt_arg = NULL;
	char *lines = "---";
	char *pci_smt = "pci";
	char *usb_smt = "usb";
	char *thun_mode = "thun";
	char *thun_port_type = "port";
	char *thun_brid_type = "bridge";
	char *thun_swit_type = "switch";
	char *usb_speed_spec = NULL;
	char *thun_speed_spec = NULL;
	const char *tb_name = NULL;
	const char *ts_name = NULL;
	const char *ts_vendor = NULL;
	int count, err, index, power;
	int start, end, entry, total_entries;
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

	/**
	 * Name of executable (sbctl).
	 */
	target = argv[0];

	/**
	 * When sbctl is given with no arguments.
	 */
	if (!(argc - 1)) {
		ARGV_usage();

		exit(EXIT_FAILURE);
	}

	/**
	 * Top-level command given to sbctl (i.e. ls, get, unset, etc).
	 */
	cmd_arg = argv[1];

	/**
	 * Tally total number of entries across all types.
	 */
	total_entries = 0;

	/**
	 *
	 * Universal Serial Bus
	 *
	 */

	/**
	 * USB devices, buses, hubs, etc.
	 */
	usbif = ALLOC(sizeof(usbif));
	usbif->length = USB_get_total_devices(&err);
	assert(!err);

	usbif->devices = ALLOC(sizeof(usbif->devices) * usbif->length);

	USB_get_devices(&err, usbif->devices);
	assert(!err);

	total_entries += usbif->length;

	/**
	 *
	 * PCI, Thunderbolt
	 *
	 */

	/**
	 * Thunderbolt ports.
	 */
	ports = ALLOC(sizeof(ports));
	ports->length = THUN_get_total_ports(&err);
	assert(!err);

	ports->ports = ALLOC(sizeof(ports->ports) * ports->length);

	THUN_get_ports(&err, ports);
	assert(!err);

	total_entries += ports->length;

	/**
	 * PCI-PCI Thunderbolt bridges.
	 */
	bridges = ALLOC(sizeof(bridges));
	bridges->length = THUN_get_total_bridges(&err);
	assert(!err);

	bridges->bridges = ALLOC(sizeof(bridges->bridges) * bridges->length);

	THUN_get_bridges(&err, bridges);
	assert(!err);

	total_entries += bridges->length;

	/**
	 * Thunderbolt switches.
	 */
	switches = ALLOC(sizeof(switches));
	switches->length = THUN_get_total_all_switches(&err);
	assert(!err);

	switches->switches = ALLOC(sizeof(switches->switches) * switches->length);

	THUN_get_all_switches(&err, switches);
	assert(!err);

	total_entries += switches->length;

	/**
	 * Handle commands, options based on bitmask.
	 */
	switch (ARGV_get_command_bitmask(cmd_arg)) {
		/**
		 * 1. sbctl list, ls [OPTIONS]
		 */
		case MASK_CMD_LIST:
			fprintf(stdout, LIST_HEADER);

			/**
			 * Entry index counter.
			 */
			count = 0;

			/**
			 * List USB devices, buses, hubs, etc.
			 */
			for (index = 0; index < usbif->length; index += 1) {
				/**
				 * Get device object, interface.
				 */
				device = usbif->devices[index];
				devif = USB_get_device_interface(&err, &device);
				assert(!err);

				fprintf(stdout, "|");
				fprintf(stdout, "%1s%-*.2d", "", 5, ++count);

				/**
				 * USB Spec, Mode.
				 */
				fprintf(stdout, "%1s%-*.4s", "", 6, usb_smt);
				fprintf(stdout, "%1s%-*.4s", "", 6, usb_smt);

				/**
				 * Placeholder for Type (device, hub, bridge, etc.)
				 */
				fprintf(stdout, "%1s%-*.4s", "", 6, lines);

				/**
				 * Get device locationID for bus number.
				 */
				usb_lid = USB_get_device_location_id(&err, devif);
				assert(!err);

				/**
				 * Format locationID into hex for strtoul.
				 */
				snprintf(bus_buf, 5, "%#lx", usb_lid);
				fprintf(stdout, "%1s%-*.3d", "", 5, (int) strtoul(bus_buf, NULL, 0));

				/**
				 * Get device assigned address.
				 */
				address = USB_get_device_address(&err, devif);
				assert(!err);

				/**
				 * ex., Address: 001
				 */
				fprintf(stdout, "%1s%-*.3lu", "", 9, address);

				/**
				 * Get device port number.
				 */
				usb_port = USB_get_device_port_number(&err, &device);
				assert(!err);

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
				assert(!err);

				/**
				 * ex., Power: 250
				 */
				fprintf(stdout, "%1s%-*d", "", 12, power);

				usb_speed = USB_get_device_speed(&err, devif);
				assert(!err);

				usb_speed_spec = USB_get_device_speed_as_spec(&err, usb_speed);
				assert(!err);

				/**
				 * ex., Speed: 480
				 */
				fprintf(stdout, "%1s%-*.5s", "", 14, usb_speed_spec);

				serial = USB_get_device_serial_number(&err, &device);
				assert(!err);

				serial = !is_null(serial) ? serial : lines;

				/**
				 * ex., Serial: 00000000
				 */
				fprintf(stdout, "%1s%-*.13s", "", 15, serial);

				dev_id = USB_get_device_id(&err, devif);
				assert(!err);

				/**
				 * ex., Device ID: 016
				 */
				fprintf(stdout, "%1s%-*.3lu", "", 12, dev_id);

				vendor = USB_get_device_vendor_name(&err, &device);
				assert(!err);

				vendor = !is_null(vendor) ? vendor : lines;

				/**
				 * ex., Vendor: Apple
				 */
				fprintf(stdout, "%1s%-*.6s", "", 8, vendor);

				product = USB_get_device_product_name(&err, &device);
				assert(!err);

				product = !is_null(product) ? product : lines;

				/**
				 * ex., Product Description: Apple Keyboard
				 */
				fprintf(stdout, "%1s%-*.19s", "", 20, product);

				fprintf(stdout, "|\n");

				(*devif)->Release(devif);
			}

			/**
			 * List Thunderbolt ports.
			 */
			for (index = 0; index < ports->length; index += 1) {
				/**
				 * Get port object.
				 */
				port = ports->ports[index];

				fprintf(stdout, "|");
				fprintf(stdout, "%1s%-*.2d", "", 5, ++count);

				/**
				 * Thunderbolt Spec, Mode, Type.
				 */
				fprintf(stdout, "%1s%-*.4s", "", 6, pci_smt);
				fprintf(stdout, "%1s%-*.4s", "", 6, thun_mode);
				fprintf(stdout, "%1s%-*.4s", "", 6, thun_port_type);

				tp_bus = THUN_get_port_bus_number(&err, &port);
				assert(!err);

				fprintf(stdout, "%1s%-*.3lu", "", 5, tp_bus);

				/**
				 * Placeholder for Address.
				 */
				fprintf(stdout, "%1s%-*.3s", "", 9, lines);

				thun_port = THUN_get_port_port_number(&err, &port);
				assert(!err);

				/**
				 * Port number.
				 */
				fprintf(stdout, "%1s%-*.2lu", "", 6, thun_port);

				/**
				 * Placeholder for Power.
				 */
				fprintf(stdout, "%1s%-*s", "", 12, lines);

				tb_vers = THUN_get_port_thunderbolt_version(&err, &port);
				assert(!err);

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
				assert(!err);

				/**
				 * Device ID.
				 */
				fprintf(stdout, "%1s%-*.3lu", "", 12, dev_id);

				/**
				 * Placeholders for Vendor and Product Description.
				 */
				fprintf(stdout, "%1s%-*.6s", "", 8, lines);

				product = THUN_get_port_description(&err, &port);
				assert(!err);

				fprintf(stdout, "%1s%-*.19s", "", 20, product);

				fprintf(stdout, "|\n");
			}

			/**
			 * List PCI-PCI Thunderbolt bridges.
			 */
			for (index = 0; index < bridges->length; index += 1) {
				/**
				 * Get bridge object.
				 */
				bridge = bridges->bridges[index];

				fprintf(stdout, "|");
				fprintf(stdout, "%1s%-*.2d", "", 5, ++count);

				/**
				 * Thunderbolt Spec, Mode, Type.
				 */
				fprintf(stdout, "%1s%-*.4s", "", 6, pci_smt);
				fprintf(stdout, "%1s%-*.4s", "", 6, thun_mode);
				fprintf(stdout, "%1s%-*.4s", "", 6, thun_brid_type);

				/**
				 * Get bridge bus number.
				 */
				tb_bus = THUN_get_bridge_bus_number(&err, &bridge);
				assert(!err);

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
				assert(!err);

				fprintf(stdout, "%1s%-*.19s", "", 20, tb_name);

				fprintf(stdout, "|\n");
			}

			/**
			 * List Thunderbolt switches.
			 */
			for (index = 0; index < switches->length; index += 1) {
				/**
				 * Get switch object.
				 */
				swit = switches->switches[index];

				fprintf(stdout, "|");
				fprintf(stdout, "%1s%-*.2d", "", 5, ++count);

				/**
				 * Thunderbolt Spec, Mode, Type.
				 */
				fprintf(stdout, "%1s%-*.4s", "", 6, pci_smt);
				fprintf(stdout, "%1s%-*.4s", "", 6, thun_mode);
				fprintf(stdout, "%1s%-*.4s", "", 6, thun_swit_type);

				ts_bus = THUN_get_switch_bus_number(&err, &swit);
				assert(!err);

				fprintf(stdout, "%1s%-*.3lu", "", 5, ts_bus);

				/**
				 * Placeholder for Address.
				 */
				fprintf(stdout, "%1s%-*.3s", "", 9, lines);

				thun_port = THUN_get_switch_port_number(&err, &swit);
				assert(!err);

				fprintf(stdout, "%1s%-*.2lu", "", 6, thun_port);

				/**
				 * Placeholder for Power.
				 */
				fprintf(stdout, "%1s%-*s", "", 12, lines);

				tb_vers = THUN_get_switch_thunderbolt_version(&err, &swit);
				assert(!err);

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

				/**
				 * Get PCI Thunderbolt device ID.
				 */
				dev_id = THUN_get_switch_device_id(&err, &swit);
				assert(!err);

				fprintf(stdout, "%1s%-*.3lu", "", 12, dev_id);

				/**
				 * Get PCI Thunderbolt switch vendor.
				 */
				ts_vendor = THUN_get_switch_vendor(&err, &swit);
				assert(!err);

				fprintf(stdout, "%1s%-*.5s", "", 8, ts_vendor);

				/**
				 * Get PCI Thunderbolt switch name.
				 */
				ts_name = THUN_get_switch_name(&err, &swit);
				assert(!err);

				fprintf(stdout, "%1s%-*.19s", "", 20, ts_name);

				fprintf(stdout, "|\n");
			}

			fprintf(stdout, LIST_FOOTER);

			break;
		/**
		 * 2. sbctl get [OPTIONS]
		 *
		 * @todo: Build out get functionality.
		 */
		case MASK_CMD_GET:
			index = 0;

			/**
			 * Entry index number given as command argument.
			 */
			opt_arg = argv[2];

			if (is_null(opt_arg)) {
				fprintf(stderr, "'%s %s' requires an entry index number.\n", target, cmd_arg);

				exit(EXIT_FAILURE);
			}

			/**
			 * Entry index numbers need to be prefixed with % sign.
			 */
			if (opt_arg[index] != ASCII_PERCENT) {
				fprintf(stderr, "Invalid format given as argument to '%s %s'\n", target, cmd_arg);

				exit(EXIT_FAILURE);
			}

			/**
			 * Get up to the first five chars from opt_arg,
			 * and copy its contents to numbers char array.
			 */
			while ((length(numbers) < (index + 1)) && (number = opt_arg[(index + 1)])) {
				numbers[index++] = number;
			}

			numbers[index] = '\0';

			/**
			 * Verify numbers represents valid integer.
			 */
			if (!is_numeric(numbers)) {
				fprintf(stderr, "%s is not a valid number.\n", numbers);

				exit(EXIT_FAILURE);
			}

			/**
			 * Get the actual entry index, as an integer.
			 */
			entry = (int) strtol(numbers, NULL, 0);

			/**
			 * Verify entry is in range {1, total_entries}.
			 */
			if (!((entry >= 1) && (entry <= total_entries))) {
				fprintf(stderr, "Invalid entry index %s\n", numbers);

				exit(EXIT_FAILURE);
			}

			index = (entry - 1);
			start = 0;
			end = (usbif->length - 1);

			/**
			 * USB entry type.
			 */
			if (in_range(index, start, end)) {
				/**
				 * Get device object, interface.
				 */
				device = usbif->devices[index];
				devif = USB_get_device_interface(&err, &device);
				assert(!err);

				product = USB_get_device_product_name(&err, &device);
				assert(!err);

				product = !is_null(product) ? product : lines;

				fprintf(stdout, "%s\n\n", product);
				fprintf(stdout, "Spec: %s\n", usb_smt);
				fprintf(stdout, "Mode: %s\n", usb_smt);
				fprintf(stdout, "Type: %s\n", lines);

				break;
			}

			start = (end + 1);
			end = ((start + ports->length) - 1);

			/**
			 * Thunderbolt port entry type.
			 */
			if (in_range(index, start, end)) {
				index = ((entry - start) - 1);

				/**
				 * Get port object.
				 */
				port = ports->ports[index];

				product = THUN_get_port_description(&err, &port);
				assert(!err);

				product = !is_null(product) ? product : lines;

				fprintf(stdout, "%s\n\n", product);
				fprintf(stdout, "Spec: %s\n", pci_smt);
				fprintf(stdout, "Mode: %s\n", thun_mode);
				fprintf(stdout, "Type: %s\n", thun_port_type);

				break;
			}

			start = (end + 1);
			end = ((start + bridges->length) - 1);

			/**
			 * Thunderbolt bridge entry type.
			 */
			if (in_range(index, start, end)) {
				index = ((entry - start) - 1);

				/**
				 * Get bridge object.
				 */
				bridge = bridges->bridges[index];

				/**
				 * Get Thunderbolt bridge name.
				 */
				tb_name = THUN_get_bridge_name(&err, &bridge);
				assert(!err);

				fprintf(stdout, "%s\n\n", tb_name);
				fprintf(stdout, "Spec: %s\n", pci_smt);
				fprintf(stdout, "Mode: %s\n", thun_mode);
				fprintf(stdout, "Type: %s\n", thun_brid_type);

				break;
			}

			start = (end + 1);
			end = ((start + switches->length) - 1);

			/**
			 * Thunderbolt switch entry type.
			 */
			if (in_range(index, start, end)) {
				index = ((entry - start) - 1);

				/**
				 * Get switch object.
				 */
				swit = switches->switches[index];

				/**
				 * Get PCI Thunderbolt switch name.
				 */
				ts_name = THUN_get_switch_name(&err, &swit);
				assert(!err);

				fprintf(stdout, "%s\n\n", ts_name);
				fprintf(stdout, "Spec: %s\n", pci_smt);
				fprintf(stdout, "Mode: %s\n", thun_mode);
				fprintf(stdout, "Type: %s\n", thun_swit_type);

				break;
			}

			fprintf(stdout, "Requested device is Unknown.\n");

			break;
		/**
		 * 3. sbctl set [OPTIONS]
		 *
		 * @todo: Build out set functionality.
		 */
		case MASK_CMD_SET:
			fprintf(stdout, "Setting properties on device.\n");

			break;
		/**
		 * 4. sbctl help
		 */
		case MASK_CMD_HELP:
			ARGV_usage();

			break;
		/**
		 * 5. sbctl version
		 */
		case MASK_CMD_VERS:
			fprintf(stdout, "%s\n", SBCTL_VERSION);

			break;
		default:
			fprintf(stderr, "Invalid option %s\n\n", cmd_arg);

			ARGV_usage();

			exit(EXIT_FAILURE);
	}

	/**
	 * Run cleanup tasks.
	 */
	FREE(usbif->devices);
	FREE(usbif);
	FREE(ports->ports);
	FREE(ports);
	FREE(bridges->bridges);
	FREE(bridges);
	FREE(switches->switches);
	FREE(switches);

	return 0;
}
