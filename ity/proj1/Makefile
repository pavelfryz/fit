proj1.pdf: proj1.ps
	ps2pdf proj1.ps

proj1.ps: proj1.dvi
	dvips -t a4 proj1.dvi

proj1.dvi: proj1.tex
	latex proj1.tex
	latex proj1.tex

clean:
	rm proj1.dvi proj1.aux proj1.toc proj1.log proj1.out proj1.ps
