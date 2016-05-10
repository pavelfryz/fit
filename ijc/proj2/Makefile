#
# Soubor:  Makefile
# Datum:   17.4.2011
# Autor:   Pavel Fryz, xfryzp00@stud.fit.vutbr.cz, FIT
# Projekt: IJC-DU2
# Popis:   Makefile vytvari soubory tail, tail2, worldcount,
#          worldcount-dynamic a knihovny libhtable.a a libhtable.so
#

CFLAGS=-std=c99 -Wall -pedantic -O3
CPPFLAGS=-std=c++98 -pedantic -Wall
LIBMODULES=hash_function.o htable_begin.o htable_clear.o htable_end.o htable_free.o htable_init.o htable_iter_deref.o htable_iter_eq.o htable_iter_next.o htable_lookup.o

all: tail tail2 wordcount wordcount-dynamic

tail: tail.o
	gcc $(CFLAGS) tail.o -o tail
	
tail2: tail2.o
	g++ $(CPPFLAGS) tail2.o -o tail2
	
tail.o: tail.c
	gcc $(CFLAGS) -c tail.c
	
tail2.o: tail2.cc
	g++ $(CPPFLAGS) -c tail2.cc

wordcount: io.o wordcount.o libhtable.a 
	gcc $(CFLAGS) -static io.o wordcount.o libhtable.a -o wordcount
	
wordcount-dynamic: io.o wordcount.o libhtable.so
	gcc $(CFLAGS) io.o wordcount.o libhtable.so -o wordcount-dynamic	
	
wordcount.o: wordcount.c htable.h io.h
	gcc $(CFLAGS) -c wordcount.c

io.o: io.c io.h
	gcc $(CFLAGS) -c io.c

libhtable.a: $(LIBMODULES) 
	ar rcs libhtable.a $(LIBMODULES)

libhtable.so: $(LIBMODULES)
	gcc $(CFLAGS) -shared -fPIC $(LIBMODULES) -o libhtable.so
	
hash_function.o: hash_function.c htable.h
	gcc $(CFLAGS) -c -fPIC hash_function.c	
htable_begin.o: htable_begin.c htable.h
	gcc $(CFLAGS) -c -fPIC htable_begin.c
htable_clear.o: htable_clear.c htable.h
	gcc $(CFLAGS) -c -fPIC htable_clear.c
htable_end.o: htable_end.c htable.h
	gcc $(CFLAGS) -c -fPIC htable_end.c
htable_free.o: htable_free.c htable.h
	gcc $(CFLAGS) -c -fPIC htable_free.c
htable_init.o: htable_init.c htable.h
	gcc $(CFLAGS) -c -fPIC htable_init.c
htable_iter_deref.o: htable_iter_deref.c htable.h
	gcc $(CFLAGS) -c -fPIC htable_iter_deref.c
htable_iter_eq.o: htable_iter_eq.c htable.h
	gcc $(CFLAGS) -c -fPIC htable_iter_eq.c
htable_iter_next.o: htable_iter_next.c htable.h
	gcc $(CFLAGS) -c -fPIC htable_iter_next.c
htable_lookup.o: htable_lookup.c htable.h 
	gcc $(CFLAGS) -c -fPIC htable_lookup.c
    
zip:
	zip xfryzp00.zip *.c *.cc *.h Makefile 
    
clean: 
	rm -f *.o
