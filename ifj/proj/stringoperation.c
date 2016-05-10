/*
 * Soubor:   stringoperation.c
 * Resitele: xbartl03, xfryzp00, xhorak34, xstane31
 * Projekt:  P�eklada� jazyka IFJ11
 * Popis:    Funkce pro pr�ci s �et�zci
 */

#include <stdlib.h>
#include <string.h>
#include <errno.h>

#include "stringoperation.h"
#include "error.h"

#define ALLOC_INC 8

/**
 * Inicializuje �et�zec.
 * @param s �et�zec
 * @return Chybov� k�d
 */
int stringInit(tString * s)
{
  if ((s->str = malloc(ALLOC_INC)) == NULL)
    return EALLOC;
  s->str[0] = '\0';
  s->length = 0;
  s->allocated = ALLOC_INC;
  return EOK;
}

/**
 * Sma�e �et�zec.
 * @param s �et�zec
 */
void stringFree(tString * s)
{
  free(s->str);
}

/**
 * Sma�e obsah �et�zece.
 * @param s �et�zec
 */
void stringClear(tString * s)
{
  s->str[0] = '\0';
  s->length = 0;
}

/**
 * P�id� znak na konec �et�zce.
 * @param s �et�zec
 * @param c Znak
 * @return Chybov� k�d
 */
int addChar(tString * s, char c)
{
  if (s->length + 1 >= s->allocated) {
    //alokovana pamet jiz nestaci
    if ((s->str = realloc(s->str, s->allocated + ALLOC_INC)) == NULL)
      return EALLOC;
    s->allocated += ALLOC_INC;
  }
  s->str[s->length] = c;
  s->length++;
  s->str[s->length] = '\0';
  return EOK;
}

/**
 * Vytvo�� kopii �et�zce.
 * @param dest C�lov� �et�zec
 * @param source Zdrojov� �et�zec
 * @return Chybov� k�d
 */
int stringCopy(tString * dest, tString * source)
{
  dest->str = malloc(source->length + 1);
  if (dest->str == NULL)
    return EALLOC;
  else {
    memcpy(dest->str, source->str, source->length + 1);
    dest->length = source->length;
    dest->allocated = source->length + 1;
  }
  return EOK;
}

/**
 * Provede konkatenaci c�lov�ho a zdrojov�ho �et�zce.
 * @param dest C�lov� �et�zec
 * @param source Zdrojov� �et�zec
 * @return Chybov� k�d
 */
int stringAppend(tString * dest, tString * source)
{
  if ((dest->str =
       realloc(dest->str, dest->length + source->length + 1)) == NULL)
    return EALLOC;
  memmove(dest->str + dest->length, source->str, source->length + 1);
  dest->length = dest->length + source->length;
  dest->allocated = dest->length + source->length + 1;
  return EOK;
}

/**
 * O��zne �et�zec podle zadan�ch index�.
 * @param s �et�zec
 * @param first Index za��tku pod�et�zce
 * @param last Index konce pod�et�zce
 */
void stringSubstr(tString * s, int first, int last)
{
  if (first < 0)
    first += s->length;
  else
    first--;
  if (last < 0)
    last += s->length;
  else
    last--;

  if (first >= s->length || last < 0 || first > last || s->length == 0)
    s->length = 0;
  else {
    if (first < 0)
      first = 0;
    if (last >= s->length)
      last = s->length - 1;
    s->length = last - first + 1;
    memmove(s->str, s->str + first, s->length);
  }
  s->str[s->length] = '\0';
}

/**
 * P�evede �et�zec na double.
 * @param s �et�zec
 * @param pNum P�eveden� ��slo
 * @return Chybov� k�d
 */
int stringToDouble(tString * s, double *pNum)
{
  char *end = NULL;
  *pNum = strtod(s->str, &end);
  if(errno==ERANGE)
    return INTER_ERR;
  return EOK;
}
