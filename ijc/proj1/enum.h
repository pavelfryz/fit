/*
 * Soubor:  enum.h
 * Datum:   21.3.2011
 * Autor:   Pavel Frýz, xfryzp00@stud.fit.vutbr.cz, FIT
 * Projekt: IJC-DU1, pøíklad a)
 * Popis:   Hlavièkovy soubor k souboru enum.c
 */

/** Výèet mìsícù */
enum months {ChybnyMesic=0, Leden, Unor, Brezen, Duben, Kveten, Cerven,
             Cervenec, Srpen, Zari, Rijen, Listopad, Prosinec };

/**
 * Funkce vytiskne název mìsíce
 * ve zkráceném formátu.
 * ("Led","Úno","Bøe","Dub",...)
 * @param d Èíslo mìsíce.
 */ 
void PrintMonthShort(enum months d); 

/**
 * Funkce vytiskne název mìsíce.
 * ("Leden","Únor","Bøezen",...)
 * @param d Èíslo mìsíce.
 */
void PrintMonth(enum months d); 

/**
 * Naète slovo ze standartního vstupu
 * a urèí o který mìsíc se jedná.
 * @return Èíslo mìsíce, 0-pøi chybì
 */
enum months ReadMonth(void); 
