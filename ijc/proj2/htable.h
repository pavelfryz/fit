/*
 * Soubor:  htable.h
 * Datum:   17.4.2011
 * Autor:   Pavel Fryz, xfryzp00@stud.fit.vutbr.cz, FIT
 * Projekt: IJC-DU2
 * Popis:   Hlavickovy soubor pro funkce pracujici s hashovaci tabulkou.
 *          +potrebne struktury
 */

#include <stdbool.h>

typedef struct htable_t {
  unsigned htable_size;         // velikost tabulky
  struct htable_listitem *UK[]; // tabulka
} htable_t;

typedef struct htable_listitem {
  char *key;                    // ukazatel na dynamicky alokovaný øetìzec, 
  unsigned data;                // poèet výskytù a 
  struct htable_listitem *next; // ukazatel na dal¹í záznam -  
} htable_listitem;

typedef struct htable_iterator {
  struct htable_t *htptr;       // ukazatel na tabulku 
  unsigned index;               // pozici iterátoru v poli 
  struct htable_listitem *ptr;  // ukazatel na polo¾ku nebo NULL 
} htable_iterator;

unsigned int hash_function(const char *str, unsigned htable_size);
htable_iterator htable_begin(htable_t * t);
void htable_clear(htable_t * t);
htable_iterator htable_end(htable_t * t);
void htable_free(htable_t * t);
htable_t *htable_init(unsigned size);
htable_listitem *htable_iter_deref(htable_iterator i);
bool htable_iter_eq(htable_iterator i1, htable_iterator i2);
htable_iterator htable_iter_next(htable_iterator i);
htable_listitem *htable_lookup(htable_t * t, const char *key);
