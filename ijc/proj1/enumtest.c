/*
 * Soubor:  enumtest.c
 * Datum:   21.3.2011
 * Autor:   Pavel Fr�z, xfryzp00@stud.fit.vutbr.cz, FIT
 * Projekt: IJC-DU1, p��klad a)
 * Popis:   Testovac� soubor pro enum.c
 */

#include <stdio.h>
#include <locale.h>
#include "error.h"
#include "enum.h"

int main(void) { /* test - NEM�NIT! */ 
	char *l = setlocale(LC_ALL,"cs_CZ.iso-8859-2"); 
	if(l==NULL) 
		Error("setlocale: Nelze nastavit �eskou lokalizaci\n"); 
	enum months m; m = ReadMonth(); // �te m�s�c 
	PrintMonthShort(m); // tiskne kr�tk� jm�no 
	printf("\n"); 
	PrintMonth(m); // tiskne dlouh� jm�no 
	printf("\n\n"); 
	for( m = Leden; m < 15; m++ ) { // �mysln� chyba 
		PrintMonthShort(m); 
		printf("\n"); 
	} 
	return 0; 
} 
