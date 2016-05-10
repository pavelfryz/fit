proj5.pdf: proj5.ps
	ps2pdf -dPDFSETTINGS=/prepress proj5.ps

proj5.ps: proj5.dvi
	dvips -q -t a4 proj5.dvi -o proj5.ps

proj5.dvi: proj5.tex
	latex proj5.tex
	latex proj5.tex

clean:
	rm -f proj5.dvi proj5.aux proj5.toc proj5.log proj5.out proj5.ps
