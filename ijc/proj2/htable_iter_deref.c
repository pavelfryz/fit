/*
 * Soubor:  htable_iter_deref.c
 * Datum:   17.4.2011
 * Autor:   Pavel Fryz, xfryzp00@stud.fit.vutbr.cz, FIT
 * Projekt: IJC-DU2
 * Popis:   Funkce pro dereferencovani iteratoru.
 */

#include "htable.h"

/**
 * Funkce dereferencuje iterator a 
 * vrati ukazatel na polozku.
 * @param i Iterator.
 * @return Ukazatel na polozku.
 */
htable_listitem *htable_iter_deref(htable_iterator i)
{
  return i.ptr;
}
