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

#ifndef kIOThunderboltPortNumberKey
#define kIOThunderboltPortNumberKey "Port Number"
#endif

#ifndef kIOThunderboltSwitchType1ClassName
#define kIOThunderboltSwitchType1ClassName "IOThunderboltSwitchType1"
#endif

#ifndef kIOThunderboltSwitchType2ClassName
#define kIOThunderboltSwitchType2ClassName "IOThunderboltSwitchType2"
#endif

#ifndef kIOPCITunnelledKey
#define kIOPCITunnelledKey "IOPCITunnelled"
#endif

typedef struct Bridge_T Bridge_T;
typedef struct Device_T Device_T;
typedef struct Port_T Port_T;
typedef struct Switch_T Switch_T;
typedef struct Service_T Service_T;
typedef struct Thun_T Thun_T;

struct Bridge_T {
	io_service_t *bridges;
	int length;
};

struct Device_T {
	io_service_t *devices;
	int length;
};

struct Port_T {
	io_service_t *ports;
	int length;
};

/**
 * @todo: Separate into type1 and type2.
 */
struct Switch_T {
	io_service_t *switches;
	int length;
};

struct Service_T {
	Bridge_T *bridges;
	Device_T *devices;
	Port_T *ports;
	Switch_T *switches;
};

struct Thun_T {
	Service_T *services;
	int length;
};

size_t THUN_get_total_all_services(int *);
void THUN_get_all_services(int *, Service_T *);

int THUN_get_total_ports(int *);
void THUN_get_ports(int *, Port_T *);
unsigned long THUN_get_port_number(int *, io_service_t);

int THUN_get_total_all_switches(int *);
void THUN_get_all_switches(int *, Switch_T *);

int THUN_get_total_type1_switches(int *);
void THUN_get_type1_switches(int *, Switch_T *);

int THUN_get_total_type2_switches(int *);
void THUN_get_type2_switches(int *, Switch_T *);

#endif /* SBCTL_THUN_H */
