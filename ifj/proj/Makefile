CFLAGS=-std=c99 -Wall -pedantic -O3
BIN=project
CC=gcc
MODULES=data.o datastack.o expr.o exprstack.o ial.o ilist.o instruction.o interpret.o main.o parser.o scanner.o stringoperation.o

all: main

main: $(MODULES)
	$(CC) $(CFLAGS) -o main $(MODULES) -lm
data.o: data.c data.h stringoperation.h error.h
	$(CC) $(CFLAGS) -c data.c
datastack.o: datastack.c datastack.h data.h error.h
	$(CC) $(CFLAGS) -c datastack.c
expr.o: expr.c expr.h exprstack.h scanner.h instruction.h ilist.h \
 stringoperation.h data.h ial.h error.h
	$(CC) $(CFLAGS) -c expr.c
exprstack.o: exprstack.c exprstack.h expr.h error.h
	$(CC) $(CFLAGS) -c exprstack.c
ial.o: ial.c ial.h stringoperation.h error.h data.h
	$(CC) $(CFLAGS) -c ial.c
ilist.o: ilist.c ilist.h instruction.h error.h
	$(CC) $(CFLAGS) -c ilist.c
instruction.o: instruction.c instruction.h
	$(CC) $(CFLAGS) -c instruction.c
interpret.o: interpret.c ilist.h instruction.h datastack.h data.h ial.h \
 stringoperation.h error.h
	$(CC) $(CFLAGS) -c interpret.c
main.o: main.c parser.h interpret.h ilist.h ial.h error.h
	$(CC) $(CFLAGS) -c main.c
parser.o: parser.c parser.h scanner.h instruction.h ilist.h \
 stringoperation.h data.h ial.h error.h expr.h
	$(CC) $(CFLAGS) -c parser.c
scanner.o: scanner.c scanner.h stringoperation.h error.h
	$(CC) $(CFLAGS) -c scanner.c
stringoperation.o: stringoperation.c stringoperation.h error.h
	$(CC) $(CFLAGS) -c stringoperation.c

clean:
	rm -f *.o main

tgz: xfryzp00.tgz

xfryzp00.tgz: Makefile rozdeleni rozsireni *.c *.h dokumentace/dokumentace.pdf
	cp dokumentace/dokumentace.pdf dokumentace.pdf
	tar -czf xfryzp00.tgz Makefile rozdeleni rozsireni *.c *.h dokumentace.pdf
	rm dokumentace.pdf

dokumentace/dokumentace.pdf: dokumentace/dokumentace.tex dokumentace/title.tex
	make -C dokumentace
