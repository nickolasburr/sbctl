/**
 * thun.c
 *
 * Copyright (C) 2017 Nickolas Burr <nickolasburr@gmail.com>
 */

#include "thun.h"

/**
 *
 * Thunderbolt ports.
 *
 */

/**
 * Get total number of PCI Thunderbolt ports.
 */
int THUN_get_total_ports (int *err) {
	int index;
	CFMutableDictionaryRef dict;
	io_iterator_t iter;
	io_service_t port;
	kern_return_t status;

	*err = 0;

	dict = IOServiceMatching(kIOThunderboltPortClassName);

	if (is_null(dict)) {
		goto on_error;
	}

	status = IOServiceGetMatchingServices(kIOMasterPortDefault, dict, &iter);

	if (status != KERN_SUCCESS) {
		goto on_error;
	}

	index = 0;

	while ((port = IOIteratorNext(iter))) {
		index++;

		IOObjectRelease(port);
	}

	IOObjectRelease(iter);

	return index;

on_error:
	*err = 1;

	return -1;
}

/**
 * Get all Thunderbolt ports.
 */
void THUN_get_ports (int *err, Port_T *ports) {
	int index;
	CFMutableDictionaryRef dict;
	io_iterator_t iter;
	io_service_t port;
	kern_return_t status;

	*err = 0;

	dict = IOServiceMatching(kIOThunderboltPortClassName);

	if (is_null(dict)) {
		goto on_error;
	}

	status = IOServiceGetMatchingServices(kIOMasterPortDefault, dict, &iter);

	if (status != KERN_SUCCESS) {
		goto on_error;
	}

	index = 0;

	while ((port = IOIteratorNext(iter))) {
		ports->ports[index] = port;

		index++;
	}

	IOObjectRelease(iter);

	return;

on_error:
	*err = 1;

	return;
}

/**
 * Get port number.
 */
unsigned long THUN_get_port_number (int *err, io_service_t port) {
	unsigned long port_num;
	CFNumberRef pn_obj;
	CFMutableDictionaryRef dict;
	io_iterator_t iter;
	kern_return_t status;

	*err = 0;

	status = IORegistryEntryCreateCFProperties(port, &dict, kCFAllocatorDefault, kNilOptions);

	if (status != KERN_SUCCESS) {
		goto on_error;
	}

	pn_obj = (CFNumberRef) CFDictionaryGetValue(dict, CFSTR(kIOThunderboltPortNumberKey));

	if (!(pn_obj && CFNumberGetValue(pn_obj, kCFNumberLongType, &port_num))) {
		goto on_error;
	}

	CFRelease(pn_obj);
	IOObjectRelease(iter);

	return port_num;

on_error:
	*err = 1;

	return -1;
}

/**
 * Get port number.
 */
unsigned long THUN_get_port_device_id (int *err, io_service_t port) {
	unsigned long dev_id;
	CFNumberRef di_obj;
	CFMutableDictionaryRef dict;
	io_iterator_t iter;
	kern_return_t status;

	*err = 0;

	status = IORegistryEntryCreateCFProperties(port, &dict, kCFAllocatorDefault, kNilOptions);

	if (status != KERN_SUCCESS) {
		goto on_error;
	}

	di_obj = (CFNumberRef) CFDictionaryGetValue(dict, CFSTR(kIOThunderboltPortDeviceIDKey));

	if (!(di_obj && CFNumberGetValue(di_obj, kCFNumberLongType, &dev_id))) {
		goto on_error;
	}

	CFRelease(di_obj);
	IOObjectRelease(iter);

	return (((unsigned long) dev_id >> 8) & 0xFF);

on_error:
	*err = 1;

	return -1;
}
