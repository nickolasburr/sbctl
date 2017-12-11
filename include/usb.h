/**
 * usb.h
 *
 * Copyright (C) 2017 Nickolas Burr <nickolasburr@gmail.com>
 */

#ifndef SBCTL_USB_H
#define SBCTL_USB_H

#include <IOKit/usb/IOUSBLib.h>
#include "common.h"
#include "serial.h"

#ifndef kUSBPortNumberKey
#define kUSBPortNumberKey "PortNum"
#endif

#ifndef UNITS_BUS_POWER
#define UNITS_BUS_POWER "mA"
#endif

#ifndef USB_LOW_SPEED
#define USB_LOW_SPEED "1.5"
#endif

#ifndef USB_FULL_SPEED
#define USB_FULL_SPEED "12"
#endif

#ifndef USB_HIGH_SPEED
#define USB_HIGH_SPEED "480"
#endif

#ifndef USB_SUPER_SPEED
#define USB_SUPER_SPEED "5120"
#endif

#ifndef USB_SUPER_SPEED_PLUS
#define USB_SUPER_SPEED_PLUS "20480"
#endif

typedef Serial_T USB_T;

IOUSBDeviceInterface **USB_get_device_interface(int *, io_service_t);
io_service_t USB_get_device_by_vendor_product_ids(int *, int, int);
void USB_get_devices(int *, io_service_t *);
int USB_get_total_devices(int *);
unsigned long long USB_get_bus_frame(int *, IOUSBDeviceInterface **);
unsigned long USB_get_bus_power(int *, IOUSBDeviceInterface **);
unsigned long USB_get_device_address(int *, IOUSBDeviceInterface **);
unsigned long USB_get_device_location_id(int *, IOUSBDeviceInterface **);
unsigned long USB_get_device_port_number(int *, io_service_t);
char *USB_get_device_product_name(int *, io_service_t);
char *USB_get_device_serial_number(int *, io_service_t);
int USB_get_device_speed(int *, IOUSBDeviceInterface **);
char *USB_get_device_speed_as_spec(int *, int);
char *USB_get_device_vendor_name(int *, io_service_t);
int USB_reset_device(int *, IOUSBDeviceInterface **);

#endif /* SBCTL_USB_H */
