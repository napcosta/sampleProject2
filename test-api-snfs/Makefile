PROGRAMS = test-ping test-copy test-append test-remove\
           test-diskusage test-defrag test-dumpcache

INCLUDES = -I. -I$(srcdir) -I../include -I ../include
COMPILE = $(CC) $(DEFS) $(INCLUDES) $(CFLAGS)
CC = gcc
CFLAGS = -g -O0 -Wall -m32 -std=c99
DEFS = -DHAVE_CONFIG_H
LIBSOCKS = -lnsl
LIBSNFS = ../snfs_lib/libsnfs.a

all: libs $(PROGRAMS)

.SUFFIXES: .c .o

test-ping: test-ping.o $(LIBSNFS)
	$(CC) $(CFLAGS) -o $@ $< $(LIBSOCKS) $(LIBSNFS)

test-copy: test-copy.o $(LIBSNFS)
	$(CC) $(CFLAGS) -o $@ $< $(LIBSOCKS) $(LIBSNFS)

test-remove: test-remove.o $(LIBSNFS)
	$(CC) $(CFLAGS) -o $@ $< $(LIBSOCKS) $(LIBSNFS)
	
test-append: test-append.o $(LIBSNFS)
	$(CC) $(CFLAGS) -o $@ $< $(LIBSOCKS) $(LIBSNFS)	
	
test-diskusage: test-diskusage.o $(LIBSNFS)
	$(CC) $(CFLAGS) -o $@ $< $(LIBSOCKS) $(LIBSNFS)		

test-defrag: test-defrag.o $(LIBSNFS)
	$(CC) $(CFLAGS) -o $@ $< $(LIBSOCKS) $(LIBSNFS)		
	
test-dumpcache: test-dumpcache.o $(LIBSNFS)
	$(CC) $(CFLAGS) -o $@ $< $(LIBSOCKS) $(LIBSNFS)			
	
libs:
	$(MAKE) libsnfs.a -C ../snfs_lib

	
.c.o:
	$(COMPILE) -c -o $@ $<


clean: clean-PROGRAMS
	rm -f *.o

clean-PROGRAMS:
	@list='$(PROGRAMS)'; for p in $$list; do \
	  f=`echo $$p|sed 's/$$//'`; \
	  echo " rm -f $$p $$f"; \
	  rm -f $$p $$f ; \
	done


# Tell versions [3.59,3.63) of GNU make to not export all variables.
# Otherwise a system limit (for SysV at least) may be exceeded.
.NOEXPORT:
