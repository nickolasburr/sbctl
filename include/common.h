/**
 * common.h
 *
 * Copyright (C) 2017 Nickolas Burr <nickolasburr@gmail.com>
 */

#ifndef SBCTL_COMMON_H
#define SBCTL_COMMON_H

#include <ctype.h>
#include <errno.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include "error.h"
#include "mem.h"
#include "utils.h"

#ifdef __APPLE__
#include <limits.h>
#else
#include <linux/limits.h>
#endif

#define SBCTL_VERSION "1.0.0"

#define _GNU_SOURCE
#define NULL_DEVICE "/dev/null"
#define NOT_FOUND -1
#define NULL_BYTE 1

/**
 * Common ASCII character codes.
 */
#define ASCII_PERCENT 37
#define ASCII_SPACE 32

extern volatile int looping;

#endif /* SBCTL_COMMON_H */
