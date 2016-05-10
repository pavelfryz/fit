obj-m += pdsfw.o

.PHONY: all clean

all: pdscli pdsfw.ko

pdscli.tab.c pdscli.tab.h: pdscli.y
	bison -d pdscli.y

lex.yy.c: pdscli.l pdscli.tab.h
	flex pdscli.l

pdscli: lex.yy.c pdscli.tab.c pdscli.tab.h
	g++ pdscli.tab.c lex.yy.c -lfl -o pdscli

pdsfw.ko: pdsfw.c
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules

clean:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) clean 
	rm -f lex.yy.c pdscli.tab.c pdscli.tab.h pdscli
