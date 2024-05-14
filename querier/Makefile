# Makefile for Querier Module
#
# Sarah Levesque Spring 2024 CS50

OBJS = querier.o
LIBS = ../libcs50/libcs50-given.a ../common/common.a

# uncomment the following to turn on verbose memory logging
#TESTING=-DMEMTEST

CFLAGS = -Wall -pedantic -std=c11 -ggdb $(TESTING) -I../common -I../libcs50
CC = gcc
MAKE = make
# for memory-leak tests
VALGRIND = valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes


querier: querier.o
	$(CC) $(CFLAGS) $^ $(LIBS) -o $@

fuzzquery: fuzzquery.o ../common/common.a ../libcs50/libcs50.a
	$(CC) $(CFLAGS) fuzzquery.o ../common/common.a ../libcs50/libcs50.a -o fuzzquery

querier.o: querier.c
	$(CC) $(CFLAGS) -c querier.c -o querier.o

fuzzquery.o: fuzzquery.c
	$(CC) $(CFLAGS) -c fuzzquery.c -o fuzzquery.o

valgrind: querier
	$(VALGRIND) -s ./querier letters-3-c letters-3

test: 
	bash -v testing.sh > testing.out 2>&1

clean:
	rm -rf *.dSYM  # MacOS debugger info
	rm -f *~ *.o
	rm -f core