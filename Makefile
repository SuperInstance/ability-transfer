CC=gcc
CFLAGS=-std=c11 -Wall -Wextra -O2 -Iinclude
OBJS=src/ability_transfer.o

all: test_ability_transfer probe_repo_extract

test_ability_transfer: tests/test_ability_transfer.c $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^ -lm

probe_repo_extract: tests/probe_repo_extract.c $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^ -lm

src/%.o: src/%.c
	$(CC) $(CFLAGS) -c -o $@ $<

test: test_ability_transfer
	@echo "--- Ability Transfer Tests ---"
	@./test_ability_transfer
	@echo "--- Round 4: Repo Extraction Probe ---"
	@sh tests/test_round4_probe.sh

clean:
	rm -f test_ability_transfer probe_repo_extract src/*.o

.PHONY: all test clean
