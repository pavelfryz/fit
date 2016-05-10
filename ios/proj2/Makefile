CFLAGS=-std=gnu99 -Wall -Wextra -Werror -pedantic

all: barbers

barbers: barbers.o
	gcc $(CFLAGS) barbers.o -lrt -lc -o barbers
	
barbers.o: barbers.c 
	gcc $(CFLAGS) -c barbers.c
    
clean: 
	rm -f *.o
