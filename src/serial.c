/**
 * serial.c
 *
 * Copyright (C) 2017 Nickolas Burr <nickolasburr@gmail.com>
 */

#include "serial.h"

/**
 * Get device by vendor, product IDs.
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
 * Get the number of USB devices available.
 */
int get_total_usb_devices (int *err) {
	int index;
	CFMutableDictionaryRef mdict, dict;
	io_iterator_t iter;
	io_service_t dev;
	kern_return_t status;

	*err = 0;

	mdict = IOServiceMatching(kIOUSBDeviceClassName);

	if (is_null(mdict)) {
		goto on_error;
	}

	status = IOServiceGetMatchingServices(kIOMasterPortDefault, mdict, &iter);

	if (status != KERN_SUCCESS) {
		goto on_error;
	}

	index = 0;

	while ((dev = IOIteratorNext(iter))) {
		index++;
	}

	IOObjectRelease(iter);

	return index;

on_error:
	*err = 1;

	return -1;
}

/**
 * Get all USB devices accessible in IORegistry.
 */
void get_usb_devices (int *err, io_service_t *devices) {
	int index;
	CFMutableDictionaryRef mdict, dict;
	io_iterator_t iter;
	io_service_t dev;
	kern_return_t status;

	*err = 0;

	mdict = IOServiceMatching(kIOUSBDeviceClassName);

	if (is_null(mdict)) {
		goto on_error;
	}

	status = IOServiceGetMatchingServices(kIOMasterPortDefault, mdict, &iter);

	if (status != KERN_SUCCESS) {
		goto on_error;
	}

	index = 0;

	while ((dev = IOIteratorNext(iter))) {
		devices[index] = dev;

		index++;
	}

	IOObjectRelease(iter);

	return;

on_error:
	*err = 1;

	return;
}

/**
 * Get device interface.
 */
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
 * Get current bus frame.
 */
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
 * Get bus power (in mA) available to USB device.
 */
long get_bus_power (int *err, IOUSBDeviceInterface **devif) {
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
 * Get device address.
 */
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

/**
 * Get device throughput speed.
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

/**
 * Get ID of the connected bus.
 *
 * @note: This does not work properly, as bus information is
 *        opaque in newer macOS versions. Likely will discard
 *        if a proper workaround can't be implemented.
 */
char *get_bus_id (int *err, io_service_t device) {
	char bus[256];
	char *bus_ptr = NULL;
	CFMutableDictionaryRef dict;
	CFTypeRef bus_obj;
	io_iterator_t iter;
	kern_return_t status;

	*err = 0;

	status = IORegistryEntryCreateCFProperties(device, &dict, kCFAllocatorDefault, kNilOptions);

	if (status != KERN_SUCCESS) {
		goto on_error;
	}

	bus_obj = CFDictionaryGetValue(dict, CFSTR(kUSBBusID));

	if (bus_obj && CFStringGetCString((CFStringRef) bus_obj, bus, 256, CFStringGetSystemEncoding())) {
		bus_ptr = ALLOC(sizeof(bus) + NULL_BYTE);
		copy(bus_ptr, bus);
	}

	return bus_ptr;

on_error:
	*err = 1;

	return NULL;
}

/**
 * Get device serial number.
 */
char *get_device_serial_number (int *err, io_service_t device) {
	char serial[256];
	char *serial_ptr = NULL;
	CFMutableDictionaryRef dict;
	CFTypeRef serial_obj;
	io_iterator_t iter;
	kern_return_t status;

	*err = 0;

	status = IORegistryEntryCreateCFProperties(device, &dict, kCFAllocatorDefault, kNilOptions);

	if (status != KERN_SUCCESS) {
		goto on_error;
	}

	serial_obj = CFDictionaryGetValue(dict, CFSTR(kIOHIDSerialNumberKey))
	           ? CFDictionaryGetValue(dict, CFSTR(kIOHIDSerialNumberKey))
	           : CFDictionaryGetValue(dict, CFSTR(kUSBSerialNumberString));

	if (serial_obj && CFStringGetCString((CFStringRef) serial_obj, serial, 256, CFStringGetSystemEncoding())) {
		serial_ptr = ALLOC(sizeof(serial) + NULL_BYTE);
		copy(serial_ptr, serial);
	}

	return serial_ptr;

on_error:
	*err = 1;

	return NULL;
}

/**
 * Get device vendor name.
 */
char *get_device_vendor_name (int *err, io_service_t device) {
	char vn[256];
	char *vn_ptr = NULL;
	CFMutableDictionaryRef dict;
	CFTypeRef vn_obj;
	io_iterator_t iter;
	kern_return_t status;

	*err = 0;

	status = IORegistryEntryCreateCFProperties(device, &dict, kCFAllocatorDefault, kNilOptions);

	if (status != KERN_SUCCESS) {
		goto on_error;
	}

	vn_obj = CFDictionaryGetValue(dict, CFSTR(kUSBVendorString));

	if (vn_obj && CFStringGetCString((CFStringRef) vn_obj, vn, 256, CFStringGetSystemEncoding())) {
		vn_ptr = ALLOC(sizeof(vn) + NULL_BYTE);
		copy(vn_ptr, vn);
	}

	return vn_ptr;

on_error:
	*err = 1;

	return NULL;
}

/**
 * Reset device.
 */
int reset_device (int *err, IOUSBDeviceInterface **devif) {
	IOReturn status;

	*err = 0;

	status = (*devif)->ResetDevice(devif);

	if (status != kIOReturnSuccess) {
		goto on_error;
	}

	return 0;

on_error:
	*err = 1;

	return -1;
}
