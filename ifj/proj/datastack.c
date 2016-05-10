/*
 * Soubor:   datastack.c
 * Resitele: xbartl03, xfryzp00, xhorak34, xstane31
 * Projekt:  P�eklada� jazyka IFJ11
 * Popis:    Funkce pro pr�ci se z�sobn�kem vyu��van� interpretem.
 *           Do z�sobn�ku se ukl�daj� prom�nn� definovan� u�ivatelem,
 *           prom�nn� pro mezi-v�po�ty, n�vratov� hodnoty a adresy
 *           instrukc� p�i vol�n� funkce, aby se v�d�lo, kde se m�
 *           po n�vratu z funkce pokra�ovat.
 */

#include <stdlib.h>
#include "datastack.h"
#include "data.h"
#include "error.h"

/**
 * Inicializuje z�sobn�k.
 * @param s Z�sobn�k
 */
void dsInit(DataStack * s)
{
  s->top = NULL;
  s->mark = NULL;
  s->active = NULL;
}

/**
 * Sma�e z�sobn�k.
 * @param s Z�sobn�k
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
 * Sma�e z�sobn�k po posledn� aktivn� zna�ku.
 * Ozna�en� m�sto z�st�v� na vrcholu z�sobn�ku.
 * @param s Z�sobn�k
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
 * Vlo�� data na vrchol z�sobn�ku.
 * @param s Z�sobn�k
 * @param d Data
 * @return Chybov� k�d
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
 * Odebere vrchol z�sobn�ku a vr�t� data.
 * Sma�e p��padnou aktivn� zna�ku.
 * @param s Z�sobn�k
 * @param d Ukazatel na vr�cen� data
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
 * Aktivuje posledn� zna�ku.
 * @param s Z�sobn�k
 */
void dsActivateMark(DataStack * s)
{
  Mark *tmp = s->mark;
  s->mark = s->mark->nextMark;
  tmp->nextMark = s->active;
  s->active = tmp;
}

/**
 * Ozna�� sou�asn� vrchol z�sobn�ku.
 * @param s Z�sobn�k
 * @return Chybov� k�d
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
 * Vr�t� data z posledn� aktivovan� pozice + posun.
 * @param s Z�sobn�k.
 * @param pos Posun, pouze ve smeru na dalsi prvky(0,1,..)
 * @param d Ukazatel na vr�cen� data
 * @return Chybov� k�d
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
 * P�ep�e data posledn� aktivovan� pozice + posun.
 * @param s Z�sobn�k
 * @param pos Posun, pouze ve smeru na dalsi prvky(0,1,..)
 * @param d Nov� data
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
