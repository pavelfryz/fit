/*
 * Soubor:   instruction.c
 * Resitele: xbartl03, xfryzp00, xhorak34, xstane31
 * Projekt:  Pøekladaè jazyka IFJ11
 * Popis:    Funkce pro práci s instrukcemi pou¾itými v seznamu instrukcí.
 */

#include <stdlib.h>
#include "instruction.h"

/**
 * Vytvoøí novì alokovaný objekt typu Instr.
 * V pøípadì chyby alokace vrací NULL.
 * @param t Typ instrukce
 * @param op Operand instrukce
 * @return Novì vytvoøený objekt
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
 * Vytvoøí kopii instrukce.
 * V pøípadì chyby alokace vrací NULL.
 * @param i Instrukce
 * @return Ukazatel na zkopírovanou instrukci
 */
Instr *copyInstr(Instr * i)
{
  return newInstr(i->type, i->op);
}

/**
 * Sma¾e instrukci.
 * @param i Instrukce
 */
void freeInstr(Instr * i)
{
  free(i);
}
