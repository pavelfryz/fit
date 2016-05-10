#
# IZU projekt 2012 
# 
# File:     Makefile
#
# Author:   Horacek (ihoracek@fit.vutbr.cz) 
#           Malacka (imalacka@fit.vutbr.cz)
#           Samek (samejan@fit.vutbr.cz)
#
# Revision: $Id: Makefile 6 2012-03-18 19:58:49Z samek $
#

EXEC = find_route
LDFLAGS = 
LIBS = -lm
sources = main.c queue.c
CLEANFILES = 

#
# promenne prekladu
#
CC     = gcc
CXX    = gcc
COPT   = -D_ALL_SOURCE
CFLAGS = -std=c99 -pedantic -Wall -W -s -O3 $(COPT)

RM_CMD=rm -f

OBJFILES = $(sources:.c=.o)

.PHONY : all clean distclean

all : $(EXEC)

clean:
	$(RM_CMD) *.o *~ core $(EXEC);

%.o : %.c %.h
	$(CXX) $(CFLAGS) -c $< -o $@

distclean: clean
	$(RM_CMD) $(CLEANFILES)

$(EXEC) : $(OBJFILES)
	$(CXX) $(CFLAGS) -o $@ $(OBJFILES) $(LDFLAGS) $(LIBS)
