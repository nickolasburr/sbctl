/**
 * serial.h
 *
 * Copyright (C) 2017 Nickolas Burr <nickolasburr@gmail.com>
 */

#ifndef SBCTL_SERIAL_H
#define SBCTL_SERIAL_H

#include <CoreFoundation/CoreFoundation.h>
#include <IOKit/IOKitLib.h>
#include <IOKit/IOCFPlugIn.h>
#include <IOKit/hid/IOHIDKeys.h>
#include "common.h"

typedef struct {
	io_service_t *devices;
	int length;
} Serial_T;

#endif /* SBCTL_SERIAL_H */
