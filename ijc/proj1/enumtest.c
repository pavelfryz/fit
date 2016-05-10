/*
 * Soubor:  enumtest.c
 * Datum:   21.3.2011
 * Autor:   Pavel Frýz, xfryzp00@stud.fit.vutbr.cz, FIT
 * Projekt: IJC-DU1, pøíklad a)
 * Popis:   Testovací soubor pro enum.c
 */

#include <stdio.h>
#include <locale.h>
#include "error.h"
#include "enum.h"

int main(void) { /* test - NEMÌNIT! */ 
	char *l = setlocale(LC_ALL,"cs_CZ.iso-8859-2"); 
	if(l==NULL) 
		Error("setlocale: Nelze nastavit èeskou lokalizaci\n"); 
	enum months m; m = ReadMonth(); // ète mìsíc 
	PrintMonthShort(m); // tiskne krátké jméno 
	printf("\n"); 
	PrintMonth(m); // tiskne dlouhé jméno 
	printf("\n\n"); 
	for( m = Leden; m < 15; m++ ) { // úmyslná chyba 
		PrintMonthShort(m); 
		printf("\n"); 
	} 
	return 0; 
} 
