/*
 * Soubor:   ilist.c
 * Resitele: xbartl03, xfryzp00, xhorak34, xstane31
 * Projekt:  P�eklada� jazyka IFJ11
 * Popis:    Funkce pro pr�ci se seznamem instrukc�.
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
 * Sma�e seznam.
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
 * Vlo�� instrukci na konec seznamu.
 * @param l Seznam
 * @param i Instrukce
 * @return Chybov� k�d
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
 * Nastav� aktivitu seznamu na prvn� instrukci.
 * @param l Seznam
 */
void ilistFirst(InstrList * l)
{
  l->active = l->first;
}

/**
 * Nastav� aktivitu seznamu na posledn� instrukci.
 * @param l Seznam
 */
void ilistLast(InstrList * l)
{
  l->active = l->first;
}

/**
 * Nastav� aktivitu seznamu na dal�� instrukci.
 * @param l Seznam
 */
void ilistNext(InstrList * l)
{
  l->active = l->active->next;
}

/**
 * Nastav� aktivitu seznamu na zadanou adresu.
 * @param l Seznam
 * @param gotoInstr Adresa
 */
void ilistGoto(InstrList * l, void *gotoInstr)
{
  l->active = (ListItem *) gotoInstr;
}

/**
 * Vr�t� adresu aktivn� polo�ky seznamu.
 * @param l Seznam
 * @return Adresa polo�ky seznamu
 */
void *ilistGetPtr(InstrList * l)
{
  return (void *)l->active;
}

/**
 * Vr�t� adresu posledn� polo�ky seznamu.
 * @param l Seznam
 * @return Adresa polo�ky seznamu
 */
void *ilistGetPtrLast(InstrList * l)
{
  return (void *)l->last;
}

/**
 * Vr�t� adresu aktivn� instrukce.
 * @param l Seznam
 * @param i Ukazatel na vr�cenou instrukci
 */
void ilistGetInstr(InstrList * l, Instr ** i)
{
  *i = l->active->i;
}
