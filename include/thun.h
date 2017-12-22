/**
 * thun.h
 *
 * Copyright (C) 2017 Nickolas Burr <nickolasburr@gmail.com>
 */

#ifndef SBCTL_THUN_H
#define SBCTL_THUN_H

#include "common.h"
#include "serial.h"

#define THUN_V1 0x1
#define THUN_V2 0x2
#define THUN_V3 0x3

#define THUN_V1_SPEED 10000
#define THUN_V2_SPEED 20000
#define THUN_V3_SPEED 40000

#define kIOPCITunnelledKey "IOPCITunnelled"
#define kIOPCITunnelControllerIDKey "IOPCITunnelControllerID"
#define kIOPCIBridgeNameKey "IOName"
#define kIOPCIBridgeClassName "IOPCIDevice"
#define kIOPCIBridgeBusRegisterKey "reg"

#define kIOThunderboltPortClassName "IOThunderboltPort"
#define kIOThunderboltPortDeviceIDKey "Device ID"
#define kIOThunderboltPortDescriptionKey "Description"
#define kIOThunderboltPortPortNumberKey "Port Number"
#define kIOThunderboltPortNativeHostInterfaceName "NHI0"
#define kIOThunderboltPortThunderboltVersionKey "Thunderbolt Version"

#define kIOThunderboltSwitchType1ClassName "IOThunderboltSwitchType1"
#define kIOThunderboltSwitchType2ClassName "IOThunderboltSwitchType2"
#define kIOThunderboltSwitchDeviceModelNameKey "Device Model Name"
#define kIOThunderboltSwitchDeviceVendorNameKey "Device Vendor Name"
#define kIOThunderboltSwitchThunderboltVersionKey "Thunderbolt Version"

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

/**
 * All PCI Thunderbolt services.
 */
int THUN_get_total_all_services(int *);
void THUN_get_all_services(int *, Service_T *);

/**
 * PCI-PCI Thunderbolt bridges.
 */
int THUN_get_total_bridges(int *);
void THUN_get_bridges(int *, Bridge_T *);
char *THUN_get_bridge_name(int *, io_service_t *);
unsigned long THUN_get_bridge_bus_number(int *, io_service_t *);

/**
 * Thunderbolt ports.
 */
int THUN_get_total_ports(int *);
void THUN_get_ports(int *, Port_T *);
unsigned long THUN_get_port_bus_number(int *, io_service_t *);
const char *THUN_get_port_description(int *, io_service_t *);
unsigned long THUN_get_port_device_id(int *, io_service_t *);
unsigned long THUN_get_port_number(int *, io_service_t *);
unsigned int THUN_get_port_thunderbolt_version(int *, io_service_t *);

/**
 * Thunderbolt switches (built-in, external).
 */
int THUN_get_total_all_switches(int *);
int THUN_get_total_type1_switches(int *);
int THUN_get_total_type2_switches(int *);

void THUN_get_all_switches(int *, Switch_T *);
void THUN_get_type1_switches(int *, Switch_T *);
void THUN_get_type2_switches(int *, Switch_T *);

const char *THUN_get_switch_name(int *, io_service_t *);
unsigned int THUN_get_switch_thunderbolt_version(int *, io_service_t *);
const char *THUN_get_switch_vendor(int *, io_service_t *);

#endif /* SBCTL_THUN_H */
