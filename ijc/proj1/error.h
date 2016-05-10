/*
 * Soubor:  error.h
 * Datum:   21.3.2011
 * Autor:   Pavel Frýz, xfryzp00@stud.fit.vutbr.cz, FIT
 * Projekt: IJC-DU1
 * Popis:   Hlavièkovy soubor k souboru error.c
 */

/**
 * Vypí¹e chybovou zprávu na chybový výstup
 * a ukonèí program. Funkce má stejné parametry
 * jako funkce printf(); Zpráva je uvozena
 * øetìzcem "CHYBA :"
 * @param fmt Formátovací øetìzec.
 */
void Error(const char *fmt, ...);
