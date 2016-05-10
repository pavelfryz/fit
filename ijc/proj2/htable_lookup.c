/*
 * Soubor:  htable_lookup.c
 * Datum:   17.4.2011
 * Autor:   Pavel Fryz, xfryzp00@stud.fit.vutbr.cz, FIT
 * Projekt: IJC-DU2
 * Popis:   Funkce pro vyhledani polozky v hashovaci tabulce.
 */

#include <stdlib.h>
#include <string.h>
#include "htable.h"

/**
 * Funkce vyhleda polozku s danym klicem.
 * Pokud polozka neexistuje prida ji do tabulky.
 * A vrati ukazatel na tuto polozku. 
 * @param t Tabulka.
 * @param kez Klic.
 * @return Vyhledanou polozku.
 */
htable_listitem *htable_lookup(htable_t * t, const char *key)
{
  unsigned index = hash_function(key, t->htable_size);
  htable_listitem *item = t->UK[index];
  htable_listitem **list = &(t->UK[index]);
  while (item != NULL) {
    if ((strcmp(item->key, key)) == 0)
      return item;
    list = &(item->next);
    item = item->next;
  }

  if ((item = malloc(sizeof(htable_listitem))) == NULL)
    return NULL;
  if ((item->key = malloc((strlen(key) + 1) * sizeof(char))) == NULL) {
    free(item);
    return NULL;
  }
  strcpy(item->key, key);
  item->data = 0;
  item->next = NULL;

  *list = item;

  return item;
}
