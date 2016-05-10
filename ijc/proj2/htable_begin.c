/*
 * Soubor:  htable_begin.c
 * Datum:   17.4.2011
 * Autor:   Pavel Fryz, xfryzp00@stud.fit.vutbr.cz, FIT
 * Projekt: IJC-DU2
 * Popis:   Funkce pro ziskani iteratoru na prvni polozku.
 */

#include <stdlib.h>
#include "htable.h"

/**
 * Funkce vraci iterator na prvni
 * polozku tabulky.
 * @param t Tabulka.
 * @return Iterator na prvni polozku.
 */
htable_iterator htable_begin(htable_t * t)
{
  htable_iterator iter = {
    .htptr = t,
    .index = 0,
    .ptr = NULL,
  };
  for (unsigned i = 0; i < t->htable_size; i++) {
    if (t->UK[i] != NULL) {
      iter.index = i;
      iter.ptr = t->UK[i];
      break;
    }
  }
  return iter;
}
