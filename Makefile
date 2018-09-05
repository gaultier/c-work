.POSIX:
.SUFFIXES:
CC = cc
CFLAGS = -Weverything -std=c99
CFLAGS_DEBUG = $(CFLAGS) -g -O0
CFLAGS_RELEASE = $(CFLAGS) -O2

all: release

release: sales_commission_release sales_commission2_release parens_release

sales_commission_release: sales_commission_release.o
	$(CC) $(LDFLAGS) -o $@ $<

sales_commission_release.o: sales_commission.c
	$(CC) $(CFLAGS_RELEASE) -Wno-vla -c $< -o $@

sales_commission2_release: sales_commission2_release.o
	$(CC) $(LDFLAGS) -o $@ $<

sales_commission2_release.o: sales_commission2.c
	$(CC) $(CFLAGS_RELEASE) -c $< -o $@
	
parens_release: parens_release.o
	$(CC) $(LDFLAGS) -o $@ $<

parens_release.o: parens.c
	$(CC) $(CFLAGS_RELEASE) -c $< -o $@

clean:
	rm -f *.o *_release *_debug

.PHONY: all clean debug release
