/*
 * Soubor:  wordcount.c
 * Datum:   14.4.2011
 * Autor:   Pavel Fryz, xfryzp00@stud.fit.vutbr.cz, FIT
 * Projekt: IJC-DU2
 * Popis:   Program pro poèítání poètu výskytu slov.
 */

#include "io.h"
#include "htable.h"
#define MAX_WORD_LENGHT 255

// velikost tabulky
// prvocislo, aby se minimalizovalo seskupovani polozek v tabulce
// pokusem se ukazalo jako nejvhodnejsi velikost 
#define TABLE_SIZE 24593

int main(void)
{
  char s[MAX_WORD_LENGHT];
  htable_t *t = htable_init(TABLE_SIZE);  // inicializace tabulky
  while (fgetword(s, MAX_WORD_LENGHT, stdin) != 0)  // cteni slova
  {
    htable_listitem *item = htable_lookup(t, s);  // vyhledani slova v tabulce
    item->data++;               // pocitani vyskytu slova
  }
  // vypsani vsech slov a poctu jejich vyskytu z tabulky
  for (htable_iterator i = htable_begin(t); !htable_iter_eq(i, htable_end(t));
       i = htable_iter_next(i)) {
    printf("%s\t%u\n", htable_iter_deref(i)->key, htable_iter_deref(i)->data);
  }
  htable_free(t);               // smazani tabulky
  return 0;
}
