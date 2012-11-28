#-------------------------------------------#
# user configuration
INSTALL_PREFIX=/usr
#-------------------------------------------#
CURRENT = 1
REVISION = 0
AGE = 0
VERSION = $(CURRENT):$(REVISION):$(AGE)

PROJDIR = `dirname $(CURDIR)`
LIBDIR = "$(PROJDIR)/lib/.libs"

CFILES := $(wildcard *.c)
OBJS := $(patsubst %.c, %.o, $(CFILES))
LIBTOOL_OBJS := $(patsubst %.c, %.lo, $(CFILES))

LIBNAME = libtinymidi
LIBTOOL_COMPILE = libtool --mode=compile
LIBTOOL_LINK = libtool --mode=link
CC = gcc
LDFLAGS =
COMPILE_FLAGS = -g -pg -Wall -pedantic -Wshadow -Wpointer-arith -Wcast-qual -Wcast-align -Wstrict-prototypes -Wmissing-prototypes -Wconversion
LINKING_FLAGS = -g -pg -O -o
MORE_LINKING_FLAGS = -R $(LIBDIR) -rpath $(INSTALL_PREFIX)/lib -version-info $(VERSION)
INCLUDE = "./include"

all: $(CFILES)
	@$(LIBTOOL_COMPILE) $(CC) $(COMPILE_FLAGS) -c $(CFILES)
	@$(LIBTOOL_LINK) $(CC) $(LINKING_FLAGS) $(LIBNAME).la $(LIBTOOL_OBJS) $(MORE_LINKING_FLAGS)

clean:
	-rm -v $(OBJS)
	-rm -v $(LIBTOOL_OBJS)
	-rm -v $(LIBNAME).la
	-rm -rfv $(CURDIR)/.libs

install:
	-install -m 644 $(CURDIR)/include/rawmidi.h $(INSTALL_PREFIX)/include
	-install -m 644 $(CURDIR)/.libs/libtinymidi.a $(INSTALL_PREFIX)/lib
	-install -m 644 $(CURDIR)/.libs/libtinymidi.so.$(CURRENT).$(REVISION).$(AGE) $(INSTALL_PREFIX)/lib
	-ln -s $(INSTALL_PREFIX)/lib/libtinymidi.so.$(CURRENT).$(REVISION).$(AGE) $(INSTALL_PREFIX)/lib/libtinymidi.so.$(CURRENT)
	-ln -s $(INSTALL_PREFIX)/lib/libtinymidi.so.$(CURRENT).$(REVISION).$(AGE) $(INSTALL_PREFIX)/lib/libtinymidi.so

uninstall:
	-rm $(INSTALL_PREFIX)/include/rawmidi.h
	-rm $(INSTALL_PREFIX)/lib/libtinymidi.a
	-rm $(INSTALL_PREFIX)/lib/libtinymidi.so.$(CURRENT).$(REVISION).$(AGE)
	-rm $(INSTALL_PREFIX)/lib/libtinymidi.so.$(CURRENT)
	-rm $(INSTALL_PREFIX)/lib/libtinymidi.so

