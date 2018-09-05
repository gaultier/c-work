.POSIX:
.SUFFIXES:
CC = cc
CFLAGS = -Weverything -std=c99
CFLAGS_DEBUG = $(CFLAGS) -g -O0
CFLAGS_RELEASE = $(CFLAGS) -O2

all: release

release: sales_commision_release

sales_commision_release: sales_commission_release.o
	$(CC) $(LDFLAGS) -o $@ $<

sales_commission_release.o: sales_commission.c
	$(CC) $(CFLAGS_RELEASE) -c $< -o $@
	
clean:
	rm -f *.o

.PHONY: all clean debug
