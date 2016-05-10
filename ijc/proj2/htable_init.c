/*
 * Soubor:  htable_init.c
 * Datum:   17.4.2011
 * Autor:   Pavel Fryz, xfryzp00@stud.fit.vutbr.cz, FIT
 * Projekt: IJC-DU2
 * Popis:   Funkce pro inicializaci hashovaci tabulky.
 */

#include <stdlib.h>
#include "htable.h"

/**
 * Funkce inicializuje hashovaci tabulku
 * dane velikosti a vrati na ni ukazatel. 
 * @param size Velikost tabulky.
 * @return Ukazatel na strukturu hashovaci tabulky.
 */
htable_t *htable_init(unsigned size)
{
  htable_t *t;
  if ((t = malloc(sizeof(htable_t) + sizeof(htable_listitem) * size)) == NULL)
    return NULL;
  t->htable_size = size;
  for (unsigned i = 0; i < size; i++)
    t->UK[i] = NULL;
  return t;
}
