CC = gcc
CFLAGS = -Wall -pedantic -g -pg
PROJDIR = `dirname $(CURDIR)`
LIBDIR = "$(PROJDIR)/.libs"
LFLAGS = -L$(LIBDIR) -ltinymidi
INCLUDE = "$(PROJDIR)/lib/include"
TESTS := $(wildcard *.c)


all: $(TESTS)
	echo $(LIBDIR)
	@$(foreach file, $^, \
			$(CC) $(CFLAGS) -I $(INCLUDE) $(file) $(LFLAGS) -o `basename $(file) .c` && \
			exec $(CURDIR)/`basename $(file) .c`)

clean: $(TESTS)
	@$(foreach file, $^, \
		rm -v $(CURDIR)/`basename $(file) .c`)
