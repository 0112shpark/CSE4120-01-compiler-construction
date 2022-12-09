#
# makefile for TINY
# Borland C Version
# K. Louden 2/3/98
#

C = gcc

CFLAGS = -std=gnu99 

OBJS = main.o util.o parse.o symtab.o analyze.o code.o cgen.o lex.yy.o
TARGET = hw1_binary

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJS) -lfl

main.o: main.c globals.h util.h scan.h parse.h analyze.h cgen.h
	$(CC) $(CFLAGS) -c main.c

util.o: util.c util.h globals.h
	$(CC) $(CFLAGS) -c util.c

parse.o: parse.c parse.h scan.h globals.h util.h
	$(CC) $(CFLAGS) -c parse.c

symtab.o: symtab.c symtab.h
	$(CC) $(CFLAGS) -c symtab.c

analyze.o: analyze.c globals.h symtab.h analyze.h
	$(CC) $(CFLAGS) -c analyze.c

code.o: code.c code.h globals.h
	$(CC) $(CFLAGS) -c code.c

cgen.o: cgen.c globals.h symtab.h code.h cgen.h
	$(CC) $(CFLAGS) -c cgen.c

lex.yy.o: lex.yy.c util.h globals.h scan.h
	$(CC) $(CFLAGS) -c lex.yy.c

lex.yy.c: lex/tiny.l
	flex lex/tiny.l
	
clean:
	rm -rf $(OBJS)

all: $(TARGET)

