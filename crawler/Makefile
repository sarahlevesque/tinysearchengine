# Makefile for Crawler Module
#
# Sarah Levesque Spring 2024 CS50


OBJS = crawler.o 
LIBS = ../libcs50/libcs50-given.a ../common/common.a

# uncomment the following to turn on verbose memory logging
#TESTING=-DMEMTEST

CFLAGS = -Wall -pedantic -std=c11 -ggdb $(TESTING) -I../common -I../libcs50
CC = gcc
MAKE = make
# for memory-leak tests
VALGRIND = valgrind --leak-check=full --show-leak-kinds=all

crawler: $(OBJS)
	$(CC) $(CFLAGS) $^ $(LIBS) -o $@

crawler.o: crawler.c


valgrind: crawler
	$(VALGRIND) -s ./crawler http://cs50tse.cs.dartmouth.edu/tse/ crawled 1

.PHONY: clean

clean:
	rm -rf *.dSYM  # MacOS debugger info
	rm -f crawled/*[0-9]
	rm -f letters-0/*[0-9]
	rm -f letters-3/*[0-9]
	rm -f toscrape-2/*[0-9]
	rm -f *~ *.o
	rm -f core
