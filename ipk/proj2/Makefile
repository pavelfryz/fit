#
# Soubor:  Makefile
# Datum:   3.3.2012
# Autor:   Pavel Fryz, xfryzp00@stud.fit.vutbr.cz, FIT
# Projekt: Klient/server - preklad DNS
#

CPPFLAGS=-std=c++98 -pedantic

project: client server

client: client.cpp client.h
	g++ $(CPPFLAGS) -o client client.cpp

server: server.cpp
	g++ $(CPPFLAGS) -pthread -o server server.cpp

clean:
	rm -f *.o client server

