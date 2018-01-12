/**
 * argv.c
 *
 * Copyright (C) 2017 Nickolas Burr <nickolasburr@gmail.com>
 */

#include "argv.h"

/**
 * sbctl commands.
 */
static Command_T commands[] = {
	{
		"list",
		"ls",
		"List all known serial devices.",
		MASK_CMD_LIST,
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
 * sbctl ls options.
 */
static Option_T ls_opts[] = {
	{
		"--pci",
		"-P",
		"List PCI buses, bridges, controllers, etc.",
		MASK_CMD_LIST_OPT_PCI,
	},
	{
		"--thun",
		"-T",
		"List Thunderbolt bridges, controllers, ports, etc.",
		MASK_CMD_LIST_OPT_THUN,
	},
	{
		"--usb",
		"-U",
		"List USB devices, buses, hubs, etc.",
		MASK_CMD_LIST_OPT_USB,
	},
	{
		"--help",
		"-h",
		"Show usage information.",
		MASK_CMD_LIST_OPT_HELP,
	},
};

/**
 * Initialize command->options[index] structs.
 */
void ARGV_init (void) {
	int index;
	Command_T *command = NULL;

	for (index = 0; index < NUM_CMDS; index += 1) {
		command = &commands[index];

		switch (ARGV_get_command_bitmask(command->value)) {
			/**
			 * sbctl ls
			 */
			case MASK_CMD_LIST:
				/**
				 * Make command->options point to ls_opts array.
				 */
				command->options = &ls_opts;

				break;
			default:
				break;
		}
	}
}

/**
 * Get command bitmask.
 */
int ARGV_get_command_bitmask (const char *value) {
	int index;
	Command_T *command = NULL;

	for (index = 0; index < NUM_CMDS; index += 1) {
		command = &commands[index];

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
 */
int ARGV_get_option_bitmask (const char *cmd, const char *value) {
	int index, xindex, length;
	Command_T *command = NULL;
	Option_T *option = NULL;

	for (index = 0; index < NUM_CMDS; index += 1) {
		command = &commands[index];

		if (!compare(command->value, cmd)) {
			/**
			 * Length of command->options array.
			 */
			switch (ARGV_get_command_bitmask(cmd)) {
				case MASK_CMD_LIST:
					length = (sizeof(ls_opts) / sizeof(ls_opts[0]));

					break;
				default:
					length = 0;

					break;
			}

			xindex = 0;

			while (length--) {
				option = &(*command->options)[xindex++];

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
			 * Length of command->options array, based on command.
			 */
			switch (ARGV_get_command_bitmask(cmd)) {
				case MASK_CMD_LIST:
					length = (sizeof(ls_opts) / sizeof(ls_opts[0]));

					break;
				default:
					length = 0;

					break;
			}

			xindex = 0;

			while (length--) {
				option = &(*command->options)[xindex++];

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
 * Print command-specific usage information.
 */
void ARGV_command_usage (const char *cmd) {
	int index, length;
	char fvalue[40];
	char *space = " ";
	Option_T *cmd_opts = NULL;
	Option_T *option = NULL;

	fprintf(stdout, "Usage: sbctl %s [OPTIONS]\n\n", cmd);
	fprintf(stdout, "Commands:\n\n");

	switch (ARGV_get_command_bitmask(cmd)) {
		case MASK_CMD_LIST:
			cmd_opts = (Option_T *) &ls_opts;
			length = (sizeof(ls_opts) / sizeof(ls_opts[0]));

			break;
		default:
			return;
	}

	for (index = 0; index < length; index += 1) {
		option = &cmd_opts[index];

		/**
		 * Format command->value string.
		 */
		copy(fvalue, option->value);

		/**
		 * If option->alias is non-null,
		 * format fvalue to include alias.
		 */
		if (!is_null(option->alias)) {
			concat(fvalue, ",");
		} else {
			option->alias = "";
			space = "";
		}

		fprintf(stdout, "%4s%-s%s%s: %-24s\n", "", fvalue, space, option->alias, option->desc);
	}

	return;
}

/**
 * Print general usage information.
 */
void ARGV_general_usage (void) {
	int index;
	char fvalue[40];
	char *space = " ";
	Command_T *command = NULL;

	fprintf(stdout, "Usage: sbctl <COMMAND> [OPTIONS]\n\n");
	fprintf(stdout, "Commands:\n\n");

	for (index = 0; index < NUM_CMDS; index += 1) {
		command = &commands[index];

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
