/*
 * Soubor:   datastack.c
 * Resitele: xbartl03, xfryzp00, xhorak34, xstane31
 * Projekt:  Pøekladaè jazyka IFJ11
 * Popis:    Funkce pro práci se zásobníkem vyu¾ívané interpretem.
 *           Do zásobníku se ukládají promìnné definované u¾ivatelem,
 *           promìnné pro mezi-výpoèty, návratové hodnoty a adresy
 *           instrukcí pøi volání funkce, aby se vìdìlo, kde se má
 *           po návratu z funkce pokraèovat.
 */

#include <stdlib.h>
#include "datastack.h"
#include "data.h"
#include "error.h"

/**
 * Inicializuje zásobník.
 * @param s Zásobník
 */
void dsInit(DataStack * s)
{
  s->top = NULL;
  s->mark = NULL;
  s->active = NULL;
}

/**
 * Sma¾e zásobník.
 * @param s Zásobník
 */
void dsClear(DataStack * s)
{
  while (s->top != NULL) {
    Node *tmp = s->top;
    s->top = s->top->prev;
    freeData(tmp->data);
    free(tmp);
  }
  while (s->mark != NULL) {
    Mark *tmp = s->mark;
    s->mark = s->mark->nextMark;
    free(tmp);
  }
  while (s->active != NULL) {
    Mark *tmp = s->active;
    s->active = s->active->nextMark;
    free(tmp);
  }
}

/**
 * Sma¾e zásobník po poslední aktivní znaèku.
 * Oznaèené místo zùstává na vrcholu zásobníku.
 * @param s Zásobník
 */
void dsClearToMark(DataStack * s)
{
  while (s->top != s->active->mark) {
    Node *tmp = s->top;
    s->top = s->top->prev;
    freeData(tmp->data);
    free(tmp);
  }
}

/**
 * Vlo¾í data na vrchol zásobníku.
 * @param s Zásobník
 * @param d Data
 * @return Chybový kód
 */
int dsPush(DataStack * s, Data * d)
{
  Node *tmp = malloc(sizeof(Node));
  if (tmp == NULL) {
    freeData(d);
    return EALLOC;
  }
  else {
    tmp->data = d;
    tmp->prev = s->top;
    tmp->next = NULL;
    if (s->top != NULL)
      s->top->next = tmp;
    s->top = tmp;
  }
  return EOK;
}

/**
 * Odebere vrchol zásobníku a vrátí data.
 * Sma¾e pøípadnou aktivní znaèku.
 * @param s Zásobník
 * @param d Ukazatel na vrácená data
 */
void dsPop(DataStack * s, Data ** d)
{
  if (s->active != NULL && s->top == s->active->mark) {
    Mark *tmp = s->active;
    s->active = s->active->nextMark;
    free(tmp);
  }
  Node *tmp = s->top;
  s->top = s->top->prev;
  if (s->top != NULL)
    s->top->next = NULL;
  *d = tmp->data;
  free(tmp);
}

/**
 * Aktivuje poslední znaèku.
 * @param s Zásobník
 */
void dsActivateMark(DataStack * s)
{
  Mark *tmp = s->mark;
  s->mark = s->mark->nextMark;
  tmp->nextMark = s->active;
  s->active = tmp;
}

/**
 * Oznaèí souèasný vrchol zásobníku.
 * @param s Zásobník
 * @return Chybový kód
 */
int dsMark(DataStack * s)
{
  Mark *tmp = malloc(sizeof(Mark));
  if (tmp == NULL)
    return EALLOC;
  else {
    tmp->nextMark = s->mark;
    tmp->mark = s->top;
    s->mark = tmp;
  }
  return EOK;
}

/**
 * Vrátí data z poslední aktivované pozice + posun.
 * @param s Zásobník.
 * @param pos Posun, pouze ve smeru na dalsi prvky(0,1,..)
 * @param d Ukazatel na vrácená data
 * @return Chybový kód
 */
int dsAt(DataStack * s, int pos, Data ** d)
{
  Node *tmp = s->active->mark;
  while (pos != 0) {
    tmp = tmp->next;
    pos--;
  }
  *d = copyData(tmp->data);
  if (*d == NULL)
    return EALLOC;
  return EOK;
}

/**
 * Pøepí¹e data poslední aktivované pozice + posun.
 * @param s Zásobník
 * @param pos Posun, pouze ve smeru na dalsi prvky(0,1,..)
 * @param d Nová data
 */
void dsActualizeAt(DataStack * s, int pos, Data * d)
{
  Node *tmp = s->active->mark;
  while (pos != 0) {
    tmp = tmp->next;
    pos--;
  }
  freeData(tmp->data);
  tmp->data = d;
}
