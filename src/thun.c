/**
 * thun.c
 *
 * Copyright (C) 2017 Nickolas Burr <nickolasburr@gmail.com>
 */

#include "thun.h"

/**
 * Get the number of Thunderbolt ports.
 */
int THUN_get_total_ports (int *err) {
	int index;
	CFMutableDictionaryRef dict;
	io_iterator_t iter;
	io_service_t device;
	kern_return_t status;

	*err = 0;

	dict = IOServiceMatching("IOThunderboltPort");

	if (is_null(dict)) {
		goto on_error;
	}

	status = IOServiceGetMatchingServices(kIOMasterPortDefault, dict, &iter);

	if (status != KERN_SUCCESS) {
		goto on_error;
	}

	index = 0;

	while ((device = IOIteratorNext(iter))) {
		index++;
	}

	IOObjectRelease(iter);

	return index;

on_error:
	*err = 1;

	return -1;
}


/**
 * Get all port interfaces connecting devices downstream via Thunderbolt.
 */
void THUN_get_ports (int *err, io_service_t *devices) {
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
		devices[index] = port;

		index++;
	}

	IOObjectRelease(iter);

	return;

on_error:
	*err = 1;

	return;
}

/**
 * Get Thunderbolt port adapter type.
 */
unsigned long THUN_get_adapter_type (int *err, io_service_t port) {
	char adapter[256];
	char *adapter_ptr = NULL;
	CFMutableDictionaryRef dict;
	CFTypeRef adapter_obj;
	CFNumberRef adapter_num;
	io_iterator_t iter;
	kern_return_t status;

	*err = 0;

	status = IORegistryEntryCreateCFProperties(port, &dict, kCFAllocatorDefault, kNilOptions);

	if (status != KERN_SUCCESS) {
		goto on_error;
	}

	adapter_obj = CFDictionaryGetValue(dict, CFSTR("Adapter Type"));

	if (!(adapter_obj && CFNumberGetValue((CFNumberRef) adapter_obj, kCFNumberLongType, &adapter_num))) {
		goto on_error;
	}

	return (unsigned long) adapter_num;

on_error:
	*err = 1;

	return -1;
}
