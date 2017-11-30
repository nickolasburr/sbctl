/**
 * signal.c
 *
 * Copyright (C) 2017 Nickolas Burr <nickolasburr@gmail.com>
 */

#include "signals.h"

/**
 * Run cleanup on SIGHUP or SIGINT.
 */
void on_signal (int signal) {
	switch (signal) {
		case SIGHUP:
			fprintf(stdout, "Caught SIGHUP, exiting...\n");

			looping = 0;
		case SIGINT:
			fprintf(stdout, "Caught SIGINT, exiting...\n");

			looping = 0;
		default:
			return;
	}
}
