/**
 * serial.h
 *
 * Copyright (C) 2017 Nickolas Burr <nickolasburr@gmail.com>
 */

#ifndef USB_SERIAL_H
#define USB_SERIAL_H

#include <CoreFoundation/CoreFoundation.h>
#include <IOKit/usb/IOUSBLib.h>
#include <IOKit/IOKitLib.h>
#include <IOKit/IOCFPlugIn.h>
#include <IOKit/hid/IOHIDKeys.h>
#include "common.h"

io_service_t get_usb_device (int *, int, int);
IOUSBDeviceInterface **get_usb_device_interface(int *, io_service_t);
int get_bus_power(int *, IOUSBDeviceInterface **);
int get_device_speed(int *, IOUSBDeviceInterface **);
char *get_serial_number(int *, io_service_t);

#endif /* USB_SERIAL_H */
