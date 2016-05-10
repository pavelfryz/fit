proj4.pdf: proj4.ps
	ps2pdf -sPAPERSIZE=a4 proj4.ps

proj4.ps: proj4.dvi
	dvips proj4.dvi

proj4.dvi: proj4.tex literatura.bib
	latex proj4.tex
	bibtex proj4.aux	
	latex proj4.tex
	latex proj4.tex

clean:
	rm proj4.dvi proj4.aux proj4.toc proj4.log proj4.out proj4.ps proj4.bbl proj4.blg
