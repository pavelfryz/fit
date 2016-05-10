#
# Soubor:  Makefile
# Datum:   15.11.2012
# Autor:   Pavel Fryz, xfryzp00@stud.fit.vutbr.cz, FIT
# Projekt: Programovani sitove sluzny
#

CFLAGS=-std=c++98 -O3

LIBS=-lpcap

project: sip_monitor

sip_monitor: sip_monitor.o
	g++ -o sip_monitor sip_monitor.o $(LIBS) 

sip_monitor.o: sip_monitor.cpp sip_monitor.h
	g++ $(CFLAGS) -c sip_monitor.cpp

clean:
	rm -f *.o sip_monitor

