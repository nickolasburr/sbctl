/**
 * argv.c
 *
 * Copyright (C) 2017 Nickolas Burr <nickolasburr@gmail.com>
 */

#include "argv.h"

static Command_T commands[] = {
	{
		"list",
		"ls",
		"List all known serial devices.",
		MASK_CMD_LIST,
	},
	{
		"get",
		NULL,
		"Get information specific to device.",
		MASK_CMD_GET,
	},
	{
		"set",
		NULL,
		"Set, modify properties of specific device.",
		MASK_CMD_SET,
	},
};

/**
 * Get command bitmask.
 */
int ARGV_get_command_bitmask (char *value) {
	int index;

	for (index = 0; index < NUM_CMDS; index += 1) {
		Command_T *command = &commands[index];

		if (!compare(command->value, value)) {
			return command->bitmask;
		}

		/**
		 * Check command->alias for possible match, as well.
		 */
		if (!is_null(command->alias) && !compare(command->alias, value)) {
			return command->bitmask;
		}
	}

	return -1;
}

/**
 * Get option bitmask.
 *
 * @todo: Finish building this out.
 */
int ARGV_get_option_bitmask (char *value) {
	int index;
	return 0;
}

/**
 * Print usage information.
 */
void ARGV_usage (void) {
	int index;
	char fvalue[36];
	char *space = " ";

	fprintf(stdout, "Usage: sbctl <COMMAND> [OPTIONS]\n\n");
	fprintf(stdout, "Commands:\n\n");

	for (index = 0; index < NUM_CMDS; index += 1) {
		Command_T *command = &commands[index];

		/**
		 * Format command->value string.
		 */
		copy(fvalue, command->value);

		/**
		 * If command->alias is non-null,
		 * format fvalue to include alias.
		 */
		if (!is_null(command->alias)) {
			concat(fvalue, ",");
		} else {
			command->alias = "";
			space = "";
		}

		fprintf(stdout, "%4s%-3s%s%s: %-24s\n", "", fvalue, space, command->alias, command->desc);
	}

	fprintf(stdout, "\nFor a list of specific command options: sbctl <COMMAND> [-h|--help]\n");
}
