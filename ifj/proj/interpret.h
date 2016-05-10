/*
 * Soubor:   interpret.h
 * Resitele: xbartl03, xfryzp00, xhorak34, xstane31
 * Projekt:  P�eklada� jazyka IFJ11
 * Popis:    Hlavi�kov� soubor interpretu
 */

struct InstrList;

/**
 * Interpretuje seznam instrukc�.
 * @param l Seznam instrukc�
 * @return Chybov� k�d
 */
int inter(struct InstrList *l);
