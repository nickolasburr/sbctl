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

#define NUM_CMDS 2

/**
 * Hex masks for commands, options.
 */
#define MASK_CMD_HELP 0x1
#define MASK_CMD_VERS 0x2
#define MASK_CMD_LIST 0x4
#define MASK_CMD_GET  0x8
#define MASK_CMD_SET  0x10

typedef struct {
	char *value;
	char *alias;
	char *desc;
	int bitmask;
} option_t;

typedef struct {
	char *value;
	char *alias;
	char *desc;
	int bitmask;
	option_t *options;
} command_t;

static command_t commands[NUM_CMDS];

int get_command_bitmask(char *);
int get_option_bitmask(char *);
void usage(void);

#endif /* SBCTL_ARGV_H */
