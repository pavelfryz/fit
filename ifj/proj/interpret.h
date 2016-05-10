/*
 * Soubor:   interpret.h
 * Resitele: xbartl03, xfryzp00, xhorak34, xstane31
 * Projekt:  Pøekladaè jazyka IFJ11
 * Popis:    Hlavièkový soubor interpretu
 */

struct InstrList;

/**
 * Interpretuje seznam instrukcí.
 * @param l Seznam instrukcí
 * @return Chybový kód
 */
int inter(struct InstrList *l);
