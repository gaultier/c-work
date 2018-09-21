.POSIX:
.SUFFIXES:
CFLAGS = -Weverything -std=c99
CFLAGS_DEBUG = $(CFLAGS) -g -O0
CFLAGS_RELEASE = $(CFLAGS) -O2

all: release debug

release: sales_commission_release sales_commission2_release house_release sokoban_release lox_release

sales_commission_release: sales_commission_release.o
	$(CC) $(LDFLAGS) -o $@ $<

sales_commission_release.o: sales_commission.c
	$(CC) $(CFLAGS_RELEASE) -Wno-vla -c $< -o $@

sales_commission2_release: sales_commission2_release.o
	$(CC) $(LDFLAGS) -o $@ $<

sales_commission2_release.o: sales_commission2.c
	$(CC) $(CFLAGS_RELEASE) -c $< -o $@
	
house_release: house_release.o
	$(CC) $(LDFLAGS) -o $@ $<

house_release.o: house.c
	$(CC) $(CFLAGS_RELEASE) -c $< -o $@

sokoban_release: sokoban_release.o
	$(CC) $(LDFLAGS) -lSDL2 -lSDL2_image -o $@ $<

sokoban_release.o: sokoban.c
	$(CC) $(CFLAGS_RELEASE) -c $< -o $@

lox_release: lox_main_release.o lex_release.o parse_release.o
	$(CC) $(LDFLAGS) -o $@ $^

lox_main_release.o: lox.c lex.h parse.h
	$(CC) $(CFLAGS_RELEASE) -c $< -o $@

lex_release.o: lex.c lex.h
	$(CC) $(CFLAGS_RELEASE) -c $< -o $@

parse_release.o: parse.c parse.h
	$(CC) $(CFLAGS_RELEASE) -c $< -o $@

debug: sales_commission_debug sales_commission2_debug house_debug sokoban_debug lox_debug

sales_commission_debug: sales_commission_debug.o
	$(CC) $(LDFLAGS) -o $@ $<

sales_commission_debug.o: sales_commission.c
	$(CC) $(CFLAGS_DEBUG) -Wno-vla -c $< -o $@

sales_commission2_debug: sales_commission2_debug.o
	$(CC) $(LDFLAGS) -o $@ $<

sales_commission2_debug.o: sales_commission2.c
	$(CC) $(CFLAGS_DEBUG) -c $< -o $@
	
house_debug: house_debug.o
	$(CC) $(LDFLAGS) -o $@ $<

house_debug.o: house.c
	$(CC) $(CFLAGS_DEBUG) -c $< -o $@

sokoban_debug: sokoban_debug.o
	$(CC) $(LDFLAGS) -lSDL2 -lSDL2_image -o $@ $<

sokoban_debug.o: sokoban.c
	$(CC) $(CFLAGS_DEBUG) -c $< -o $@

lox_debug: lox_main_debug.o lex_debug.o parse_debug.o
	$(CC) $(LDFLAGS) -o $@ $^

lox_main_debug.o: lox.c lex.h parse.h
	$(CC) $(CFLAGS_DEBUG) -c $< -o $@

lex_debug.o: lex.c lex.h
	$(CC) $(CFLAGS_DEBUG) -c $< -o $@

parse_debug.o: parse.c parse.h
	$(CC) $(CFLAGS_DEBUG) -c $< -o $@

clean:
	rm -f *.o *_release *_debug

.PHONY: all clean debug release
