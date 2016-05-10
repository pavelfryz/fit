#
# Soubor:  Makefile
# Datum:   8.12.2012
# Autori:  xbartl03, xbartl03@stud.fit.vutbr.cz
#          xfryzp00, xfryzp00@stud.fit.vutbr.cz
# Projekt: Reseni prohibicni krize v CR
#

CFLAGS=-std=c++98 -O3

LIBS=-lsimlib

SIMTIME=180

project: alc

alc: alc.o
	g++ -o alc alc.o $(LIBS) 

alc.o: alc.cpp
	g++ $(CFLAGS) -c alc.cpp

run: exp1 exp2 exp3 exp4 exp5

plot: graph1 graph2 graph3 graph4 graph5

exp1: alc
	./alc -i 200 -r 0 -t $(SIMTIME) -o exp1_200.out
	./alc -i 500 -r 0 -t $(SIMTIME) -o exp1_500.out 

exp2: alc
	./alc -i 200 -r 50 -t $(SIMTIME) -o exp2_200.out
	./alc -i 500 -r 125 -t $(SIMTIME) -o exp2_500.out

exp3: alc
	./alc -i 200 -r 100 -t $(SIMTIME) -o exp3_200.out
	./alc -i 500 -r 250 -t $(SIMTIME) -o exp3_500.out

exp4: alc
	./alc -i 200 -r 200 -t $(SIMTIME) -o exp4_200.out
	./alc -i 500 -r 500 -t $(SIMTIME) -o exp4_500.out

exp5: alc
	./alc -i 200 -r 1 -s -t $(SIMTIME) -o exp5_200.out
	./alc -i 500 -r 1 -s -t $(SIMTIME) -o exp5_500.out

graph1: exp1
	tail -n +25 exp1_200.out|head -n -2|sed 's/|//g'|cat alc.plt -|gnuplot >exp1_200.ps
	tail -n +25 exp1_500.out|head -n -2|sed 's/|//g'|cat alc.plt -|gnuplot >exp1_500.ps

graph2: exp2
	tail -n +25 exp2_200.out|head -n -2|sed 's/|//g'|cat alc.plt -|gnuplot >exp2_200.ps
	tail -n +25 exp2_500.out|head -n -2|sed 's/|//g'|cat alc.plt -|gnuplot >exp2_500.ps

graph3: exp3
	tail -n +25 exp3_200.out|head -n -2|sed 's/|//g'|cat alc.plt -|gnuplot >exp3_200.ps
	tail -n +25 exp3_500.out|head -n -2|sed 's/|//g'|cat alc.plt -|gnuplot >exp3_500.ps

graph4: exp4
	tail -n +25 exp4_200.out|head -n -2|sed 's/|//g'|cat alc.plt -|gnuplot >exp4_200.ps
	tail -n +25 exp4_500.out|head -n -2|sed 's/|//g'|cat alc.plt -|gnuplot >exp4_500.ps

graph5: exp5
	tail -n +25 exp5_200.out|head -n -2|sed 's/|//g'|cat alc.plt -|gnuplot >exp5_200.ps
	tail -n +25 exp5_500.out|head -n -2|sed 's/|//g'|cat alc.plt -|gnuplot >exp5_500.ps

clean:
	rm -f *.o alc *.out *.ps

