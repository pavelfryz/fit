# Makefile

CFLAGS=-std=c99 -Wall -pedantic -g -pg
RM=rm -f

OBJS=proj4.o proj4list.o proj4string.o
TARBALL=Makefile proj4.c proj4list.c proj4string.c proj4list.h proj4string.h

proj4: $(OBJS)

proj4.o: proj4.c proj4list.h proj4string.h

proj4list.o: proj4list.c proj4list.h

proj4string.o: proj4string.c proj4string.h 

clean:
	$(RM) $(OBJS)
	$(RM) proj4

xfryzp00-fit.tar.gz: $(TARBALL)
	tar c $(TARBALL) | gzip > $@

tgz: xfryzp00-fit.tar.gz
