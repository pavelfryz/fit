/*
 * Soubor:  error.c
 * Datum:   21.3.2011
 * Autor:   Pavel Fr�z, xfryzp00@stud.fit.vutbr.cz, FIT
 * Projekt: IJC-DU1
 * Popis:   Funkce pro v�pis chybov� zpr�vy
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include "error.h"

/**
 * Vyp�e chybovou zpr�vu na chybov� v�stup
 * a ukon�� program. Funkce m� stejn� parametry
 * jako funkce printf(); Zpr�va je uvozena
 * �et�zcem "CHYBA :"
 * @param fmt Form�tovac� �et�zec.
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
