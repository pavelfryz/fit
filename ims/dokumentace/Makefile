ims.pdf: ims.tex title.tex
	pdflatex ims.tex
	bibtex ims
	pdflatex ims.tex
	pdflatex ims.tex

clean:
	rm -f ims.aux ims.toc ims.log ims.bbl ims.blg ims.out
