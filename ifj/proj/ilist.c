/*
 * Soubor:   ilist.c
 * Resitele: xbartl03, xfryzp00, xhorak34, xstane31
 * Projekt:  Pøekladaè jazyka IFJ11
 * Popis:    Funkce pro práci se seznamem instrukcí.
 */

#include <stdlib.h>
#include "ilist.h"
#include "instruction.h"
#include "error.h"

/**
 * Inicializuje seznam.
 * @param l Seznam
 */
void ilistInit(InstrList * l)
{
  l->first = NULL;
  l->last = NULL;
  l->active = NULL;
}

/**
 * Sma¾e seznam.
 * @param l Seznam
 */
void ilistFree(InstrList * l)
{
  while (l->first != NULL) {
    ListItem *tmp = l->first;
    l->first = l->first->next;
    freeInstr(tmp->i);
    free(tmp);
  }
}

/**
 * Vlo¾í instrukci na konec seznamu.
 * @param l Seznam
 * @param i Instrukce
 * @return Chybový kód
 */
int ilistInsertLast(InstrList * l, Instr * i)
{
  ListItem *tmp = malloc(sizeof(ListItem));
  if (tmp == NULL) {
    freeInstr(i);
    return EALLOC;
  }
  else {
    tmp->i = i;
    tmp->next = NULL;
    if (l->first == NULL)
      l->first = tmp;
    else
      l->last->next = tmp;
    l->last = tmp;
  }
  return EOK;
}

/**
 * Nastaví aktivitu seznamu na první instrukci.
 * @param l Seznam
 */
void ilistFirst(InstrList * l)
{
  l->active = l->first;
}

/**
 * Nastaví aktivitu seznamu na poslední instrukci.
 * @param l Seznam
 */
void ilistLast(InstrList * l)
{
  l->active = l->first;
}

/**
 * Nastaví aktivitu seznamu na dal¹í instrukci.
 * @param l Seznam
 */
void ilistNext(InstrList * l)
{
  l->active = l->active->next;
}

/**
 * Nastaví aktivitu seznamu na zadanou adresu.
 * @param l Seznam
 * @param gotoInstr Adresa
 */
void ilistGoto(InstrList * l, void *gotoInstr)
{
  l->active = (ListItem *) gotoInstr;
}

/**
 * Vrátí adresu aktivní polo¾ky seznamu.
 * @param l Seznam
 * @return Adresa polo¾ky seznamu
 */
void *ilistGetPtr(InstrList * l)
{
  return (void *)l->active;
}

/**
 * Vrátí adresu poslední polo¾ky seznamu.
 * @param l Seznam
 * @return Adresa polo¾ky seznamu
 */
void *ilistGetPtrLast(InstrList * l)
{
  return (void *)l->last;
}

/**
 * Vrátí adresu aktivní instrukce.
 * @param l Seznam
 * @param i Ukazatel na vrácenou instrukci
 */
void ilistGetInstr(InstrList * l, Instr ** i)
{
  *i = l->active->i;
}
