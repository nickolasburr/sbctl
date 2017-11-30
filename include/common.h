/**
 * common.h
 *
 * Copyright (C) 2017 Nickolas Burr <nickolasburr@gmail.com>
 */

#ifndef USB_COMMON_H
#define USB_COMMON_H

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

#define USB_VERSION "1.0.0"

#define _GNU_SOURCE
#define NULL_DEVICE "/dev/null"
#define NOT_FOUND -1
#define NULL_BYTE 1

extern volatile int looping;

#endif /* USB_COMMON_H */
