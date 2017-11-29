/**
 * serial.c
 *
 * Copyright (C) 2017 Nickolas Burr <nickolasburr@gmail.com>
 */

#include "serial.h"

IOUSBDeviceInterface **get_usb_device_interface (int *err, io_service_t device) {
	IOCFPlugInInterface  **plgif = NULL;
	IOUSBDeviceInterface **devif = NULL;
	IOReturn result;
	SInt32 score;

	*err = 0;

	result = IOCreatePlugInInterfaceForService(device, kIOUSBDeviceUserClientTypeID, kIOCFPlugInInterfaceID, &plgif, &score);

	if (!(result == kIOReturnSuccess && !is_null(plgif))) {
		goto on_error;
	}

	result = (*plgif)->QueryInterface(plgif, CFUUIDGetUUIDBytes(kIOUSBDeviceInterfaceID), (LPVOID *) &devif);
	(*plgif)->Release(plgif);

	if (!(result == kIOReturnSuccess && !is_null(devif))) {
		goto on_error;
	}

	return devif;

on_error:
	*err = 1;

	return NULL;
}

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
 * Get bus power (in mA) available to USB device.
 */
int get_bus_power (int *err, IOUSBDeviceInterface **devif) {
	UInt32 power;
	IOReturn status;

	*err = 0;

	status = (*devif)->GetDeviceBusPowerAvailable(devif, &power);

	if (status != kIOReturnSuccess) {
		goto on_error;
	}

	return (int) power;

on_error:
	*err = 1;

	return NULL;
}

/**
 * Get USB device throughput speed.
 */
int get_device_speed (int *err, IOUSBDeviceInterface **devif) {
	UInt8 speed;
	IOReturn status;

	*err = 0;

	status = (*devif)->GetDeviceSpeed(devif, &speed);

	if (status != kIOReturnSuccess) {
		goto on_error;
	}

	return (int) speed;

on_error:
	*err = 1;

	return NULL;
}

long get_device_address (int *err, IOUSBDeviceInterface **devif) {
	UInt16 addr;
	IOReturn status;

	*err = 0;

	status = (*devif)->GetDeviceAddress(devif, &addr);

	if (status != kIOReturnSuccess) {
		goto on_error;
	}

	return (long) addr;

on_error:
	*err = 1;

	return NULL;
}

long long get_bus_frame (int *err, IOUSBDeviceInterface **devif) {
	UInt64 frame;
	AbsoluteTime time;
	IOReturn status;

	*err = 0;

	status = (*devif)->GetBusFrameNumber(devif, &frame, &time);

	if (status != kIOReturnSuccess) {
		goto on_error;
	}

	return (long long) frame;

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
	CFTypeRef serial_obj;
	io_iterator_t iter;
	kern_return_t status;

	*err = 0;

	status = IORegistryEntryCreateCFProperties(device, &dict, kCFAllocatorDefault, kNilOptions);

	if (status != KERN_SUCCESS) {
		goto on_error;
	}

	serial_obj = CFDictionaryGetValue(dict, CFSTR(kIOHIDSerialNumberKey));

	if (!serial_obj) {
		serial_obj = CFDictionaryGetValue(dict, CFSTR(kUSBSerialNumberString));
	}

	if (serial_obj && CFStringGetCString((CFStringRef) serial_obj, serial, 256, CFStringGetSystemEncoding())) {
		serial_ptr = ALLOC(sizeof(serial) + NULL_BYTE);
		copy(serial_ptr, serial);
	}

	return serial_ptr;

on_error:
	*err = 1;

	return NULL;
}
