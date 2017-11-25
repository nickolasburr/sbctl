###
### Makefile
###

CC      = gcc
TARGET  = scictl
INSTALL = /usr/bin/install -c

prefix = /usr/local
bindir = $(prefix)/bin
binprefix =

ARFLAGS = rcs
RM      = rm
RMFLAGS = -rf

INC_DIR = include
# IOK_DIR = /System/Library/Frameworks/IOKit.framework/Headers
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
	@cd $(TOOLS) && ./clean.sh

install:
	@cd $(TOOLS) && ./install.sh

uninstall:
	@cd $(TOOLS) && ./uninstall.sh
