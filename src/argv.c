/**
 * argv.c
 *
 * Copyright (C) 2017 Nickolas Burr <nickolasburr@gmail.com>
 */

#include "argv.h"

/**
 * sbctl ls options.
 */
static Option_T list_opts[] = {
	{
		"--pci",
		"-P",
		"List PCI devices, buses, bridges, etc.",
		MASK_CMD_LIST_OPT_PCI,
	},
	{
		"--usb",
		"-U",
		"List USB devices, buses, hubs, etc.",
		MASK_CMD_LIST_OPT_USB,
	},
};

static Command_T commands[] = {
	{
		"list",
		"ls",
		"List all known serial devices.",
		MASK_CMD_LIST,
		list_opts,
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
 * @todo: Refactor & condense.
 */
int ARGV_get_option_bitmask (const char *cmd, const char *value) {
	int index, xindex, length;
	Command_T *command = NULL;
	Option_T *option = NULL;

	xindex = 0;

	for (index = 0; index < NUM_CMDS; index += 1) {
		command = &commands[index];

		if (!compare(command->value, cmd)) {
			/**
			 * Length of command options array.
			 */
			length = (sizeof(&command->options) / sizeof(option));

			while (length--) {
				option = &command->options[xindex++];

				if (!compare(option->value, value)) {
					return option->bitmask;
				}

				if (!is_null(option->alias) && !compare(option->alias, value)) {
					return option->bitmask;
				}
			}
		}

		/**
		 * Check command->alias for possible match, as well.
		 */
		if (!is_null(command->alias) && !compare(command->alias, cmd)) {
			/**
			 * Length of command options array.
			 */
			length = (sizeof(&command->options) / sizeof(option));

			fprintf(stdout, "1. ARGV_get_option_bitmask -> %s\n", command->alias);

			while (length--) {
				option = &command->options[xindex++];

				if (!compare(option->value, value)) {
					return option->bitmask;
				}

				if (!is_null(option->alias) && !compare(option->alias, value)) {
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
