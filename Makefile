CC=gcc
CFLAGS=-std=c11 -Wall -Wextra -O2 -Iinclude
OBJS=src/ability_transfer.o

all: test_ability_transfer

test_ability_transfer: tests/test_ability_transfer.c $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^ -lm

src/%.o: src/%.c
	$(CC) $(CFLAGS) -c -o $@ $<

test: test_ability_transfer
	@echo "--- Ability Transfer Tests ---"
	@./test_ability_transfer

clean:
	rm -f test_ability_transfer src/*.o

.PHONY: all test clean
