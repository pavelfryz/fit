/*
 * Soubor:   parser.h
 * Resitele: xbartl03, xfryzp00, xhorak34, xstane31
 * Projekt:  Pøekladaè jazyka IFJ11
 * Popis:    Hlavièkový soubor parseru
 */

#include <stdio.h>

struct InstrList;
struct Symbol;

/**
 * Analyzuje zdrojový kód,
 * generuje instrukce a
 * pøidává symboly do tabulky symbolù.
 * @param argList Seznam instrukcí
 * @param argTable Tabulka symbolù
 * @param argF Zdrojový soubor
 * @return Chybový kód
 */
int parse(struct InstrList *argList, struct Symbol **argTable, FILE * argF);
