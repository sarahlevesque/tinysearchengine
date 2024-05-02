# Makefile for Indexer Module
#
# Sarah Levesque Spring 2024 CS50

OBJS = indexer.o indexertest.o
LIBS = ../libcs50/libcs50-given.a ../common/common.a

# uncomment the following to turn on verbose memory logging
#TESTING=-DMEMTEST

CFLAGS = -Wall -pedantic -std=c11 -ggdb $(TESTING) -I../common -I../libcs50
CC = gcc
MAKE = make
# for memory-leak tests
VALGRIND = valgrind --leak-check=full --show-leak-kinds=all

indexer: indexer.o
	$(CC) $(CFLAGS) $^ $(LIBS) -o $@

indexertest: indexertest.o ../common/common.a ../libcs50/libcs50.a
	$(CC) $(CFLAGS) indexertest.o ../common/common.a ../libcs50/libcs50.a -o indexertest

test: indexertest test testcomp
	./indexertest < test testcomp

indexer.o: indexer.c
	$(CC) $(CFLAGS) -c indexer.c -o indexer.o
	
indexertest.o: indexertest.c
	$(CC) $(CFLAGS) -c indexertest.c -o indexertest.o

valgrind: indexertest
	$(VALGRIND) -s ./indexertest test testcomp

all: indexer indexertest

.PHONY: clean all

clean:
	rm -rf *.dSYM  # MacOS debugger info
	rm -f *~ *.o
	rm -f core
	rm -f indexer indexertest
