/*
 * Soubor:  error.h
 * Datum:   21.3.2011
 * Autor:   Pavel Fr�z, xfryzp00@stud.fit.vutbr.cz, FIT
 * Projekt: IJC-DU1
 * Popis:   Hlavi�kovy soubor k souboru error.c
 */

/**
 * Vyp�e chybovou zpr�vu na chybov� v�stup
 * a ukon�� program. Funkce m� stejn� parametry
 * jako funkce printf(); Zpr�va je uvozena
 * �et�zcem "CHYBA :"
 * @param fmt Form�tovac� �et�zec.
 */
void Error(const char *fmt, ...);
