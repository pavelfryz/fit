/*
 * Soubor:   parser.h
 * Resitele: xbartl03, xfryzp00, xhorak34, xstane31
 * Projekt:  P�eklada� jazyka IFJ11
 * Popis:    Hlavi�kov� soubor parseru
 */

#include <stdio.h>

struct InstrList;
struct Symbol;

/**
 * Analyzuje zdrojov� k�d,
 * generuje instrukce a
 * p�id�v� symboly do tabulky symbol�.
 * @param argList Seznam instrukc�
 * @param argTable Tabulka symbol�
 * @param argF Zdrojov� soubor
 * @return Chybov� k�d
 */
int parse(struct InstrList *argList, struct Symbol **argTable, FILE * argF);
