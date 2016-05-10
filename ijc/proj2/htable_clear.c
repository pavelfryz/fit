/*
 * Soubor:  htable_clear.c
 * Datum:   17.4.2011
 * Autor:   Pavel Fryz, xfryzp00@stud.fit.vutbr.cz, FIT
 * Projekt: IJC-DU2
 * Popis:   Funkce pro smazani polozek hashovaci tabulky.
 */

#include <stdlib.h>
#include "htable.h"

/**
 * Funkce maze vsechny polozky tabulky.
 * @param t Tabulka.
 */
void htable_clear(htable_t * t)
{
  htable_listitem *item, *next;
  for (unsigned i = 0; i < t->htable_size; i++) {
    item = t->UK[i];
    while (item != NULL) {
      next = item->next;
      free(item->key);
      free(item);
      item = next;
    }
  }
}
