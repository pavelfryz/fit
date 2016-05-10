/*
 * Soubor:  enum.c
 * Datum:   21.3.2011
 * Autor:   Pavel Frýz, xfryzp00@stud.fit.vutbr.cz, FIT
 * Projekt: IJC-DU1, pøíklad a)
 * Popis:   Funkce pro práci s mìsíci
 */

#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <locale.h>
#include "error.h"
#include "enum.h"

/** Chybová hlá¹ení - mìsíc mimo rozsah. */
const char *OUTOFRANGE = "PrintMonth*: Hodnota %d je mimo rozsah\n";
/** Názvy mìsícù s diakritikou. */
const char *MONTHMSG[] =
{  
  "Leden", "Únor", "Bøezen", "Duben", "Kvìten", "Èerven",
  "Èervenec", "Srpen", "Záøí", "Øíjen", "Listopad", "Prosinec",
};
/** Zkrácené názvy mìsícù s diakritikou. */
const char *MONTHSHORTMSG[] =
{  
  "Led", "Úno", "Bøe", "Dub", "Kvì", "Èen", 
  "Èec", "Srp", "Záø", "Øíj", "Lis", "Pro",
};
/** Názvy mìsícù bez diakritiky. */
const char *MONTHASCIIMSG[] =
{  
  "Leden", "Unor", "Brezen", "Duben", "Kveten", "Cerven",
  "Cervenec", "Srpen", "Zari", "Rijen", "Listopad", "Prosinec",
};
/** Zkrácené názvy mìsícù bez diakritiky. */
const char *MONTHSHORTASCIIMSG[] =
{  
  "Led", "Uno", "Bre", "Dub", "Kve", "Cen", 
  "Cec", "Srp", "Zar", "Rij", "Lis", "Pro",
};
/** Pole s názvy mìsícù 
 *  [Celé s diakritikou, Zkrácené s diakritikou
 *  Celé bez diakritiky, Zkrácené bez diakritiky]
 */
const char **ALLMONTHMSG[] = 
{
	MONTHMSG, MONTHSHORTMSG, MONTHASCIIMSG, MONTHSHORTASCIIMSG,
};

/**
 * Funkce vytiskne název mìsíce
 * ve zkráceném formátu.
 * ("Led","Úno","Bøe","Dub",...)
 * @param d Èíslo mìsíce.
 */
void PrintMonthShort(enum months d)
{
	if(d<Leden||d>Prosinec)
		Error(OUTOFRANGE,d);      
	else printf("%s",MONTHSHORTMSG[d-1]);
	return;  
}

/**
 * Funkce vytiskne název mìsíce.
 * ("Leden","Únor","Bøezen",...)
 * @param d Èíslo mìsíce.
 */
void PrintMonth(enum months d)
{
	if(d<Leden||d>Prosinec)
		Error(OUTOFRANGE,d);      
	else printf("%s",MONTHMSG[d-1]);
	return;  
} 

/**
 * Naète slovo ze standartního vstupu
 * a urèí o který mìsíc se jedná.
 * @return Èíslo mìsíce, 0-pøi chybì
 */
enum months ReadMonth(void)
{
	int c;
	char input[10];
	int i=0;
	char *l = setlocale(LC_ALL,"cs_CZ.iso-8859-2"); 
	if(l==NULL) 
		Error("setlocale: Nelze nastavit èeskou lokalizaci\n"); 
	// Preskoci bile znaky
	while(isspace(c=getchar()));

	if(isalpha(c)) 
	{// prevede prvni znak na velke pismeno a ulozi v input
		input[i]=toupper(c);
	}
	else 
	{// prvni znak neni pismeno
		if(c!=EOF) ungetc(c,stdin);
		return 0;
	}
	i++;
	while((c=getchar())!=EOF)
	{// cteni znaku ze vstupu
		if(isalpha(c)) 
		{// pismeno
			if(i<9)
			{// prevede velka pismena na mala a ulozi v input
				input[i]=tolower(c);
				i++;
			}
	   	}
	   	else
	   	{// konec slova, posledni znak nebyl pismeno
	   		// vraceni znaku na vstup
	   		ungetc(c,stdin);
			break;
		}	
	}
	if(i==9) return 0;
	input[i]='\0';
	
	//Porovnani nactene hodnoty s nazvy mesicu
	for(i=0;i<4;i++)
	for(int j=0;j<12;j++)
	if(strcmp(input,ALLMONTHMSG[i][j])==0) return j+1;
		
	return 0;
}
