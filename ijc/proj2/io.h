/*
 * Soubor:  io.h
 * Datum:   17.4.2011
 * Autor:   Pavel Fryz, xfryzp00@stud.fit.vutbr.cz, FIT
 * Projekt: IJC-DU2
 * Popis:   Hlavickovy soubor pro io.c.
 */

#include <stdio.h>

/**
 * Funkce nacte jedno slovo ze souboru, 
 * pokud je slovo delsi nez max, zbytek 
 * slova zahodi a vypise varovani 
 * na chybovy vystup (pouze pro prvni presah).
 * @param s Ukazatel na pole znaku pro nactene slovo.
 * @param max Maximalni delka slova.
 * @param f Vstupni soubor.
 * @return Pocet nactenych znaku.
 */
int fgetword(char *s, int max, FILE * f);
