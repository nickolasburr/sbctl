/**
 * format.h
 *
 * Copyright (C) 2017 Nickolas Burr <nickolasburr@gmail.com>
 */

#ifndef SBCTL_FORMAT_H
#define SBCTL_FORMAT_H

#define COLUMN_LINE "---"

#define BASIC_LIST_HEADER "  ###   Spec   Mode   Type   Bus   Address   Port   Power [mA]   Speed [Mbps]   Serial Number   Device ID   Vendor   Product Description  \n"
#define TABLE_LIST_HEADER "------------------------------------------------------------------------------------------------------------------------------------------\n" \
                          "| ### | Spec | Mode | Type | Bus | Address | Port | Power [mA] | Speed [Mbps] | Serial Number | Device ID | Vendor | Product Description |\n" \
                          "------------------------------------------------------------------------------------------------------------------------------------------\n"
#define TABLE_LIST_FOOTER "------------------------------------------------------------------------------------------------------------------------------------------\n"
#define NOT_SPECIFIED "Not Specified"

#endif /* SBCTL_FORMAT_H */
