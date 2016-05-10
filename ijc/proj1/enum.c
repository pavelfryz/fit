/*
 * Soubor:  enum.c
 * Datum:   21.3.2011
 * Autor:   Pavel Fr�z, xfryzp00@stud.fit.vutbr.cz, FIT
 * Projekt: IJC-DU1, p��klad a)
 * Popis:   Funkce pro pr�ci s m�s�ci
 */

#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <locale.h>
#include "error.h"
#include "enum.h"

/** Chybov� hl�en� - m�s�c mimo rozsah. */
const char *OUTOFRANGE = "PrintMonth*: Hodnota %d je mimo rozsah\n";
/** N�zvy m�s�c� s diakritikou. */
const char *MONTHMSG[] =
{  
  "Leden", "�nor", "B�ezen", "Duben", "Kv�ten", "�erven",
  "�ervenec", "Srpen", "Z���", "��jen", "Listopad", "Prosinec",
};
/** Zkr�cen� n�zvy m�s�c� s diakritikou. */
const char *MONTHSHORTMSG[] =
{  
  "Led", "�no", "B�e", "Dub", "Kv�", "�en", 
  "�ec", "Srp", "Z��", "��j", "Lis", "Pro",
};
/** N�zvy m�s�c� bez diakritiky. */
const char *MONTHASCIIMSG[] =
{  
  "Leden", "Unor", "Brezen", "Duben", "Kveten", "Cerven",
  "Cervenec", "Srpen", "Zari", "Rijen", "Listopad", "Prosinec",
};
/** Zkr�cen� n�zvy m�s�c� bez diakritiky. */
const char *MONTHSHORTASCIIMSG[] =
{  
  "Led", "Uno", "Bre", "Dub", "Kve", "Cen", 
  "Cec", "Srp", "Zar", "Rij", "Lis", "Pro",
};
/** Pole s n�zvy m�s�c� 
 *  [Cel� s diakritikou, Zkr�cen� s diakritikou
 *  Cel� bez diakritiky, Zkr�cen� bez diakritiky]
 */
const char **ALLMONTHMSG[] = 
{
	MONTHMSG, MONTHSHORTMSG, MONTHASCIIMSG, MONTHSHORTASCIIMSG,
};

/**
 * Funkce vytiskne n�zev m�s�ce
 * ve zkr�cen�m form�tu.
 * ("Led","�no","B�e","Dub",...)
 * @param d ��slo m�s�ce.
 */
void PrintMonthShort(enum months d)
{
	if(d<Leden||d>Prosinec)
		Error(OUTOFRANGE,d);      
	else printf("%s",MONTHSHORTMSG[d-1]);
	return;  
}

/**
 * Funkce vytiskne n�zev m�s�ce.
 * ("Leden","�nor","B�ezen",...)
 * @param d ��slo m�s�ce.
 */
void PrintMonth(enum months d)
{
	if(d<Leden||d>Prosinec)
		Error(OUTOFRANGE,d);      
	else printf("%s",MONTHMSG[d-1]);
	return;  
} 

/**
 * Na�te slovo ze standartn�ho vstupu
 * a ur�� o kter� m�s�c se jedn�.
 * @return ��slo m�s�ce, 0-p�i chyb�
 */
enum months ReadMonth(void)
{
	int c;
	char input[10];
	int i=0;
	char *l = setlocale(LC_ALL,"cs_CZ.iso-8859-2"); 
	if(l==NULL) 
		Error("setlocale: Nelze nastavit �eskou lokalizaci\n"); 
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
