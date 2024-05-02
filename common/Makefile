# Makefile for Common Library
#   Builds and compiles all these utility functions into a library.
#
# Sarah Levesque Spring 2024 CS50

OBJS = pagedir.o index.o word.o
INCLUDES = -I../libcs50
LIB = common.a

CFLAGS = -Wall -pedantic -std=c11 -ggdb $(FLAGS) $(INCLUDES)
CC = gcc
MAKE = make

# Build $(LIB) by archiving object files
$(LIB): $(OBJS)
	ar cr $(LIB) $(OBJS)

pagedir.o = pagedir.h
index.o = index.h
word.o = word.h

# clean up after our compilation
clean:
	rm -f core
	rm -f $(LIB) *~ *.o