#
# Soubor:  Makefile
# Datum:   29.4.2012
# Autor:   Pavel Fryz, xfryzp00@stud.fit.vutbr.cz, FIT
# Projekt: RDT
#

CFLAGS=-std=c99 `xml2-config --cflags`

LIBS=`xml2-config --libs`

project: rdtserver rdtclient

rdtserver: rdtserver.c
	gcc $(CFLAGS) $(LIBS) -o rdtserver rdtserver.c

rdtclient: rdtclient.c
	gcc $(CFLAGS) $(LIBS) -o rdtclient rdtclient.c

clean:
	rm -f *.o rdtclient rdtserver

