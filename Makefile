.POSIX:
.SUFFIXES:
CC = cc
CFLAGS = -Weverything -g -O0 -std=c99

all: build/debug/sales_commission 

build/debug/sales_commission: build/debug/sales_commission.o
	$(CC) -o build/debug/sales_commission build/debug/sales_commission.o

build/debug/sales_commission.o: sales_commission.c
	$(CC) -c $(CFLAGS) sales_commission.c build/debug/sales_commission.o

clean:
	rm -f build/debug/* 
