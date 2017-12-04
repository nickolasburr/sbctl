/**
 * argv.c
 *
 * Copyright (C) 2017 Nickolas Burr <nickolasburr@gmail.com>
 */

#include "argv.h"

static option_t options[] = {
	{
		"list",
		"ls",
		"List available devices.",
		0x1,
	},
	{
		"show",
		NULL,
		"Show information for specific device[s].",
		0x2
	}
};

/**
 * Get bitmask associated with value.
 */
int get_mask_from_value (char *value) {
	int index;

	for (index = 0; index < NUM_OPTS; index += 1) {
		option_t *option = &options[index];

		if (!compare(option->value, value)) {
			return option->mask;
		}

		/**
		 * Check option->alias for possible match, as well.
		 */
		if (!is_null(option->alias) && !compare(option->alias, value)) {
			return option->mask;
		}
	}

	return -1;
}

/**
 * Print usage information.
 */
void usage (void) {
	int index;
	char fvalue[36];

	fprintf(stdout, "Usage: sbctl [OPTIONS]\n\n");
	fprintf(stdout, "Options:\n\n");

	for (index = 0; index < NUM_OPTS; index += 1) {
		option_t *option = &options[index];

		/**
		 * Format option->value string.
		 */
		copy(fvalue, option->value);
		concat(fvalue, ",");

		fprintf(stdout, "%4s%-22s %s: %-24s\n", "", fvalue, option->alias, option->desc);
	}
}
