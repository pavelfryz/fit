/*
 * Soubor:   instruction.c
 * Resitele: xbartl03, xfryzp00, xhorak34, xstane31
 * Projekt:  P�eklada� jazyka IFJ11
 * Popis:    Funkce pro pr�ci s instrukcemi pou�it�mi v seznamu instrukc�.
 */

#include <stdlib.h>
#include "instruction.h"

/**
 * Vytvo�� nov� alokovan� objekt typu Instr.
 * V p��pad� chyby alokace vrac� NULL.
 * @param t Typ instrukce
 * @param op Operand instrukce
 * @return Nov� vytvo�en� objekt
 */
Instr *newInstr(InstrType t, void *op)
{
  Instr *i = malloc(sizeof(Instr));
  if (i == NULL)
    return NULL;
  else {
    i->op = op;
    i->type = t;
  }
  return i;
}

/**
 * Vytvo�� kopii instrukce.
 * V p��pad� chyby alokace vrac� NULL.
 * @param i Instrukce
 * @return Ukazatel na zkop�rovanou instrukci
 */
Instr *copyInstr(Instr * i)
{
  return newInstr(i->type, i->op);
}

/**
 * Sma�e instrukci.
 * @param i Instrukce
 */
void freeInstr(Instr * i)
{
  free(i);
}
