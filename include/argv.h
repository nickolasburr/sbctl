/**
 * argv.h
 *
 * Copyright (C) 2017 Nickolas Burr <nickolasburr@gmail.com>
 */

#ifndef SBCTL_ARGV_H
#define SBCTL_ARGV_H

#include "common.h"
#include "error.h"
#include "utils.h"

/**
 * Commands
 */
#define NUM_CMDS 6

/**
 * Hex masks for commands.
 */
#define MASK_CMD_HELP  0x1
#define MASK_CMD_VERS  0x2
#define MASK_CMD_LIST  0x4
#define MASK_CMD_GET   0x8
#define MASK_CMD_SET   0x10
#define MASK_CMD_UNSET 0x20

/**
 * Command options
 */

/**
 * Number of 'sbctl ls' options.
 */
#define NUM_LS_OPTS 5

/**
 * Hex masks for 'sbctl ls' options.
 */
#define MASK_CMD_LIST_OPT_HELP ((0xFF << 0x0))
#define MASK_CMD_LIST_OPT_PCI  ((0xFF << 0x1))
#define MASK_CMD_LIST_OPT_TBL  ((0xFF << 0x2))
#define MASK_CMD_LIST_OPT_THUN ((0xFF << 0x3))
#define MASK_CMD_LIST_OPT_USB  ((0xFF << 0x4))

/**
 * Number of 'sbctl get' options.
 */
#define NUM_GET_OPTS 1

/**
 * Hex masks for 'sbctl get' options.
 */
#define MASK_CMD_GET_OPT_HELP ((0xFE << 0x0))

/**
 * Example usage for 'sbctl get'.
 */
#define EXAMPLE_USAGE_CMD_GET "sbctl get %1"

typedef struct {
	char *value;
	char *alias;
	char *desc;
	int bitmask;
} Option_T;

typedef struct {
	char *value;
	char *alias;
	char *desc;
	int bitmask;
	Option_T (*options)[];
} Command_T;

static Command_T commands[NUM_CMDS];
static Option_T get_opts[NUM_GET_OPTS];
static Option_T ls_opts[NUM_LS_OPTS];

int ARGV_get_command_bitmask(const char *);
int ARGV_get_option_bitmask(const char *, const char *);
void ARGV_init(void);
void ARGV_general_usage(void);
void ARGV_command_usage(const char *);

#endif /* SBCTL_ARGV_H */
