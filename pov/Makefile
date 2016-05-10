# Parametry prekladace
CC=g++
CFLAGS=-std=c++11 -O3 `pkg-config --cflags opencv`
LIBS=`pkg-config --libs opencv`

# Kompilace
all: qr

qr: main.cpp
	$(CC) $^ -o $@ $(CFLAGS) $(LIBS) 

clean:
	rm -f *.o *~ qr

run: qr 
	./qr 0.jpg 0-out.jpg
	./qr 1.jpg 1-out.jpg
	./qr 2.jpg 2-out.jpg
	./qr 3.jpg 3-out.jpg
