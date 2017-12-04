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

#define NUM_OPTS 2

typedef struct {
	char *value;
	char *alias;
	char *desc;
	int mask;
} option_t;

static option_t options[NUM_OPTS];

int get_mask_from_value(char *);
void usage(void);

#endif /* SBCTL_ARGV_H */
