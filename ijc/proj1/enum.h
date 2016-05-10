/*
 * Soubor:  enum.h
 * Datum:   21.3.2011
 * Autor:   Pavel Fr�z, xfryzp00@stud.fit.vutbr.cz, FIT
 * Projekt: IJC-DU1, p��klad a)
 * Popis:   Hlavi�kovy soubor k souboru enum.c
 */

/** V��et m�s�c� */
enum months {ChybnyMesic=0, Leden, Unor, Brezen, Duben, Kveten, Cerven,
             Cervenec, Srpen, Zari, Rijen, Listopad, Prosinec };

/**
 * Funkce vytiskne n�zev m�s�ce
 * ve zkr�cen�m form�tu.
 * ("Led","�no","B�e","Dub",...)
 * @param d ��slo m�s�ce.
 */ 
void PrintMonthShort(enum months d); 

/**
 * Funkce vytiskne n�zev m�s�ce.
 * ("Leden","�nor","B�ezen",...)
 * @param d ��slo m�s�ce.
 */
void PrintMonth(enum months d); 

/**
 * Na�te slovo ze standartn�ho vstupu
 * a ur�� o kter� m�s�c se jedn�.
 * @return ��slo m�s�ce, 0-p�i chyb�
 */
enum months ReadMonth(void); 
