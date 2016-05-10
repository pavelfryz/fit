/*
 * Soubor:   exprstack.c
 * Resitele: xbartl03, xfryzp00, xhorak34, xstane31
 * Projekt:  Překladač jazyka IFJ11
 * Popis:    Funkce pro práci se zásobníkem využívané precendenční
 *           syntaktickou analýzou.
 */

#include <stdlib.h>
#include "exprstack.h"
#include "expr.h"
#include "error.h"

/**
 * Inicializuje zásobník.
 * @param s Zásobník
 */
void initStack(tStack * s)
{
  s->top = NULL;
}

/**
 * Smaže zásobník.
 * @param s Zásobník
 */
void freeStack(tStack * s)
{
  while (s->top != NULL) {
    tElem *del = s->top;
    s->top = s->top->next;
    free(del);
  }
}

/**
 * Vloží hodnotu na vrchol zásobníku.
 * @param s Zásobník
 * @param val Hodnota
 * @return Chybový kód
 */
int push(tStack * s, int val)
{
  tElem *tmp = malloc(sizeof(tElem));
  if (tmp == NULL)
    return EALLOC;
  else {
    tmp->val = val;
    tmp->next = s->top;
    s->top = tmp;
  }
  return EOK;
}

/**
 * Odebere vrchol zásobníku a vrátí hodnotu.
 * @param s Zásobník
 * @return Hodnotu na vrcholu
 */
int pop(tStack * s)
{
  tElem *del = s->top;
  int ret = del->val;
  s->top = s->top->next;
  free(del);
  return ret;
}

/**
 * Vrátí nejvrchnější terminální symbol.
 * @param s Zásobník
 * @return Hodnotu nejvyššího terminálu
 */
int topTerm(tStack * s)
{
  tElem *tmp = s->top;
  while (tmp != NULL) {
    if (tmp->val != NONTERM)
      return tmp->val;
    tmp = tmp->next;
  }
  return PDOLLAR;
}

/**
 * Vloží řídící znak CLT('<') za nejvyšší terminál.
 * @param s Zásobník
 * @return Chybový kód
 */
int pushCLT(tStack * s)
{
  tElem *tmp = malloc(sizeof(tElem));
  if (tmp == NULL)
    return EALLOC;
  else {
    tmp->val = CLT;
    if (s->top->val == NONTERM) {
      tmp->next = s->top->next;
      s->top->next = tmp;
    }
    else {
      tmp->next = s->top;
      s->top = tmp;
    }
  }
  return EOK;
}
