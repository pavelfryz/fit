#
# Projekt: Prakticka uloha resena siti BP - asociace
# Autor: Pavel Fryz
# Datum: 23.11.2014
#
# Pouziti: 
#   - preklad:                     make
#   - zabalit:                     make pack
#
#   - vycistit:                    make clean
#
#   - naucit sit:                  make learn
#   - testovani site:              make test
#   - asociace podobnych:          make test2
#

CXX=g++
CXXFLAGS=-std=c++11 -O3

all: neuralnet

neuralnet: bpnet.o main.o pgmimage.o
	$(CXX) $(CXXFLAGS) bpnet.o main.o pgmimage.o -o neuralnet

bpnet.o: bpnet.cpp bpnet.h
	$(CXX) $(CXXFLAGS) -c bpnet.cpp

main.o: main.cpp pgmimage.h bpnet.h
	$(CXX) $(CXXFLAGS) -c main.cpp

pgmimage.o: pgmimage.cpp pgmimage.h
	$(CXX) $(CXXFLAGS) -c pgmimage.cpp

net.bin: 
	./neuralnet

learn: neuralnet
	./neuralnet

test: neuralnet net.bin
	./neuralnet ./deck1/1h.pgm
	./neuralnet ./deck1/2h.pgm
	./neuralnet ./deck1/3h.pgm
	./neuralnet ./deck1/4h.pgm
	./neuralnet ./deck1/5h.pgm
	./neuralnet ./deck1/6h.pgm
	./neuralnet ./deck1/7h.pgm
	./neuralnet ./deck1/8h.pgm
	./neuralnet ./deck1/9h.pgm
	./neuralnet ./deck1/10h.pgm
	./neuralnet ./deck1/11h.pgm
	./neuralnet ./deck1/12h.pgm
	./neuralnet ./deck1/13h.pgm

test2: neuralnet net.bin 
	./neuralnet ./deck1/1d.pgm
	./neuralnet ./deck1/2d.pgm
	./neuralnet ./deck1/3d.pgm
	./neuralnet ./deck1/4d.pgm
	./neuralnet ./deck1/5d.pgm
	./neuralnet ./deck1/6d.pgm
	./neuralnet ./deck1/7d.pgm
	./neuralnet ./deck1/8d.pgm
	./neuralnet ./deck1/9s.pgm
	./neuralnet ./deck1/10s.pgm
	./neuralnet ./deck1/11c.pgm
	./neuralnet ./deck1/12c.pgm
	./neuralnet ./deck2/13h.pgm

pack:
	tar cvzf ../xfryzp00.tar.gz ../xfryzp00

clean: 
	rm -f *.o net.bin neuralnet
