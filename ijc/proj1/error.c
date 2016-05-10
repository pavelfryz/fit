/*
 * Soubor:  error.c
 * Datum:   21.3.2011
 * Autor:   Pavel Frýz, xfryzp00@stud.fit.vutbr.cz, FIT
 * Projekt: IJC-DU1
 * Popis:   Funkce pro výpis chybové zprávy
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include "error.h"

/**
 * Vypí¹e chybovou zprávu na chybový výstup
 * a ukonèí program. Funkce má stejné parametry
 * jako funkce printf(); Zpráva je uvozena
 * øetìzcem "CHYBA :"
 * @param fmt Formátovací øetìzec.
 */
void Error(const char *fmt, ...)
{
	va_list arg;
	va_start(arg,fmt);
	fprintf(stderr,"%s","CHYBA: ");
	vfprintf(stderr, fmt, arg);
	va_end(arg);
	exit(1);   	   	
}
