/**
 * usb.c
 *
 * Copyright (C) 2017 Nickolas Burr <nickolasburr@gmail.com>
 */

#include "usb.h"

/**
 * Get device by vendor, product IDs.
 */
io_service_t USB_get_device_by_vendor_product_ids (int *err, int vendor_id, int product_id) {
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
int USB_get_total_devices (int *err) {
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
void USB_get_devices (int *err, io_service_t *devices) {
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
IOUSBDeviceInterface **USB_get_device_interface (int *err, io_service_t *device) {
	IOCFPlugInInterface  **plgif = NULL;
	IOUSBDeviceInterface **devif = NULL;
	IOReturn result;
	SInt32 score;

	*err = 0;

	result = IOCreatePlugInInterfaceForService(*device, kIOUSBDeviceUserClientTypeID, kIOCFPlugInInterfaceID, &plgif, &score);

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
unsigned long long USB_get_bus_frame (int *err, IOUSBDeviceInterface **devif) {
	UInt64 frame;
	AbsoluteTime time;
	IOReturn status;

	*err = 0;

	status = (*devif)->GetBusFrameNumber(devif, &frame, &time);

	if (status != kIOReturnSuccess) {
		goto on_error;
	}

	return (unsigned long long) frame;

on_error:
	*err = 1;

	return -1;
}

/**
 * Get location ID of device.
 */
unsigned long USB_get_device_location_id (int *err, IOUSBDeviceInterface **devif) {
	UInt32 lid;
	IOReturn status;

	*err = 0;

	status = (*devif)->GetLocationID(devif, &lid);

	if (status != kIOReturnSuccess) {
		goto on_error;
	}

	return (unsigned long) lid;

on_error:
	*err = 1;

	return -1;
}

/**
 * Get device ID.
 *
 * @note: The device ID is specific to sbctl registry.
 */
unsigned long USB_get_device_id (int *err, IOUSBDeviceInterface **devif) {
	UInt32 lid;
	IOReturn status;

	*err = 0;

	status = (*devif)->GetLocationID(devif, &lid);

	if (status != kIOReturnSuccess) {
		goto on_error;
	}

	return (((unsigned long) lid >> 8) & 0xFF);

on_error:
	*err = 1;

	return -1;
}

/**
 * Get bus power (in mA) available to USB device.
 */
unsigned long USB_get_device_bus_power (int *err, IOUSBDeviceInterface **devif) {
	UInt32 power;
	IOReturn status;

	*err = 0;

	status = (*devif)->GetDeviceBusPowerAvailable(devif, &power);

	if (status != kIOReturnSuccess) {
		goto on_error;
	}

	return ((unsigned long) power << 1);

on_error:
	*err = 1;

	return -1;
}

/**
 * Get device address.
 */
unsigned long USB_get_device_address (int *err, IOUSBDeviceInterface **devif) {
	UInt16 addr;
	IOReturn status;

	*err = 0;

	status = (*devif)->GetDeviceAddress(devif, &addr);

	if (status != kIOReturnSuccess) {
		goto on_error;
	}

	return (unsigned long) addr;

on_error:
	*err = 1;

	return -1;
}

/**
 * Get device throughput speed.
 */
int USB_get_device_speed (int *err, IOUSBDeviceInterface **devif) {
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

	return -1;
}

/**
 * Get projected device throughput speed as string,
 * which contains max TP, as outlined in USB specs.
 */
char *USB_get_device_speed_per_spec (int *err, int speed) {
	char *speed_spec = NULL;

	*err = 0;

	switch (speed) {
		case kUSBDeviceSpeedLow:
			speed_spec = USB_LOW_SPEED;

			break;
		case kUSBDeviceSpeedFull:
			speed_spec = USB_FULL_SPEED;

			break;
		case kUSBDeviceSpeedHigh:
			speed_spec = USB_HIGH_SPEED;

			break;
		case kUSBDeviceSpeedSuper:
			speed_spec = USB_SUPER_SPEED;

			break;
		case kUSBDeviceSpeedSuperPlus:
			speed_spec = USB_SUPER_SPEED_PLUS;

			break;
		default:
			*err = 1;
	}

	return speed_spec;
}

/**
 * Get device serial number.
 */
char *USB_get_device_serial_number (int *err, io_service_t *device) {
	char serial[256];
	char *serial_ptr = NULL;
	CFMutableDictionaryRef dict;
	CFTypeRef serial_obj;
	io_iterator_t iter;
	kern_return_t status;

	*err = 0;

	status = IORegistryEntryCreateCFProperties(*device, &dict, kCFAllocatorDefault, kNilOptions);

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
 * Get device port number.
 */
unsigned long USB_get_device_port_number (int *err, io_service_t *device) {
	unsigned long port_num;
	CFMutableDictionaryRef dict;
	CFNumberRef pn_obj;
	io_iterator_t iter;
	kern_return_t status;

	*err = 0;

	status = IORegistryEntryCreateCFProperties(*device, &dict, kCFAllocatorDefault, kNilOptions);

	if (status != KERN_SUCCESS) {
		goto on_error;
	}

	pn_obj = (CFNumberRef) CFDictionaryGetValue(dict, CFSTR(kUSBPortNumberKey));

	if (!(pn_obj && CFNumberGetValue(pn_obj, kCFNumberLongType, &port_num))) {
		port_num = -1;
	}

	return port_num;

on_error:
	*err = 1;

	return -1;
}

/**
 * Get device product name.
 */
char *USB_get_device_product_name (int *err, io_service_t *device) {
	char pn[256];
	char *pn_ptr = NULL;
	CFMutableDictionaryRef dict;
	CFTypeRef pn_obj;
	io_iterator_t iter;
	kern_return_t status;

	*err = 0;

	status = IORegistryEntryCreateCFProperties(*device, &dict, kCFAllocatorDefault, kNilOptions);

	if (status != KERN_SUCCESS) {
		goto on_error;
	}

	pn_obj = CFDictionaryGetValue(dict, CFSTR(kUSBProductString));

	if (pn_obj && CFStringGetCString((CFStringRef) pn_obj, pn, 256, CFStringGetSystemEncoding())) {
		pn_ptr = ALLOC(sizeof(pn) + NULL_BYTE);
		copy(pn_ptr, pn);
	}

	return pn_ptr;

on_error:
	*err = 1;

	return NULL;
}

/**
 * Get device vendor name.
 */
char *USB_get_device_vendor_name (int *err, io_service_t *device) {
	char vn[256];
	char *vn_ptr = NULL;
	CFMutableDictionaryRef dict;
	CFTypeRef vn_obj;
	io_iterator_t iter;
	kern_return_t status;

	*err = 0;

	status = IORegistryEntryCreateCFProperties(*device, &dict, kCFAllocatorDefault, kNilOptions);

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
int USB_reset_device (int *err, IOUSBDeviceInterface **devif) {
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
