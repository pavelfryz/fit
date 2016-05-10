/*
 * Soubor:  htable_free.c
 * Datum:   17.4.2011
 * Autor:   Pavel Fryz, xfryzp00@stud.fit.vutbr.cz, FIT
 * Projekt: IJC-DU2
 * Popis:   Funkce pro smazani hashovaci tabulky.
 */

#include <stdlib.h>
#include "htable.h"

/**
 * Funkce maze tabulku, vcetne vsech polozek. 
 * @param t Tabulka.
 */
void htable_free(htable_t * t)
{
  htable_clear(t);
  free(t);
}
