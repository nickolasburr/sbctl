/**
 * compat.h
 *
 * Copyright (C) 2024 Nickolas Burr <nickolasburr@gmail.com>
 */

#ifndef SBCTL_COMPAT_H
#define SBCTL_COMPAT_H

#if (MAC_OS_X_VERSION_MAX_ALLOWED < 120000)
#define kIOMainPortDefault kIOMasterPortDefault
#endif

#endif /* SBCTL_COMPAT_H */
