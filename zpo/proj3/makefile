
all: $(patsubst %.cpp, %, $(wildcard *.cpp))

%: %.cpp 
	g++ $^ -o $@ -Wall -I. -I/usr/local/include -lopencv_core -lopencv_imgproc -lopencv_highgui
    
clean:
	rm -rf *o
