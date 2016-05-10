/*
 * Soubor:  io.c
 * Datum:   17.4.2011
 * Autor:   Pavel Fryz, xfryzp00@stud.fit.vutbr.cz, FIT
 * Projekt: IJC-DU2
 * Popis:   Funkce pro nacteni slova ze souboru.
 */

#include <ctype.h>
#include <stdbool.h>
#include "io.h"

/**
 * Funkce nacte jedno slovo ze souboru, 
 * pokud je slovo delsi nez max, zbytek 
 * slova zahodi a vypise varovani 
 * na chybovy vystup (pouze pro prvni presah).
 * @param s Ukazatel na pole znaku pro nactene slovo.
 * @param max Maximalni delka slova.
 * @param f Vstupni soubor.
 * @return Pocet nactenych znaku.
 */
int fgetword(char *s, int max, FILE * f)
{
  int i = 0, c;
  static bool eword = false;
  while (isspace(c = getc(f))) ;
  if (c == EOF) {
    s[0] = '\0';
    return 0;
  }
  s[i] = c;
  i++;
  while ((c = getc(f)) != EOF && !isspace(c)) {
    if (i < max - 1) {
      s[i] = c;
      i++;
    }
    else if (!eword) {          // tisk varovani o prekroceni max delky slova 
      fprintf(stderr, "Varovani: Prekrocena max delka slova.\n");
      eword = true;
    }
  }
  s[i] = '\0';
  ungetc(c, f);
  return i;
}
