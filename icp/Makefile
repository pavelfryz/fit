#
# Projekt: Simulator Petriho siti
# Autori: Pavel Fryz
#         Vojtech Bartl
# Datum: 23.6.2004
#
# Pouziti: 
#   - preklad:                     make
#   - vygenerovat dokumentaci:     make doxygen
#   - zabalit:                     make pack
#
#   - vycistit:                    make clean
#
#   - spustit clienta:             make run
#   - spustit server:              make runserver
#

all: doxygen client server

client: makeclient
	make -C src/client install
server: makeserver
	make -C src/server install

run: client
	./pn2012/pn2012

runserver: server
	./server2012/server2012	

doxygen:
	doxygen Doxyfile

pack:
	tar cvzf ../xfryzp00.tar.gz ../xfryzp00

makeclient:
	qmake -o src/client/Makefile src/client/Client.pro

makeserver:
	qmake -o src/server/Makefile src/server/Server.pro

clean: makeclient makeserver
	make -C src/client clean
	make -C src/server clean
	rm -f src/client/Makefile src/server/Makefile
	rm -r -f pn2012 server2012
	rm -f -r doc/*


