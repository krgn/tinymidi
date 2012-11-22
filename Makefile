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
MORE_LINKING_FLAGS = -R $(LIBDIR) -rpath /usr/lib
INCLUDE = "./include"

all: $(CFILES)
	@$(LIBTOOL_COMPILE) $(CC) $(COMPILE_FLAGS) -c $(CFILES)
	@$(LIBTOOL_LINK) $(CC) $(LINKING_FLAGS) $(LIBNAME).la $(LIBTOOL_OBJS) $(MORE_LINKING_FLAGS)

clean:
	@rm -v $(OBJS)
	@rm -v $(LIBTOOL_OBJS)
	@rm -v $(LIBNAME).la
	@rm -rfv $(CURDIR)/.libs

setup:
	@echo $(LIBDIR)
