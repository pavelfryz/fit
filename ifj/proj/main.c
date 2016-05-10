/*
 * Soubor:   main.c
 * Resitele: xbartl03, xfryzp00, xhorak34, xstane31
 * Projekt:  Pøekladaè jazyka IFJ11
 * Popis:    Hlavní program pøekladaèe IFJ11.
 */

#include <stdio.h>
#include "parser.h"
#include "interpret.h"
#include "ilist.h"
#include "ial.h"
#include "error.h"

/**
 * Hlavní program.
 */
int main(int argc, char *argv[])
{
  char *srcfile;
  if (argc == 2)
    srcfile = argv[1];
  else
    return -INTER_ERR;
  FILE *file;
  if ((file = fopen(srcfile, "r")) == NULL)
    return -INTER_ERR;
  int retVal;
  InstrList ilist;
  STable stable;
  ilistInit(&ilist);
  if (stableInit(&stable) == EALLOC) {
    fclose(file);
    return -INTER_ERR;
  }
  retVal = parse(&ilist, stable, file);
  fclose(file);
  if (retVal == EOK) {
    retVal = inter(&ilist);
  }
  if (retVal == EALLOC)
    retVal = INTER_ERR;
  stableFree(stable);
  ilistFree(&ilist);
  return -retVal;
}
