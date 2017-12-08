/**
 * thun.h
 *
 * Copyright (C) 2017 Nickolas Burr <nickolasburr@gmail.com>
 */

#ifndef SBCTL_THUN_H
#define SBCTL_THUN_H

#include "common.h"
#include "serial.h"

#ifndef kIOThunderboltPortClassName
#define kIOThunderboltPortClassName "IOThunderboltPort"
#endif

typedef Serial_T Thun_T;

int THUN_get_total_ports(int *);
void THUN_get_ports(int *, io_service_t *);
unsigned long THUN_get_adapter_type(int *, io_service_t);

#endif /* SBCTL_THUN_H */
