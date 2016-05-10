/*
 * Soubor:  hash_function.c
 * Datum:   17.4.2011
 * Autor:   Pavel Fryz, xfryzp00@stud.fit.vutbr.cz, FIT
 * Projekt: IJC-DU2
 * Popis:   Hashovaci funkce pro retezce.
 */

/**
 * Hashovaci funkce, ktera na zaklade
 * retezce a velikosti pole vraci index pole.
 * @param str Retezec.
 * @param htable_size Velikost pole.
 * @return Index.
 */
unsigned int hash_function(const char *str, unsigned htable_size)
{
  unsigned int h = 0;
  unsigned char *p;
  for (p = (unsigned char *)str; *p != '\0'; p++)
    h = 31 * h + *p;
  return h % htable_size;
}
