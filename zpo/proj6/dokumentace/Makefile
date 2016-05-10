NAME=dokumentace

$(NAME).dvi: $(NAME).tex
	pdflatex $(NAME)
	bibtex $(NAME)
	pdflatex $(NAME)
	pdflatex $(NAME)

$(NAME).ps: $(NAME).dvi
	dvips $(NAME).dvi

clean:
	rm -f $(NAME).aux $(NAME).log $(NAME).ps $(NAME).dvi
