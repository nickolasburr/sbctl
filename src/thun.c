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

	return (((unsigned long) dev_id >> 8) & 0xFF);

on_error:
	*err = 1;

	return -1;
}

/**
 * Get port description.
 */
char *THUN_get_port_description (int *err, io_service_t port) {
	char desc_buf[256];
	char *desc_ptr = NULL;
	CFTypeRef cf_obj;
	CFMutableDictionaryRef dict;
	kern_return_t status;

	*err = 0;

	status = IORegistryEntryCreateCFProperties(port, &dict, kCFAllocatorDefault, kNilOptions);

	if (status != KERN_SUCCESS) {
		goto on_error;
	}

	cf_obj = (CFStringRef) CFDictionaryGetValue(dict, CFSTR(kIOThunderboltPortDescriptionKey));

	if (cf_obj && CFStringGetCString(cf_obj, desc_buf, 256, CFStringGetSystemEncoding())) {
		desc_ptr = ALLOC(sizeof(desc_buf) + NULL_BYTE);
		copy(desc_ptr, desc_buf);
	}

	CFRelease(cf_obj);

	return desc_ptr;

on_error:
	*err = 1;

	return NULL;
}

/**
 *
 * Thunderbolt bridges.
 *
 */

/**
 * Get total number of PCI-PCI Thunderbolt bridges.
 */
int THUN_get_total_bridges (int *err) {
	int index;
	CFMutableDictionaryRef dict;
	io_iterator_t iter;
	io_service_t bridge;
	kern_return_t status;

	*err = 0;

	dict = IOServiceMatching(kIOPCI2PCIBridgeClassName);

	if (is_null(dict)) {
		goto on_error;
	}

	/**
	 * We need to match against our IOPCITunnelled key.
	 */
	CFDictionarySetValue(dict, CFSTR(kIOPCITunnelledKey), kCFBooleanTrue);

	status = IOServiceGetMatchingServices(kIOMasterPortDefault, dict, &iter);

	if (status != KERN_SUCCESS) {
		goto on_error;
	}

	index = 0;

	while ((bridge = IOIteratorNext(iter))) {
		index++;

		IOObjectRelease(bridge);
	}

	IOObjectRelease(iter);

	return index;

on_error:
	*err = 1;

	return -1;
}

/**
 * Get all PCI-PCI Thunderbolt bridges.
 */
void THUN_get_bridges (int *err, Bridge_T *bridges) {
	int index;
	CFMutableDictionaryRef dict;
	io_iterator_t iter;
	io_service_t bridge;
	kern_return_t status;

	*err = 0;

	dict = IOServiceMatching(kIOPCI2PCIBridgeClassName);

	if (is_null(dict)) {
		goto on_error;
	}

	/**
	 * We need to match against our IOPCITunnelled key.
	 */
	CFDictionarySetValue(dict, CFSTR(kIOPCITunnelledKey), kCFBooleanTrue);

	status = IOServiceGetMatchingServices(kIOMasterPortDefault, dict, &iter);

	if (status != KERN_SUCCESS) {
		goto on_error;
	}

	index = 0;

	while ((bridge = IOIteratorNext(iter))) {
		bridges->bridges[index] = bridge;

		index++;
	}

	IOObjectRelease(iter);

	return;

on_error:
	*err = 1;

	return;
}
