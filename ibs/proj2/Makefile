ARCHIV=xfryzp00
CV1=xfryzp00
CVS = $(CV1)

all: $(CVS)

$(CV1): clean
	pdflatex $(CV1)
	pdflatex $(CV1)
	
clean:
	rm -f *.dvi *.log $(CV1).blg $(CV1).bbl $(CV1).toc *.aux $(CV1).out	$(CV2).blg $(CV2).bbl $(CV2).toc $(CV2).out

pack:
	tar czvf $(ARCHIV).tar.gz *.tex *.bib *.pdf *.jpg *.bst Makefile

