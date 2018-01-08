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
		{
			"--usb",
			"-U",
			"List USB devices, buses, hubs, etc.",
			MASK_CMD_LIST_OPT_USB
		},
	},
	{
		"get",
		"show",
		"Get information about device.",
		MASK_CMD_GET,
	},
	{
		"set",
		NULL,
		"Set, modify properties of device.",
		MASK_CMD_SET,
	},
	{
		"unset",
		NULL,
		"Remove properties from device.",
		MASK_CMD_UNSET,
	},
	{
		"help",
		NULL,
		"Show usage information.",
		MASK_CMD_HELP,
	},
	{
		"version",
		NULL,
		"Show current release version.",
		MASK_CMD_VERS,
	},
};

/**
 * Get command bitmask.
 */
int ARGV_get_command_bitmask (const char *value) {
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
int ARGV_get_option_bitmask (const char *cmd, const char *value, int length) {
	int index, xindex;
	Command_T *command = NULL;
	Option_T *options = NULL,
	         *option = NULL;

	for (index = 0; index < NUM_CMDS; index += 1) {
		command = &commands[index];

		if (!compare(command->value, value)) {
			options = command->options;

			for (xindex = 0; xindex < length; xindex += 1) {
				option = &options[xindex];

				if (!compare(option->value, value)) {
					return option->bitmask;
				}
			}
		}

		/**
		 * Check command->alias for possible match, as well.
		 */
		if (!is_null(command->alias) && !compare(command->alias, value)) {
			options = command->options;

			for (xindex = 0; xindex < length; xindex += 1) {
				option = &options[xindex];

				if (!compare(option->value, value)) {
					return option->bitmask;
				}
			}
		}
	}

	return -1;
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

		fprintf(stdout, "%4s%-s%s%s: %-24s\n", "", fvalue, space, command->alias, command->desc);
	}

	fprintf(stdout, "\nFor a list of command-specific options: sbctl <COMMAND> [-h|--help]\n");
}
