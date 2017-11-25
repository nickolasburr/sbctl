#!/bin/sh

set -ex

PROJ_DIR="$(dirname $PWD)"
TARGET="usbctl"

if test -f "$PROJ_DIR/$TARGET"; then
	rm -rf "$PROJ_DIR/$TARGET"
fi

if test -d "$PROJ_DIR/$TARGET.dSYM"; then
	rm -rf "$PROJ_DIR/$TARGET.dSYM"
fi
