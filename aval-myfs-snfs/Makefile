PROGRAMS = append1 copy1 remove1 defrag1\
           cache1


INCLUDES = -I. -I$(srcdir) -I../include -I ../include
COMPILE = $(CC) $(DEFS) $(INCLUDES) $(CFLAGS)
CC = gcc
CFLAGS = -g -O0 -Wall -m32 -std=c99
DEFS = -DHAVE_CONFIG_H
LIBSOCKS = -lnsl
LIBSNFS = ../snfs_lib/libsnfs.a
LIBSTHREAD = ../sthread_lib/libsthread.a
COMMON = tfuncs.o

all: libs $(PROGRAMS)

.SUFFIXES: .c .o


cache1: cache1.o $(COMMON) $(LIBSNFS)
	$(CC) $(CFLAGS) -o $@ $< $(COMMON) $(LIBSOCKS) $(LIBSNFS)
	
defrag1: defrag1.o $(COMMON) $(LIBSNFS)
	$(CC) $(CFLAGS) -o $@ $< $(COMMON) $(LIBSOCKS) $(LIBSNFS)	

append1: append1.o $(COMMON) $(LIBSNFS)
	$(CC) $(CFLAGS) -o $@ $< $(COMMON) $(LIBSOCKS) $(LIBSNFS)
	
copy1: copy1.o $(COMMON) $(LIBSNFS)
	$(CC) $(CFLAGS) -o $@ $< $(COMMON) $(LIBSOCKS) $(LIBSNFS)

remove1: remove1.o $(COMMON) $(LIBSNFS)
	$(CC) $(CFLAGS) -o $@ $< $(COMMON) $(LIBSOCKS) $(LIBSNFS)
	
libs:
	$(MAKE) libsnfs.a -C ../snfs_lib
	$(MAKE) libsthread.a -C ../sthread_lib


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


