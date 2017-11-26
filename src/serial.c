/**
 * serial.c
 *
 * Copyright (C) 2017 Nickolas Burr <nickolasburr@gmail.com>
 */

#include "serial.h"

/**
 * Get USB device by vendor, product IDs.
 */
io_service_t get_usb_device (int *err, int vendor_id, int product_id) {
	CFMutableDictionaryRef dict;
	CFNumberRef num_ref;
	io_iterator_t iter;
	io_service_t device;
	kern_return_t status;

	*err = 0;

	/**
	 * Matching dictionary for IOUSBDevice class.
	 */
	dict = IOServiceMatching(kIOUSBDeviceClassName);

	num_ref = CFNumberCreate(kCFAllocatorDefault, kCFNumberSInt32Type, &vendor_id);
	CFDictionaryAddValue(dict, CFSTR(kUSBVendorID), num_ref);
	CFRelease(num_ref);

	num_ref = 0;

	num_ref = CFNumberCreate(kCFAllocatorDefault, kCFNumberSInt32Type, &product_id);
	CFDictionaryAddValue(dict, CFSTR(kUSBProductID), num_ref);
	CFRelease(num_ref);

	num_ref = 0;

	/**
	 * Set matching services, get kern status.
	 */
	status = IOServiceGetMatchingServices(kIOMasterPortDefault, dict, &iter);

	if (status != KERN_SUCCESS) {
		goto on_error;
	}

	/**
	 * Get io_service_t device object.
	 */
	device = IOIteratorNext(iter);

	if (!device) {
		goto on_error;
	}

	return device;

on_error:
	*err = 1;

	return NULL;
}

/**
 * Get bus power available for device.
 */
char *get_bus_power (int *err, io_service_t device) {
	char *power_ptr;
	char power[5];
	CFMutableDictionaryRef dict;
	CFTypeRef power_ref;
	kern_return_t status;

	*err = 0;

	status = IORegistryEntryCreateCFProperties(device, &dict, kCFAllocatorDefault, kNilOptions);

	if (status != KERN_SUCCESS) {
		goto on_error;
	}

	power_ref = CFDictionaryGetValue(dict, CFSTR(kUSBDevicePropertyBusPowerAvailable));

	if (!power_ref) {
		goto on_error;
	}

	if (CFStringGetCString((CFStringRef) power_ref, power, 5, CFStringGetSystemEncoding())) {
		power_ptr = ALLOC(sizeof(power) + NULL_BYTE);
		copy(power_ptr, power);

		return power_ptr;
	}

on_error:
	*err = 1;

	return NULL;
}

/**
 * Get USB device serial number.
 *
 * @note Adapted from https://goo.gl/T9eXNQ
 */
char *get_serial_number (int *err, io_service_t device) {
	char serial[256], *serial_ptr;
	CFMutableDictionaryRef dict;
	CFNumberRef num_ref;
	io_iterator_t iter;
	io_service_t device;
	kern_return_t status;

	*err = 0;

	/**
	 * Matching dictionary for IOUSBDevice class.
	 */
	mdict = IOServiceMatching(kIOUSBDeviceClassName);

	num_ref = CFNumberCreate(kCFAllocatorDefault, kCFNumberSInt32Type, &vendor_id);
	CFDictionaryAddValue(mdict, CFSTR(kUSBVendorID), num_ref);
	CFRelease(num_ref);

	num_ref = 0;

	num_ref = CFNumberCreate(kCFAllocatorDefault, kCFNumberSInt32Type, &product_id);
	CFDictionaryAddValue(mdict, CFSTR(kUSBProductID), num_ref);
	CFRelease(num_ref);

	num_ref = 0;

	status = IOServiceGetMatchingServices(kIOMasterPortDefault, mdict, &iter);

	/**
	 * Set marker if an error was encountered.
	 */
	if (status != KERN_SUCCESS) {
		*err = 1;

		return NULL;
	}

	if ((device = IOIteratorNext(iter))) {
		if (IORegistryEntryCreateCFProperties(device, &dict, kCFAllocatorDefault, kNilOptions) == KERN_SUCCESS) {
			CFTypeRef obj = CFDictionaryGetValue(dict, CFSTR(kIOHIDSerialNumberKey));

			if (!obj) {
				obj = CFDictionaryGetValue(dict, CFSTR(kUSBSerialNumberString));
			}

			if (obj && CFStringGetCString((CFStringRef) obj, serial, 256, CFStringGetSystemEncoding())) {
				serial_ptr = ALLOC(sizeof(serial) + NULL_BYTE);
				copy(serial_ptr, serial);

				return serial_ptr;
			}
		}
	}

	return NULL;
}
