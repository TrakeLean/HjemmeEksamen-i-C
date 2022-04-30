CC=gcc
BIN=index
 
MAP_SRC:=hashmap.c
INDEX_SRC:=index.c
MAIN_SRC:=main.c

SRC:=common.c ui.c linkedlist.c trie.c document.c $(MAP_SRC) $(INDEX_SRC)
SRC:=$(patsubst %.c,src/%.c, $(SRC))

INCLUDE=include

CFLAGS=-Wall -Wextra -g
LDFLAGS=-lm -lncurses -DLOG_LEVEL=0

.PHONY: all
.PHONY: index
.PHONY: test
.PHONY: bench
.PHONY: clean

all: index test bench

index: src/*.c src/main.c Makefile
	$(info $(SRC))
	$(CC) -o $(BIN) $(CFLAGS) $(SRC) src/main.c -I$(INCLUDE) $(LDFLAGS)


test: $(SRC) Makefile
	$(CC) -o test_index $(CFLAGS) $(SRC) src/test.c -I$(INCLUDE) $(LDFLAGS)

bench: $(SRC) Makefile
	$(CC) -o bench_index $(CFLAGS) $(SRC) src/benchmark.c -I$(INCLUDE) $(LDFLAGS)

run: test bench
	./test_index
	./bench_index 1> bench.txt


clean:
	rm -rf index test_index bench_index
