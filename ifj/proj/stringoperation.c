/*
 * Soubor:   stringoperation.c
 * Resitele: xbartl03, xfryzp00, xhorak34, xstane31
 * Projekt:  Pøekladaè jazyka IFJ11
 * Popis:    Funkce pro práci s øetìzci
 */

#include <stdlib.h>
#include <string.h>
#include <errno.h>

#include "stringoperation.h"
#include "error.h"

#define ALLOC_INC 8

/**
 * Inicializuje øetìzec.
 * @param s Øetìzec
 * @return Chybový kód
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
 * Sma¾e øetìzec.
 * @param s Øetìzec
 */
void stringFree(tString * s)
{
  free(s->str);
}

/**
 * Sma¾e obsah øetìzece.
 * @param s Øetìzec
 */
void stringClear(tString * s)
{
  s->str[0] = '\0';
  s->length = 0;
}

/**
 * Pøidá znak na konec øetìzce.
 * @param s Øetìzec
 * @param c Znak
 * @return Chybový kód
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
 * Vytvoøí kopii øetìzce.
 * @param dest Cílový øetìzec
 * @param source Zdrojový øetìzec
 * @return Chybový kód
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
 * Provede konkatenaci cílového a zdrojového øetìzce.
 * @param dest Cílový øetìzec
 * @param source Zdrojový øetìzec
 * @return Chybový kód
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
 * Oøízne øetìzec podle zadaných indexù.
 * @param s Øetìzec
 * @param first Index zaèátku podøetìzce
 * @param last Index konce podøetìzce
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
 * Pøevede øetìzec na double.
 * @param s Øetìzec
 * @param pNum Pøevedené èíslo
 * @return Chybový kód
 */
int stringToDouble(tString * s, double *pNum)
{
  char *end = NULL;
  *pNum = strtod(s->str, &end);
  if(errno==ERANGE)
    return INTER_ERR;
  return EOK;
}
