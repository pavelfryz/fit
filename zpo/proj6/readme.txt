Překlad aplikace:
make

Vytvoří se dvě spustitelné verze thresholding a thresholding-cv,
verze thresholding-cv se liší pouze v tom, že používá pro výpočet 
mediánu, průměru, minima, maxima knihovnu opencv, ve verzi thresholding
jsou tyto funkce implementovány v aplikaci. 

Použití:
./thresholding input-image output-image method [radius] [param1] [param2]

Kde input-image je vstupní soubor, output-image je název výstupního souboru
bez koncovky, method je číslo zvolené metody, radius udává velikost klouzavého 
okna, velikost je (2*radius+1)x(2*radius+1), param1 a param2 jsou parametry
použité metody.

Seznam metod:
1 : mean
2 : median
3 : midgray
4 : bernsen
5 : niblack

Příklad použití:
./thresholding data/text.png output 2 3
Příkaz provede prahování pomocí mediánu(2), velikost okna je 7x7(2*3+1),
vstupní soubor je data/text.png, výstup prahování je uložen do souboru output.png
