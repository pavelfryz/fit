/*
 * Soubor:   data.c
 * Resitele: xbartl03, xfryzp00, xhorak34, xstane31
 * Projekt:  P�eklada� jazyka IFJ11
 * Popis:    Funkce pro pr�ci s daty ulo�en�mi v z�sobn�ku.
 */

#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include "data.h"
#include "stringoperation.h"
#include "error.h"

// deklarace pomocn�ch funkc�
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
// konec deklarac� pomocn�ch funkc�

/**
 * Vytvo�� nov� alokovan� objekt typu Data.
 * V p��pad� chyby alokace vrac� NULL.
 * @param t Typ dat
 * @param d Data
 * @return Nov� vytvo�en� objekt
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
 * Vytvo�� kopii dat.
 * V p��pad� chyby alokace vrac� NULL.
 * @param d Data
 * @return Ukazatel na zkop�rovan� data
 */
Data *copyData(Data * d)
{
  return newData(d->type, d->data);
}

/**
 * Sma�e data.
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
 * Porovn� dvoje data na rovnost.
 * operace neni definovana pro typ DMARK.
 * @param a Prvn� data
 * @param b Druh� data
 * @return V�sledek porovn�n�
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
 * Vytvo�� kopii dat typu number(double).
 * V p��pad� chyby alokace vrac� NULL.
 * @param d Data
 * @return Ukazatel na zkop�rovan� data
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
 * Vytvo�� kopii dat typu string(tString).
 * V p��pad� chyby alokace vrac� NULL.
 * @param d Data
 * @return Ukazatel na zkop�rovan� data
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
 * Vytvo�� kopii dat typu nil nebo mark(ukazatel).
 * @param d Data
 * @return Kopii ukazatele
 */
void *copyNilOrMark(void *n)
{
  return n;
}

/**
 * Vytvo�� kopii dat typu boolean(bool).
 * V p��pad� chyby alokace vrac� NULL.
 * @param d Data
 * @return Ukazatel na zkop�rovan� data
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
 * Sma�e data typu string(tString).
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
 * Porovn� dvoje data typu number(double) na rovnost.
 * @param a Prvn� data
 * @param b Druh� data
 * @return V�sledek porovn�n�
 */
bool isEqNumber(void *a, void *b)
{
  return *(double *)a == *(double *)b;
}

/**
 * Porovn� dvoje data typu string(tString) na rovnost.
 * @param a Prvn� data
 * @param b Druh� data
 * @return V�sledek porovn�n�
 */
bool isEqString(void *a, void *b)
{
  return !strcmp(((tString *) a)->str, ((tString *) b)->str);
}

/**
 * Porovn� dvoje data typu nil(ukazatel) na rovnost.
 * @param a Prvn� data
 * @param b Druh� data
 * @return V�sledek porovn�n�
 */
bool isEqNil(void *a, void *b)
{
  return a == b;
}

/**
 * Porovn� dvoje data typu boolean(bool) na rovnost.
 * @param a Prvn� data
 * @param b Druh� data
 * @return V�sledek porovn�n�
 */
bool isEqBoolean(void *a, void *b)
{
  return *(bool *) a == *(bool *) b;
}
