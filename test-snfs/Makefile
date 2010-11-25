PROGRAMS = 01myfs01 02myfs02 03myfs03 04myfs04 05myfs05 \
           06myfs06 07myfs07 08myfs08 09myfs09 10myfs10 \
           11multiserver01 12multiserver02 13multiserver03 14multiserver04 \
           15filesystem01 16filesystem02 17filesystem03 18filesystem04 19filesystem05 \
           20filesystem06 



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


# Part SNFS CLIENT

01myfs01: 01myfs01.o $(COMMON) $(LIBSNFS)
	$(CC) $(CFLAGS) -o $@ $< $(COMMON) $(LIBSOCKS) $(LIBSNFS)

02myfs02: 02myfs02.o $(COMMON) $(LIBSNFS)
	$(CC) $(CFLAGS) -o $@ $< $(COMMON) $(LIBSOCKS) $(LIBSNFS)

03myfs03: 03myfs03.o $(COMMON) $(LIBSNFS)
	$(CC) $(CFLAGS) -o $@ $< $(COMMON) $(LIBSOCKS) $(LIBSNFS)

04myfs04: 04myfs04.o $(COMMON) $(LIBSNFS)
	$(CC) $(CFLAGS) -o $@ $< $(COMMON) $(LIBSOCKS) $(LIBSNFS)

05myfs05: 05myfs05.o $(COMMON) $(LIBSNFS)
	$(CC) $(CFLAGS) -o $@ $< $(COMMON) $(LIBSOCKS) $(LIBSNFS)

06myfs06: 06myfs06.o $(COMMON) $(LIBSNFS)
	$(CC) $(CFLAGS) -o $@ $< $(COMMON) $(LIBSOCKS) $(LIBSNFS)

07myfs07: 07myfs07.o $(COMMON) $(LIBSNFS)
	$(CC) $(CFLAGS) -o $@ $< $(COMMON) $(LIBSOCKS) $(LIBSNFS)

08myfs08: 08myfs08.o $(COMMON) $(LIBSNFS)
	$(CC) $(CFLAGS) -o $@ $< $(COMMON) $(LIBSOCKS) $(LIBSNFS)

09myfs09: 09myfs09.o $(COMMON) $(LIBSNFS)
	$(CC) $(CFLAGS) -o $@ $< $(COMMON) $(LIBSOCKS) $(LIBSNFS)

10myfs10: 10myfs10.o $(COMMON) $(LIBSNFS)
	$(CC) $(CFLAGS) -o $@ $< $(COMMON) $(LIBSOCKS) $(LIBSNFS)


# Part Multi-Thread Server

11multiserver01: 11multiserver01.o $(COMMON) $(LIBSNFS)
	$(CC) $(CFLAGS) -o $@ $< $(COMMON) $(LIBSOCKS) $(LIBSNFS)

12multiserver02: 12multiserver02.o $(COMMON) $(LIBSNFS)
	$(CC) $(CFLAGS) -o $@ $< $(COMMON) $(LIBSOCKS) $(LIBSNFS)

13multiserver03: 13multiserver03.o $(COMMON) $(LIBSNFS)
	$(CC) $(CFLAGS) -o $@ $< $(COMMON) $(LIBSOCKS) $(LIBSNFS)

14multiserver04: 14multiserver04.o $(COMMON) $(LIBSNFS)
	$(CC) $(CFLAGS) -o $@ $< $(COMMON) $(LIBSOCKS) $(LIBSNFS)


# Part File-System

15filesystem01: 15filesystem01.o $(COMMON) $(LIBSNFS)
	$(CC) $(CFLAGS) -o $@ $< $(COMMON) $(LIBSOCKS) $(LIBSNFS)

16filesystem02: 16filesystem02.o $(COMMON) $(LIBSNFS)
	$(CC) $(CFLAGS) -o $@ $< $(COMMON) $(LIBSOCKS) $(LIBSNFS)

17filesystem03: 17filesystem03.o $(COMMON) $(LIBSNFS)
	$(CC) $(CFLAGS) -o $@ $< $(COMMON) $(LIBSOCKS) $(LIBSNFS)

18filesystem04: 18filesystem04.o $(COMMON) $(LIBSNFS)
	$(CC) $(CFLAGS) -o $@ $< $(COMMON) $(LIBSOCKS) $(LIBSNFS)

19filesystem05: 19filesystem05.o $(COMMON) $(LIBSNFS)
	$(CC) $(CFLAGS) -o $@ $< $(COMMON) $(LIBSOCKS) $(LIBSNFS)

20filesystem06: 20filesystem06.o $(COMMON) $(LIBSNFS)
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
