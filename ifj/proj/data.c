/*
 * Soubor:   data.c
 * Resitele: xbartl03, xfryzp00, xhorak34, xstane31
 * Projekt:  Pøekladaè jazyka IFJ11
 * Popis:    Funkce pro práci s daty ulo¾enými v zásobníku.
 */

#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include "data.h"
#include "stringoperation.h"
#include "error.h"

// deklarace pomocných funkcí
void *copyNumber(void *number);
void *copyString(void *string);
void *copyNilOrMark(void *nil);
void *copyBoolean(void *b);
void freeString(void *string);
void freeNilOrMark(void *nil);
bool isEqNumber(void *a, void *b);
bool isEqString(void *a, void *b);
bool isEqNil(void *a, void *b);
bool isEqBoolean(void *a, void *b);
// konec deklarací pomocných funkcí

/**
 * Vytvoøí novì alokovaný objekt typu Data.
 * V pøípadì chyby alokace vrací NULL.
 * @param t Typ dat
 * @param d Data
 * @return Novì vytvoøený objekt
 */
Data *newData(DataType t, void *d)
{
  Data *data = malloc(sizeof(Data));
  if (data == NULL)
    return NULL;
  else {
    static void *(*const copy[])(void *) = {
    copyNumber, copyString, copyBoolean, copyNilOrMark, copyNilOrMark};
    data->data = copy[t] (d);
    data->type = t;
    if (data->data == NULL && t < DNIL) {
      free(data);
      return NULL;
    }
  }
  return data;
}

/**
 * Vytvoøí kopii dat.
 * V pøípadì chyby alokace vrací NULL.
 * @param d Data
 * @return Ukazatel na zkopírovaná data
 */
Data *copyData(Data * d)
{
  return newData(d->type, d->data);
}

/**
 * Sma¾e data.
 * @param d Data
 */
void freeData(Data * d)
{
  static void (*const delete[]) (void *) = {
  free, freeString, free, freeNilOrMark, freeNilOrMark};
  delete[d->type] (d->data);
  free(d);
}

/**
 * Porovná dvoje data na rovnost.
 * operace neni definovana pro typ DMARK.
 * @param a První data
 * @param b Druhá data
 * @return Výsledek porovnání
 */
bool isEqData(Data * a, Data * b)
{
  if (a->type != b->type)
    return false;
  static bool(*const iseq[]) (void *, void *) = {
  isEqNumber, isEqString, isEqBoolean, isEqNil};
  return iseq[a->type] (a->data, b->data);
}

/**
 * Vytvoøí kopii dat typu number(double).
 * V pøípadì chyby alokace vrací NULL.
 * @param d Data
 * @return Ukazatel na zkopírovaná data
 */
void *copyNumber(void *number)
{
  double *tmp = malloc(sizeof(double));
  if (tmp == NULL)
    return NULL;
  else {
    *tmp = *(double *)number;
  }
  return tmp;
}

/**
 * Vytvoøí kopii dat typu string(tString).
 * V pøípadì chyby alokace vrací NULL.
 * @param d Data
 * @return Ukazatel na zkopírovaná data
 */
void *copyString(void *string)
{
  tString *tmp = malloc(sizeof(tString));
  if (tmp == NULL)
    return NULL;
  else {
    if (stringCopy(tmp, (tString *) string) == EALLOC) {
      free(tmp);
      return NULL;
    }
  }
  return tmp;
}

/**
 * Vytvoøí kopii dat typu nil nebo mark(ukazatel).
 * @param d Data
 * @return Kopii ukazatele
 */
void *copyNilOrMark(void *n)
{
  return n;
}

/**
 * Vytvoøí kopii dat typu boolean(bool).
 * V pøípadì chyby alokace vrací NULL.
 * @param d Data
 * @return Ukazatel na zkopírovaná data
 */
void *copyBoolean(void *b)
{
  bool *tmp = malloc(sizeof(bool));
  if (tmp == NULL)
    return NULL;
  else {
    *tmp = *(bool *) b;
  }
  return tmp;
}

/**
 * Sma¾e data typu string(tString).
 * @param d Data
 */
void freeString(void *string)
{
  stringFree((tString *) string);
  free(string);
}

void freeNilOrMark(void *nm)
{
}

/**
 * Porovná dvoje data typu number(double) na rovnost.
 * @param a První data
 * @param b Druhá data
 * @return Výsledek porovnání
 */
bool isEqNumber(void *a, void *b)
{
  return *(double *)a == *(double *)b;
}

/**
 * Porovná dvoje data typu string(tString) na rovnost.
 * @param a První data
 * @param b Druhá data
 * @return Výsledek porovnání
 */
bool isEqString(void *a, void *b)
{
  return !strcmp(((tString *) a)->str, ((tString *) b)->str);
}

/**
 * Porovná dvoje data typu nil(ukazatel) na rovnost.
 * @param a První data
 * @param b Druhá data
 * @return Výsledek porovnání
 */
bool isEqNil(void *a, void *b)
{
  return a == b;
}

/**
 * Porovná dvoje data typu boolean(bool) na rovnost.
 * @param a První data
 * @param b Druhá data
 * @return Výsledek porovnání
 */
bool isEqBoolean(void *a, void *b)
{
  return *(bool *) a == *(bool *) b;
}
