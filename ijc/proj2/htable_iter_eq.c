/*
 * Soubor:  htable_iter_eq.c
 * Datum:   17.4.2011
 * Autor:   Pavel Fryz, xfryzp00@stud.fit.vutbr.cz, FIT
 * Projekt: IJC-DU2
 * Popis:   Funkce pro porovnani dvou iteratoru.
 */

#include "htable.h"

/**
 * Funkce porovna dva iteratory na shodu. 
 * @param i1 Prvni iterator.
 * @param i2 Druhy iterator.
 * @return Hodnotu true-shodne iteratory, jinak false.
 */
bool htable_iter_eq(htable_iterator i1, htable_iterator i2)
{
  return ((i1.index == i2.index) && (i1.htptr == i2.htptr)
          && (i1.ptr == i2.ptr));
}
