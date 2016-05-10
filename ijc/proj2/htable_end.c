/*
 * Soubor:  htable_end.c
 * Datum:   17.4.2011
 * Autor:   Pavel Fryz, xfryzp00@stud.fit.vutbr.cz, FIT
 * Projekt: IJC-DU2
 * Popis:   Funkce pro ziskani iteretoru za posledni polozku.
 */

#include <stdlib.h>
#include "htable.h"

/**
 * Funkce vraci iterator za posledni 
 * polozku tabulky.
 * @param t Tabulka.
 * @return Iterator za posledni polozku.
 */
htable_iterator htable_end(htable_t * t)
{
  htable_iterator iter = {
    .htptr = t,
    .index = 0,
    .ptr = NULL,
  };
  for (unsigned i = 0; i < t->htable_size; i++) {
    if (t->UK[t->htable_size - i - 1] != NULL) {
      iter.index = t->htable_size - i - 1;
      break;
    }
  }
  return iter;
}
