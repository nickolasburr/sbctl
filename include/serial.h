/**
 * serial.h
 *
 * Copyright (C) 2017 Nickolas Burr <nickolasburr@gmail.com>
 */

#ifndef SBCTL_SERIAL_H
#define SBCTL_SERIAL_H

#include <CoreFoundation/CoreFoundation.h>
#include <IOKit/usb/IOUSBLib.h>
#include <IOKit/IOKitLib.h>
#include <IOKit/IOCFPlugIn.h>
#include <IOKit/hid/IOHIDKeys.h>
#include "common.h"

typedef struct {
	io_service_t *devices;
	int total;
} SerialDeviceInterface;

int get_num_usb_devices(int *);
void get_usb_devices(int *, io_service_t *);
io_service_t get_usb_device(int *, int, int);
IOUSBDeviceInterface **get_usb_device_interface(int *, io_service_t);
long long get_bus_frame(int *, IOUSBDeviceInterface **);
long get_bus_power(int *, IOUSBDeviceInterface **);
long get_device_address(int *, IOUSBDeviceInterface **);
int get_device_speed(int *, IOUSBDeviceInterface **);
char *get_serial_number(int *, io_service_t);
int reset_device(int *, IOUSBDeviceInterface **);

#endif /* SBCTL_SERIAL_H */
