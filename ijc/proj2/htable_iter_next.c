/*
 * Soubor:  htable_iter_next.c
 * Datum:   17.4.2011
 * Autor:   Pavel Fryz, xfryzp00@stud.fit.vutbr.cz, FIT
 * Projekt: IJC-DU2
 * Popis:   Funkce pro posun iteratoru na dalsi polozku.
 */

#include <stdlib.h>
#include "htable.h"

/**
 * Funkce vraci iterator na dalsi polozku. 
 * @param i Iterator.
 * @return Iterator na dalsi polozku.
 */
htable_iterator htable_iter_next(htable_iterator i)
{
  if (i.ptr != NULL && i.ptr->next != NULL) {
    i.ptr = i.ptr->next;
    return i;
  }
  for (unsigned count = i.index + 1; count < i.htptr->htable_size; count++) {
    if (i.htptr->UK[count] != NULL) {
      i.ptr = i.htptr->UK[count];
      i.index = count;
      return i;
    }
  }
  i.index = i.index;
  i.ptr = NULL;
  return i;
}
