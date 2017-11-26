/**
 * serial.c
 *
 * Copyright (C) 2017 Nickolas Burr <nickolasburr@gmail.com>
 */

#include "serial.h"

/**
 * Get USB device serial number.
 *
 * @note Adapted from https://goo.gl/T9eXNQ
 */
CFStringRef get_serial_number (int vendor_id, int product_id) {
	CFMutableDictionaryRef mdict, dict;
	CFNumberRef num_ref;
	io_iterator_t iter;
	io_service_t device;
	kern_return_t kr;

	/**
	 * Matching dictionary for class.
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

	if (IOServiceGetMatchingServices(kIOMasterPortDefault, mdict, &iter) != KERN_SUCCESS) {
		return -1;
	}

	if ((device = IOIteratorNext(iter))) {
		if (IORegistryEntryCreateCFProperties(device, &dict, kCFAllocatorDefault, kNilOptions) == KERN_SUCCESS) {
			CFTypeRef obj = CFDictionaryGetValue(dict, CFSTR(kIOHIDSerialNumberKey));

			if (!obj) {
				obj = CFDictionaryGetValue(dict, CFSTR(kUSBSerialNumberString));
			}

			if (obj) {
				return CFStringCreateCopy(kCFAllocatorDefault, (CFStringRef) obj);
			}
		}
	}

	return NULL;
}
