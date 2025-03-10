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
	char *num_arg = NULL;
	char *pci_smt = "pci";
	char *usb_smt = "usb";
	char *usb_speed_spec = NULL;
	/* char *thun_speed_spec = NULL; */
	const char *tb_name = NULL;
	const char *ts_name = NULL;
	const char *ts_vendor = NULL;
	int count, err;
	int index, opt_index;
	int lstbl, lsusb, lspci, lsthun;
	int start, end, entry, total_entries;
	int power, usb_speed, thun_speed;
	unsigned int tb_vers;
	unsigned long address, usb_lid, dev_id;
	unsigned long usb_port, thun_port;
	unsigned long tb_bus, tp_bus, ts_bus;
	unsigned long long frame;
	io_service_t device, port, bridge, swit;
	IOUSBDeviceInterface **devif;
	USB_T *usbif;
	/* Thun_T *thunif; */
	Port_T *ports;
	Bridge_T *bridges;
	Switch_T *switches;

	ARGV_init();

	/**
	 * Name of executable (sbctl).
	 */
	target = argv[0];

	/**
	 * When sbctl is given with no arguments.
	 */
	if (argc < 2) {
		ARGV_general_usage();
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
	 * Handle commands based on bitmask.
	 */
	switch (ARGV_get_command_bitmask(cmd_arg)) {
		/**
		 * 1. sbctl list, ls [OPTIONS]
		 */
		case MASK_CMD_LIST:
			lspci = 0;
			lsusb = 0;
			lstbl = 0;
			lsthun = 0;

			/**
			 * Modifier options given to 'sbctl ls'.
			 */
			for (opt_index = 2; opt_index < argc; opt_index += 1) {
				opt_arg = argv[opt_index];

				if (!is_null(opt_arg)) {
					/**
					 * Handle options based on bitmask.
					 */
					switch (ARGV_get_option_bitmask(cmd_arg, opt_arg)) {
						case MASK_CMD_LIST_OPT_HELP:
							ARGV_command_usage(cmd_arg);
							exit(EXIT_SUCCESS);
						case MASK_CMD_LIST_OPT_PCI:
							lspci = 1;
							break;
						case MASK_CMD_LIST_OPT_USB:
							lsusb = 1;
							break;
						case MASK_CMD_LIST_OPT_TBL:
							lstbl = 1;
							break;
						case MASK_CMD_LIST_OPT_THUN:
							lsthun = 1;
							break;
						default:
							fprintf(
								stdout,
								"%s%1s%s: Invalid option %s\n",
								target,
								"",
								cmd_arg,
								opt_arg
							);
							goto on_error;
					}
				}
			}

			/**
			 * Table header.
			 */
			fprintf(
				stdout,
				lstbl ? TABLE_LIST_HEADER : BASIC_LIST_HEADER
			);

			/**
			 * Entry index counter.
			 */
			count = 0;

			if (lsusb) {
				/**
				 *
				 * USB devices, buses, hubs, etc.
				 *
				 */

				for (index = 0; index < usbif->length; index += 1) {
					/**
					 * Get device object, interface.
					 */
					device = usbif->devices[index];
					devif = USB_get_device_interface(&err, &device);

					if (err == -1) {
						fprintf(
							stderr,
							"Unable to connect, skipping...\n"
						);
						continue;
					}

					fprintf(stdout, "%c", lstbl ? ASCII_PIPE : ASCII_SPACE);
					fprintf(stdout, "%1s%-*.2d", "", 5, ++count);

					/**
					 * USB Spec, Mode.
					 */
					fprintf(stdout, "%1s%-*.4s", "", 6, usb_smt);
					fprintf(stdout, "%1s%-*.4s", "", 6, usb_smt);

					/**
					 * Placeholder for Type (device, hub, bridge, etc.)
					 */
					fprintf(stdout, "%1s%-*.4s", "", 6, COLUMN_LINE);

					/**
					 * Get device locationID for bus number.
					 */
					usb_lid = USB_get_device_location_id(&err, devif);
					assert(!err);

					/**
					 * Format locationID into hex for strtoul.
					 */
					snprintf(bus_buf, 5, "%#lx", usb_lid);
					fprintf(
						stdout,
						"%1s%-*.3d",
						"",
						5,
						(int) strtoul(bus_buf, NULL, 0)
					);

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
						fprintf(stdout, "%1s%-*.3s", "", 6, COLUMN_LINE);
					}

					/**
					 * Get bus power available to device.
					 */
					power = USB_get_device_bus_power(&err, devif);
					assert(!err);

					/**
					 * ex., Power: 250
					 */
					fprintf(stdout, "%1s%-*d", "", 12, power);

					usb_speed = USB_get_device_speed(&err, devif);
					assert(!err);

					usb_speed_spec = USB_get_device_speed_per_spec(&err, usb_speed);
					assert(!err);

					/**
					 * ex., Speed: 480
					 */
					fprintf(stdout, "%1s%-*.5s", "", 14, usb_speed_spec);

					serial = USB_get_device_serial_number(&err, &device);
					assert(!err);

					serial = !is_null(serial) ? serial : COLUMN_LINE;

					/**
					 * ex., Serial: 00000000
					 */
					fprintf(stdout, "%1s%-*.13s", "", 15, serial);

					dev_id = USB_get_device_id(&err, devif);
					assert(!err);

					/**
					 * ex., Device ID: 016
					 */
					fprintf(stdout, "%1s%-*.3lu", "", 11, dev_id);

					vendor = USB_get_device_vendor_name(&err, &device);
					assert(!err);

					vendor = !is_null(vendor) ? vendor : COLUMN_LINE;

					/**
					 * ex., Vendor: Apple
					 */
					fprintf(stdout, "%1s%-*.6s", "", 8, vendor);

					product = USB_get_device_product_name(&err, &device);
					assert(!err);

					product = !is_null(product) ? product : COLUMN_LINE;

					/**
					 * ex., Product Description: Apple Keyboard
					 */
					fprintf(stdout, "%1s%-*.19s", "", 20, product);

					if (lstbl) {
						fprintf(stdout, "|");
					}

					fprintf(stdout, "\n");
					(*devif)->Release(devif);
				}
			} else {
				count += usbif->length;
			}

			if (lsthun) {
				/**
				 *
				 * Thunderbolt ports.
				 *
				 */

				for (index = 0; index < ports->length; index += 1) {
					/**
					 * Get port object.
					 */
					port = ports->ports[index];

					if (lstbl) {
						fprintf(stdout, "|");
					} else {
						fprintf(stdout, "%c", ASCII_SPACE);
					}

					fprintf(stdout, "%1s%-*.2d", "", 5, ++count);

					/**
					 * Thunderbolt Spec, Mode, Type.
					 */
					fprintf(stdout, "%1s%-*.4s", "", 6, pci_smt);
					fprintf(stdout, "%1s%-*.4s", "", 6, THUN_MODE_NAME);
					fprintf(stdout, "%1s%-*.4s", "", 6, THUN_PORT_TYPE);

					tp_bus = THUN_get_port_bus_number(&err, &port);
					assert(!err);

					fprintf(stdout, "%1s%-*.3lu", "", 5, tp_bus);

					/**
					 * Placeholder for Address.
					 */
					fprintf(stdout, "%1s%-*.3s", "", 9, COLUMN_LINE);

					thun_port = THUN_get_port_port_number(&err, &port);
					assert(!err);

					/**
					 * Port number.
					 */
					fprintf(stdout, "%1s%-*.2lu", "", 6, thun_port);

					/**
					 * Placeholder for Power.
					 */
					fprintf(stdout, "%1s%-*s", "", 12, COLUMN_LINE);

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
							fprintf(stdout, "%1s%-*.5s", "", 14, COLUMN_LINE);
					}

					/**
					 * Placeholder for Serial Number.
					 */
					fprintf(stdout, "%1s%-*.13s", "", 15, COLUMN_LINE);

					dev_id = THUN_get_port_device_id(&err, &port);
					assert(!err);

					/**
					 * Device ID.
					 */
					fprintf(stdout, "%1s%-*.3lu", "", 11, dev_id);

					/**
					 * Placeholder for Vendor.
					 */
					fprintf(stdout, "%1s%-*.6s", "", 8, COLUMN_LINE);

					product = THUN_get_port_description(&err, &port);
					assert(!err);

					fprintf(stdout, "%1s%-*.19s", "", 20, product);

					if (lstbl) {
						fprintf(stdout, "|");
					}

					fprintf(stdout, "\n");
				}

				/**
				 *
				 * PCI-PCI Thunderbolt bridges.
				 *
				 */

				for (index = 0; index < bridges->length; index += 1) {
					/**
					 * Get bridge object.
					 */
					bridge = bridges->bridges[index];

					if (lstbl) {
						fprintf(stdout, "|");
					} else {
						fprintf(stdout, "%c", ASCII_SPACE);
					}

					fprintf(stdout, "%1s%-*.2d", "", 5, ++count);

					/**
					 * Thunderbolt Spec, Mode, Type.
					 */
					fprintf(stdout, "%1s%-*.4s", "", 6, pci_smt);
					fprintf(stdout, "%1s%-*.4s", "", 6, THUN_MODE_NAME);
					fprintf(stdout, "%1s%-*.4s", "", 6, THUN_BRID_TYPE);

					/**
					 * Get bridge bus number.
					 */
					tb_bus = THUN_get_bridge_bus_number(&err, &bridge);
					assert(!err);

					fprintf(stdout, "%1s%-*.3lu", "", 5, tb_bus);

					/**
					 * Placeholder for Address, Port.
					 */
					fprintf(stdout, "%1s%-*.3s", "", 9, COLUMN_LINE);
					fprintf(stdout, "%1s%-*.3s", "", 6, COLUMN_LINE);

					/**
					 * Placeholders for Power, Speed, Serial Number, and Device ID.
					 */
					fprintf(stdout, "%1s%-*s", "", 12, COLUMN_LINE);
					fprintf(stdout, "%1s%-*.5s", "", 14, COLUMN_LINE);
					fprintf(stdout, "%1s%-*.13s", "", 15, COLUMN_LINE);
					fprintf(stdout, "%1s%-*.3s", "", 11, COLUMN_LINE);

					/**
					 * Placeholder for Vendor.
					 */
					fprintf(stdout, "%1s%-*.6s", "", 8, COLUMN_LINE);

					/**
					 * Get Thunderbolt bridge name.
					 */
					tb_name = THUN_get_bridge_name(&err, &bridge);
					assert(!err);

					fprintf(stdout, "%1s%-*.19s", "", 20, tb_name);

					if (lstbl) {
						fprintf(stdout, "|");
					}

					fprintf(stdout, "\n");
				}

				/**
				 *
				 * Thunderbolt switches.
				 *
				 */

				for (index = 0; index < switches->length; index += 1) {
					/**
					 * Get switch object.
					 */
					swit = switches->switches[index];

					if (lstbl) {
						fprintf(stdout, "|");
					} else {
						fprintf(stdout, "%c", ASCII_SPACE);
					}

					fprintf(stdout, "%1s%-*.2d", "", 5, ++count);

					/**
					 * Thunderbolt Spec, Mode, Type.
					 */
					fprintf(stdout, "%1s%-*.4s", "", 6, pci_smt);
					fprintf(stdout, "%1s%-*.4s", "", 6, THUN_MODE_NAME);
					fprintf(stdout, "%1s%-*.4s", "", 6, THUN_SWIT_TYPE);

					ts_bus = THUN_get_switch_bus_number(&err, &swit);
					assert(!err);

					fprintf(stdout, "%1s%-*.3lu", "", 5, ts_bus);

					/**
					 * Placeholder for Address.
					 */
					fprintf(stdout, "%1s%-*.3s", "", 9, COLUMN_LINE);

					thun_port = THUN_get_switch_port_number(&err, &swit);
					assert(!err);

					fprintf(stdout, "%1s%-*.2lu", "", 6, thun_port);

					/**
					 * Placeholder for Power.
					 */
					fprintf(stdout, "%1s%-*s", "", 12, COLUMN_LINE);

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
							fprintf(stdout, "%1s%-*.5s", "", 14, COLUMN_LINE);
					}

					/**
					 * Placeholder for Serial Number.
					 */
					fprintf(stdout, "%1s%-*.13s", "", 15, COLUMN_LINE);

					/**
					 * Get PCI Thunderbolt device ID.
					 */
					dev_id = THUN_get_switch_device_id(&err, &swit);
					assert(!err);

					fprintf(stdout, "%1s%-*.3lu", "", 11, dev_id);

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

					if (lstbl) {
						fprintf(stdout, "|");
					}

					fprintf(stdout, "\n");
				}
			} else {
				count += (ports->length + bridges->length + switches->length);
			}

			if (lstbl) {
				fprintf(stdout, TABLE_LIST_FOOTER);
			}

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
			num_arg = argv[2];

			if (!is_null(num_arg)) {
				/**
				 * Handle options based on bitmask.
				 */
				switch (ARGV_get_option_bitmask(cmd_arg, num_arg)) {
					case MASK_CMD_GET_OPT_HELP:
						ARGV_command_usage(cmd_arg);
						exit(EXIT_SUCCESS);
					default:
						break;
				}
			} else {
				fprintf(
					stderr,
					"%s%1s%s: Missing argument.\n",
					target,
					"",
					cmd_arg
				);
				exit(EXIT_FAILURE);
			}

			/**
			 * Entry index numbers need to be prefixed with % sign.
			 */
			if (num_arg[index] != ASCII_PERCENT) {
				fprintf(
					stderr,
					"%s%1s%s: Invalid format given as argument.\n",
					target,
					"",
					cmd_arg
				);
				exit(EXIT_FAILURE);
			}

			/**
			 * Get up to the first five chars from opt_arg,
			 * and copy its contents to numbers char array.
			 */
			while (
				(length(numbers) < (index + 1))
				&& (number = num_arg[(index + 1)])
			) {
				numbers[index++] = number;
			}

			numbers[index] = '\0';

			/**
			 * Verify numbers represents valid integer.
			 */
			if (!is_numeric(numbers)) {
				fprintf(
					stderr,
					"%s%1s%s: %s is not a valid number.\n",
					target,
					"",
					cmd_arg,
					numbers
				);
				exit(EXIT_FAILURE);
			}

			/**
			 * The actual entry index, as an integer.
			 */
			entry = (int) strtol(numbers, NULL, 0);

			/**
			 * Verify entry is in range {1, total_entries}.
			 */
			if (!((entry >= 1) && (entry <= total_entries))) {
				fprintf(
					stderr,
					"%s%1s%s: Invalid entry %s\n",
					target,
					"",
					cmd_arg,
					numbers
				);
				exit(EXIT_FAILURE);
			}

			/**
			 * Index of requested entry, adjusted for
			 * an initial index of 1 instead of 0.
			 */
			index = (entry - 1);

			/**
			 *
			 * USB entry.
			 *
			 */

			start = 0;
			end = (usbif->length - 1);

			if (in_range(index, start, end)) {
				/**
				 * Get device object, interface.
				 */
				device = usbif->devices[index];
				devif = USB_get_device_interface(&err, &device);
				assert(!err);

				product = USB_get_device_product_name(&err, &device);
				assert(!err);

				product = !is_null(product) ? product : NOT_SPECIFIED;

				fprintf(stdout, "%1s%s\n\n", "", product);
				fprintf(stdout, "%1sSpec: %s\n", "", usb_smt);
				fprintf(stdout, "%1sMode: %s\n", "", usb_smt);
				fprintf(stdout, "%1sType: %s\n", "", COLUMN_LINE);

				/**
				 * Separate into new block.
				 */
				fprintf(stdout, "\n");

				/**
				 * Get device locationID for bus number.
				 */
				usb_lid = USB_get_device_location_id(&err, devif);
				assert(!err);

				/**
				 * Format locationID into hex for strtoul.
				 */
				snprintf(bus_buf, 5, "%#lx", usb_lid);
				fprintf(
					stdout,
					"%1sBus: %-*.3d\n",
					"",
					3,
					(int) strtoul(bus_buf, NULL, 0)
				);

				/**
				 * Get device assigned address.
				 */
				address = USB_get_device_address(&err, devif);
				assert(!err);

				/**
				 * ex., Address: 001
				 */
				fprintf(stdout, "%1sAddress: %-*.3lu\n", "", 3, address);

				/**
				 * Get device port number.
				 */
				usb_port = USB_get_device_port_number(&err, &device);
				assert(!err);

				/**
				 * ex., Port: 02
				 */
				if (!is_error((int) usb_port, -1)) {
					fprintf(stdout, "%1sPort: %-*.2lu\n", "", 3, usb_port);
				} else {
					fprintf(stdout, "%1sPort: %-*.3s\n", "", 3, COLUMN_LINE);
				}

				/**
				 * Get bus power available to device.
				 */
				power = USB_get_device_bus_power(&err, devif);
				assert(!err);

				/**
				 * ex., Power: 250mA
				 */
				fprintf(
					stdout,
					"%1sPower: %*d%s\n",
					"",
					(snprintf(0, 0, "%+d", power) - 1),
					power,
					BUS_POWER_UNITS
				);

				usb_speed = USB_get_device_speed(&err, devif);
				assert(!err);

				usb_speed_spec = USB_get_device_speed_per_spec(&err, usb_speed);
				assert(!err);

				/**
				 * ex., Speed: 480Mbps [High]
				 */
				fprintf(
					stdout,
					"%1sSpeed: %s%s",
					"",
					usb_speed_spec,
					DEV_SPEED_UNITS
				);

				switch (usb_speed) {
					case kUSBDeviceSpeedLow:
						fprintf(stdout, "%1s%s\n", "", "[Low]");
						break;
					case kUSBDeviceSpeedFull:
						fprintf(stdout, "%1s%s\n", "", "[Full]");
						break;
					case kUSBDeviceSpeedHigh:
						fprintf(stdout, "%1s%s\n", "", "[High]");
						break;
					case kUSBDeviceSpeedSuper:
						fprintf(stdout, "%1s%s\n", "", "[Super]");
						break;
					case kUSBDeviceSpeedSuperPlus:
						fprintf(stdout, "%1s%s\n", "", "[Super+]");
						break;
					default:
						fprintf(stdout, "%1s%s\n", "", "[Unknown]");
				}

				/**
				 * Separate into new block.
				 */
				fprintf(stdout, "\n");

				serial = USB_get_device_serial_number(&err, &device);
				assert(!err);

				serial = !is_null(serial) ? serial : COLUMN_LINE;

				/**
				 * ex., Serial: 00000000
				 */
				fprintf(stdout, "%1sSerial: %s\n", "", serial);

				dev_id = USB_get_device_id(&err, devif);
				assert(!err);

				/**
				 * ex., Device ID: 016
				 */
				fprintf(stdout, "%1sDevice ID: %lu\n", "", dev_id);

				vendor = USB_get_device_vendor_name(&err, &device);
				assert(!err);

				vendor = !is_null(vendor) ? vendor : COLUMN_LINE;

				/**
				 * ex., Vendor: Apple
				 */
				fprintf(stdout, "%1sVendor: %s\n", "", vendor);
				break;
			}

			/**
			 *
			 * Thunderbolt port entry.
			 *
			 */

			start = (end + 1);
			end = ((start + ports->length) - 1);

			if (in_range(index, start, end)) {
				index = ((entry - start) - 1);

				/**
				 * Get port object.
				 */
				port = ports->ports[index];

				product = THUN_get_port_description(&err, &port);
				assert(!err);

				product = !is_null(product) ? product : NOT_SPECIFIED;

				fprintf(stdout, "%1s%s\n\n", "", product);
				fprintf(stdout, "%1sSpec: %s\n", "", pci_smt);
				fprintf(stdout, "%1sMode: %s\n", "", THUN_MODE_NAME);
				fprintf(stdout, "%1sType: %s\n", "", THUN_PORT_TYPE);

				/**
				 * Separate into new block.
				 */
				fprintf(stdout, "\n");

				tp_bus = THUN_get_port_bus_number(&err, &port);
				assert(!err);

				fprintf(stdout, "%1sBus: %-*.3lu\n", "", 3, tp_bus);

				/**
				 * Placeholder for Address.
				 */
				fprintf(stdout, "%1sAddress: %-*.3s\n", "", 3, COLUMN_LINE);

				thun_port = THUN_get_port_port_number(&err, &port);
				assert(!err);

				/**
				 * Port number.
				 */
				fprintf(stdout, "%1sPort: %-*.2lu\n", "", 2, thun_port);

				/**
				 * Placeholder for Power.
				 */
				fprintf(stdout, "%1sPower: %-*s\n", "", 3, COLUMN_LINE);

				tb_vers = THUN_get_port_thunderbolt_version(&err, &port);
				assert(!err);

				/**
				 * Show speed rating based on Thunderbolt version.
				 */
				switch (tb_vers) {
					case THUN_V1:
						fprintf(stdout, "%1sSpeed: %-*.5d [Gen 1]\n", "", 5, THUN_V1_SPEED);
						break;
					case THUN_V2:
						fprintf(stdout, "%1sSpeed: %-*.5d [Gen 2]\n", "", 5, THUN_V2_SPEED);
						break;
					case THUN_V3:
						fprintf(stdout, "%1sSpeed: %-*.5d [Gen 3]\n", "", 5, THUN_V3_SPEED);
						break;
					default:
						fprintf(stdout, "%1sSpeed: %-*.5s\n", "", 5, COLUMN_LINE);
				}

				/**
				 * Separate into new block.
				 */
				fprintf(stdout, "\n");

				/**
				 * Placeholder for Serial Number.
				 */
				fprintf(stdout, "%1sSerial: %s\n", "", COLUMN_LINE);

				dev_id = THUN_get_port_device_id(&err, &port);
				assert(!err);

				/**
				 * Device ID.
				 */
				fprintf(stdout, "%1sDevice ID: %-*.3lu\n", "", 3, dev_id);

				/**
				 * Placeholder for Vendor.
				 */
				fprintf(
					stdout,
					"%1sVendor: %s\n",
					"",
					COLUMN_LINE
				);
				break;
			}

			/**
			 *
			 * Thunderbolt bridge entry.
			 *
			 */

			start = (end + 1);
			end = ((start + bridges->length) - 1);

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

				fprintf(stdout, "%1s%s\n\n", "", tb_name);
				fprintf(stdout, "%1sSpec: %s\n", "", pci_smt);
				fprintf(stdout, "%1sMode: %s\n", "", THUN_MODE_NAME);
				fprintf(stdout, "%1sType: %s\n", "", THUN_BRID_TYPE);

				/**
				 * Separate into new block.
				 */
				fprintf(stdout, "\n");

				/**
				 * Get bridge bus number.
				 */
				tb_bus = THUN_get_bridge_bus_number(&err, &bridge);
				assert(!err);

				fprintf(stdout, "%1sBus: %-*.3lu\n", "", 3, tb_bus);

				/**
				 * Placeholder for Address, Port, Power, Speed.
				 */
				fprintf(stdout, "%1sAddress: %-*.3s\n", "", 3, COLUMN_LINE);
				fprintf(stdout, "%1sPort: %-*.3s\n", "", 3, COLUMN_LINE);
				fprintf(stdout, "%1sPower: %-*.3s\n", "", 3, COLUMN_LINE);
				fprintf(stdout, "%1sSpeed: %-*.3s\n", "", 3, COLUMN_LINE);

				/**
				 * Separate into new block.
				 */
				fprintf(stdout, "\n");

				/**
				 * Placeholders for Serial Number, Device ID, and Vendor.
				 */
				fprintf(stdout, "%1sSerial: %-*.3s\n", "", 3, COLUMN_LINE);
				fprintf(stdout, "%1sDevice ID: %-*.3s\n", "", 3, COLUMN_LINE);
				fprintf(stdout, "%1sVendor: %-*.3s\n", "", 3, COLUMN_LINE);

				break;
			}

			/**
			 *
			 * Thunderbolt switch entry.
			 *
			 */

			start = (end + 1);
			end = ((start + switches->length) - 1);

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

				fprintf(
					stdout,
					"%1s%s\n\n%1sSpec:%s\n%1sMode:%s\n%1sType:%s\n",
					"",
					ts_name,
					"",
					pci_smt,
					"",
					THUN_MODE_NAME,
					"",
					THUN_SWIT_TYPE
				);

				/**
				 * Separate into new block.
				 */
				fprintf(stdout, "\n");

				/**
				 * Get switch bus number.
				 */
				ts_bus = THUN_get_switch_bus_number(&err, &swit);
				assert(!err);

				fprintf(stdout, "%1sBus: %-*.3lu\n", "", 3, ts_bus);

				/**
				 * Placeholder for Address.
				 */
				fprintf(stdout, "%1sAddress: %-*.3s\n", "", 3, COLUMN_LINE);

				thun_port = THUN_get_switch_port_number(&err, &swit);
				assert(!err);

				fprintf(stdout, "%1sPort: %-*.2lu\n", "", 3, thun_port);

				/**
				 * Placeholder for Power.
				 */
				fprintf(stdout, "%1sPower: %-*.3s\n", "", 3, COLUMN_LINE);

				tb_vers = THUN_get_switch_thunderbolt_version(&err, &swit);
				assert(!err);

				/**
				 * Show speed rating based on Thunderbolt version.
				 */
				switch (tb_vers) {
					case THUN_V1:
						fprintf(stdout, "%1sSpeed: %-*.5d [Gen 1]\n", "", 5, THUN_V1_SPEED);
						break;
					case THUN_V2:
						fprintf(stdout, "%1sSpeed: %-*.5d [Gen 2]\n", "", 5, THUN_V2_SPEED);
						break;
					case THUN_V3:
						fprintf(stdout, "%1sSpeed: %-*.5d [Gen 3]\n", "", 5, THUN_V3_SPEED);
						break;
					default:
						fprintf(stdout, "%1sSpeed: %-*.5s\n", "", 5, COLUMN_LINE);
				}

				/**
				 * Separate into new block.
				 */
				fprintf(stdout, "\n");

				/**
				 * Placeholder for Serial Number.
				 */
				fprintf(stdout, "%1sSerial: %-*.3s\n", "", 3, COLUMN_LINE);

				/**
				 * Get PCI Thunderbolt device ID.
				 */
				dev_id = THUN_get_switch_device_id(&err, &swit);
				assert(!err);

				fprintf(stdout, "%1sDevice ID: %-*.3lu\n", "", 3, dev_id);

				/**
				 * Get PCI Thunderbolt switch vendor.
				 */
				ts_vendor = THUN_get_switch_vendor(&err, &swit);
				assert(!err);

				fprintf(
					stdout,
					"%1sVendor: %s\n",
					"",
					ts_vendor
				);
				break;
			}

			fprintf(
				stderr,
				"ERROR: The requested device could not be located.\n"
			);
			break;
		/**
		 * 3. sbctl set [OPTIONS]
		 *
		 * @todo: Build out set functionality.
		 */
		case MASK_CMD_SET:
			fprintf(stdout, "This command is not yet implemented.\n");
			break;
		/**
		 * 4. sbctl help
		 */
		case MASK_CMD_HELP:
			ARGV_general_usage();
			break;
		/**
		 * 5. sbctl version
		 */
		case MASK_CMD_VERS:
			fprintf(stdout, "%s\n", SBCTL_VERSION);
			break;
		default:
			fprintf(
				stderr,
				"Invalid option %s\n\n",
				cmd_arg
			);
			ARGV_general_usage();
			exit(EXIT_FAILURE);
	}

	/**
	 * Run cleanup tasks.
	 */
	FREE_ALL(
		usbif->devices,
		usbif,
		ports->ports,
		ports,
		bridges->bridges,
		bridges,
		switches->switches,
		switches
	);
	return 0;

on_error:
	FREE_ALL(
		usbif->devices,
		usbif,
		ports->ports,
		ports,
		bridges->bridges,
		bridges,
		switches->switches,
		switches
	);
	return -1;
}
