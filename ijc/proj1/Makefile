CFLAGS=-std=c99 -Wall -pedantic

all: enumtest prvocisla prvocisla-inline
	
enumtest: error.o enum.o enumtest.o
	gcc $(CFLAGS) error.o enum.o enumtest.o -o enumtest
		
prvocisla: error.o prvocisla.o
	gcc $(CFLAGS) error.o prvocisla.o -o prvocisla
	
prvocisla-inline: error.o prvocisla-inline.o
	gcc $(CFLAGS) error.o prvocisla-inline.o -o prvocisla-inline

enum.o: enum.c error.h enum.h
	gcc $(CFLAGS) -c enum.c
      
enumtest.o: enumtest.c error.h enum.h
	gcc $(CFLAGS) -c enumtest.c
      
error.o: error.c error.h
	gcc $(CFLAGS) -c error.c
    
prvocisla.o: prvocisla.c error.h
	gcc $(CFLAGS) -c prvocisla.c -O2
      
prvocisla-inline.o: prvocisla.c error.h
	gcc $(CFLAGS) -c prvocisla.c -O2 -DUSE_INLINE -o prvocisla-inline.o
	
clean: 
	rm -f *.o
