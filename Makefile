###
### Makefile
###

CC      = gcc
TARGET  = sbctl
INSTALL = /usr/bin/install -c

prefix = /usr/local
bindir = $(prefix)/bin
binprefix =

ARFLAGS = rcs
RM      = rm
RMFLAGS = -rf

INC_DIR = include
SOURCES = src
TOOLS   = tools

CSFILES = $(wildcard $(SOURCES)/*.c)
OBFILES = $(patsubst %.c,%.o,$(CSFILES))

CFLAGS  = -I$(INC_DIR) -framework CoreFoundation -framework IOKit
LDFLAGS = -pthread -lz

.PHONY: all clean install uninstall

all: $(TARGET)

$(TARGET): $(CSFILES)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

clean:
	@unset CDPATH; cd $(TOOLS) && ./clean.sh

install:
	@unset CDPATH; cd $(TOOLS) && ./install.sh

uninstall:
	@unset CDPATH; @cd $(TOOLS) && ./uninstall.sh
