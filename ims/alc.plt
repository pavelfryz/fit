#
# Soubor:  alc.plt
# Datum:   8.12.2012
# Autori:  xbartl03, xbartl03@stud.fit.vutbr.cz
#          xfryzp00, xfryzp00@stud.fit.vutbr.cz
# Projekt: Reseni prohibicni krize v CR
#

set term postscript
set grid
set xlabel "Cas [hodiny]"
set ylabel "Pocet vzorku"

set xrange [0:15]
set boxwidth 1.01 relative
set style fill solid 1 noborder
plot "-" u 1:3 w boxes lc rgb"green" notitle
