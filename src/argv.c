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
		"rm",
		"Remove properties from device.",
		MASK_CMD_UNSET,
	},
	{
		"help",
		"--help",
		"Show usage information.",
		MASK_CMD_HELP,
	},
	{
		"version",
		"--version",
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
		"--table",
		"-t",
		"Output list information in table format.",
		MASK_CMD_LIST_OPT_TBL,
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
	char value[40];
	Option_T *cmd_opts = NULL;
	Option_T *option = NULL;

	fprintf(stdout, "Usage: sbctl %s [OPTIONS]\n\n", cmd);
	fprintf(stdout, "Options:\n\n");

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
		copy(value, option->value);

		/**
		 * If option->alias is non-null,
		 * format fvalue to include alias.
		 */
		if (!is_null(option->alias)) {
			concat(value, ", ");
		} else {
			option->alias = "";
		}

		fprintf(stdout, "%4s%-s%s: %-24s\n", "", value, option->alias, option->desc);
	}

	return;
}

/**
 * Print general usage information.
 */
void ARGV_general_usage (void) {
	int index;
	char value[40];
	Command_T *command = NULL;

	fprintf(stdout, "Usage: sbctl <COMMAND> [OPTIONS]\n\n");
	fprintf(stdout, "Commands:\n\n");

	for (index = 0; index < NUM_CMDS; index += 1) {
		command = &commands[index];

		/**
		 * Format command->value string.
		 */
		copy(value, command->value);

		/**
		 * If command->alias is non-null,
		 * format fvalue to include alias.
		 */
		if (!is_null(command->alias)) {
			concat(value, ", ");
		} else {
			command->alias = "";
		}

		fprintf(stdout, "%4s%-s%s: %-24s\n", "", value, command->alias, command->desc);
	}

	fprintf(stdout, "\nFor a list of command-specific options: sbctl <COMMAND> [-h|--help]\n");
}
