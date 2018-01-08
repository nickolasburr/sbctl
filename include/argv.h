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

#define NUM_CMDS 6
#define LIST_NUM_OPTS 1

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
 * Hex masks for options.
 */
#define MASK_CMD_LIST_OPT_USB ((0xFF << 0x1))

/**
 * ASCII character codes.
 */
#define ASCII_PERCENT 37

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
	Option_T *options;
} Command_T;

static Command_T commands[NUM_CMDS];

int ARGV_get_command_bitmask(const char *);
int ARGV_get_option_bitmask(const char *, const char *, int);
void ARGV_usage(void);

#endif /* SBCTL_ARGV_H */
