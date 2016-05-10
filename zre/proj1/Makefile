#
# Projekt: Dekoder projekt zre
# Autor: Pavel Fryz
# Datum: 21.4.2015
#
# Pouziti: 
#   - preklad:                     make
#   - vycistit:                    make clean
#

CXX=g++
CXXFLAGS=-std=c++11 -O3

all: zre_proj1_linux

zre_proj1_linux: main.o armadillo-5.000.1/libarmadillo.a
	$(CXX) $(CXXFLAGS) main.o armadillo-5.000.1/libarmadillo.a -o zre_proj1_linux

main.o: main.cpp armadillo-5.000.1
	$(CXX) $(CXXFLAGS) -I armadillo-5.000.1/include -c main.cpp

armadillo-5.000.1/libarmadillo.a: armadillo-5.000.1
	cd armadillo-5.000.1 && cmake . -DBUILD_SHARED_LIBS=OFF
	make -C armadillo-5.000.1

armadillo-5.000.1:
	wget http://sourceforge.net/projects/arma/files/armadillo-5.000.1.tar.gz
	tar -xvzf armadillo-5.000.1.tar.gz 

clean: 
	rm -f *.o zre_proj1_linux
