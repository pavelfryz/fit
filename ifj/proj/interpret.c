/*
 * Soubor:   interpret.c
 * Resitele: xbartl03, xfryzp00, xhorak34, xstane31
 * Projekt:  Pøekladaè jazyka IFJ11
 * Popis:    Modul interpretu
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <math.h>
#include <ctype.h>

#include "ilist.h"
#include "instruction.h"
#include "datastack.h"
#include "data.h"
#include "ial.h"
#include "stringoperation.h"
#include "error.h"

//deklarace pomocných funkcí - ka¾dá funkce odpovídá jedné instrukci
int ipush(void *);
int ipop(void *);
int iread(void *);
int iwrite(void *);
int imark(void *);
int icall(void *);
int iret(void *);
int ijmpz(void *);
int ijmp(void *);
int ihalt(void *);
int iadd(void *);
int isub(void *);
int imul(void *);
int idiv(void *);
int ipow(void *);
int icmp_eq(void *);
int icmp_neq(void *);
int icmp_qreater(void *);
int icmp_less(void *);
int icmp_greater_eq(void *);
int icmp_less_eq(void *);
int iappend(void *);
int itype(void *);
int isubstr(void *);
int ifind(void *);
int isort(void *);
//konec deklarací pomocných funkcí

static DataStack *stack;
static InstrList *list;

// referencni promenne
static tString STR_NIL = {.str = "nil",.length = 3, };
static tString STR_STRING = {.str = "string",.length = 6, };
static tString STR_NUMBER = {.str = "number",.length = 6, };
static tString STR_BOOLEAN = {.str = "boolean",.length = 7, };

/**
 * Interpretuje seznam instrukcí.
 * @param l Seznam instrukcí
 * @return Chybový kód
 */
int inter(InstrList * l)
{
  DataStack s;
  Instr *i;
  stack = &s;
  list = l;

  dsInit(stack);
  ilistFirst(list);
  while (1) {
    ilistGetInstr(list, &i);
    static int (*const run[]) (void *) = {
    ipush, ipop, iread, iwrite, imark, icall, iret, ijmpz, ijmp,
          ihalt, iadd, isub, imul, idiv, ipow, icmp_eq, icmp_neq,
          icmp_qreater, icmp_less, icmp_greater_eq, icmp_less_eq, iappend,
          itype, isubstr, ifind, isort};
    int retVal = run[i->type] (i->op);
    if (retVal < 0) {
      dsClear(stack);
      return retVal;
    }
    if (retVal == IHALT) {
      dsClear(stack);
      return EOK;
    }
    ilistNext(list);
  }
}

/**
 * Instrukce IPUSH - vlo¾í operand instrukce na vrchol zásobníku.
 * @param op Operand instrukce (adresa symbolu z tabulky symbolù)
 * @return Chybový kód
 */
int ipush(void *op)
{
  Data *d;
  if (((Symbol *) op)->type == SCONST) {
    d = copyData(((Symbol *) op)->data);
    if (d == NULL)
      return EALLOC;
  }
  else {
    int pos = ((SVar *) ((Symbol *) op)->data)->pos;
    if (dsAt(stack, pos, &d) == EALLOC)
      return EALLOC;
  }
  if (dsPush(stack, d) == EALLOC)
    return EALLOC;
  return EOK;
}

/**
 * Instrukce IPOP - ulo¾í hodnotu z vrcholu zásobníku do promìnné.
 * @param op Operand instrukce (adresa promìnné obsahující pozici dat na zásobníku)
 * @return Chybový kód
 */
int ipop(void *op)
{
  Data *d;
  dsPop(stack, &d);
  dsActualizeAt(stack, ((SVar *) op)->pos, d);
  return EOK;
}

#define READ_ERR 1

/**
 * Funkce pro naètení celé èásti èísla. První
 * znak se bere z parametru c, dal¹í ze standartního
 * vstupu. Do c ulo¾en první ¹patný znak.
 * @param c Ukazatel na znak
 * @param s Øetìzec kam se pøidávají èíslice
 * @return Chybový kód
 */
static inline int readIntPart(int *c, tString *s){
  if(!isdigit(*c)){
    ungetc(*c,stdin);
    return READ_ERR;
  }else{
    if(addChar(s,*c)==EALLOC) return EALLOC;
    while(isdigit(*c=getchar()))
      if(addChar(s,*c)==EALLOC) return EALLOC;
  }
  return EOK;
}

/**
 * Funkce pro naètení desetinné èásti èísla. První
 * znak se bere z parametru c, dal¹í ze standartního
 * vstupu. Do c ulo¾en první ¹patný znak.
 * @param c Ukazatel na znak
 * @param s Øetìzec kam se pøidávají èíslice
 * @return Chybový kód
 */
static inline int readFractionPart(int *c, tString *s){
  if (*c=='.'){
    if (!isdigit(*c=getchar())){
      ungetc(*c,stdin);
      return READ_ERR;
    }else{
      if(addChar(s,'.')==EALLOC) return EALLOC;
      if(addChar(s,*c)==EALLOC) return EALLOC;
      while(isdigit(*c=getchar()))
        if(addChar(s,*c)==EALLOC) return EALLOC;
    }
  }
  return EOK;
}

/**
 * Funkce pro naètení exponenciální èásti èísla. První
 * znak se bere z parametru c, dal¹í ze standartního
 * vstupu. Do c ulo¾en první ¹patný znak.
 * @param c Ukazatel na znak
 * @param s Øetìzec kam se pøidávají èíslice
 * @return Chybový kód
 */
static inline int readExpPart(int *c, tString *s){
  if (*c=='e'){
    if(addChar(s,*c)==EALLOC) return EALLOC;
    if ('+'==(*c=getchar()) || '-'==*c){
      if(addChar(s,*c)==EALLOC) return EALLOC;
      *c=getchar();
    }
    if (!isdigit(*c)){
      ungetc(*c,stdin);
      return READ_ERR;
    }else {
      if(addChar(s,*c)==EALLOC) return EALLOC;
      while(isdigit(*c=getchar()))
        if(addChar(s,*c)==EALLOC) return EALLOC;
    }
  }
  ungetc(*c,stdin);
  return EOK;
}


/**
 * Instrukce IREAD - pøeète hodnotu ze vstupu a ulo¾í ji na zásobník.
 * @param op Operand instrukce (format nacteni)
 * @return Chybový kód
 */
int iread(void *op)
{
  Data *data=NULL;
  int c;
  double d;
  tString s;
  if (((Data *) op)->type == DNUMBER) {
    int i = *(double *)((Data *) op)->data;
    if((c = getchar()) == EOF){
      data = newData(DNIL,NULL);
    }
    else{
      if (stringInit(&s) == EALLOC)
        return EALLOC;
      if(i==0)
        ungetc(c,stdin);
      else{
        i--;
        if (addChar(&s, c) == EALLOC)
            return EALLOC;
        while (i && (c = getchar()) != EOF) {
          if (addChar(&s, c) == EALLOC)
            return EALLOC;
          i--;
        }
      }
      data = newData(DSTRING, &s);
      stringFree(&s);
    }
  }
  else {
    switch (((tString *) ((Data *) op)->data)->str[1]) {
    case 'n':
      if (stringInit(&s) == EALLOC)
        return EALLOC;
      while(isspace(c=getchar()));   // preskoci bile znaky
      int err;
      err=readIntPart(&c,&s);        // nacte celou cast cisla
      if(err==EOK){
        err=readFractionPart(&c,&s); // nacte desetinou cast cisla
        if(err==EOK)
          err=readExpPart(&c,&s);    // nacte exponencialni cast cisla
      }
      if(err==EALLOC)
       return EALLOC;
      else if(err==READ_ERR)
        data = newData(DNIL,NULL);
      else{
        if(stringToDouble(&s, &d)!=EOK){
          stringFree(&s);
          return RUN_ERR;
        }
        data = newData(DNUMBER, &d);
      }
      stringFree(&s);
      break;
    case 'l':
      if (stringInit(&s) == EALLOC)
        return EALLOC;
      if((c=getchar()) == EOF)
        data = newData(DNIL, NULL);
      else if(c!='\n'){
        if (addChar(&s, c) == EALLOC)
            return EALLOC;
        while ((c = getchar()) != EOF && c != '\n') {
          if (addChar(&s, c) == EALLOC)
            return EALLOC;
        }
        data = newData(DSTRING, &s);
      }
      else data = newData(DSTRING, &s);
      stringFree(&s);
      break;
    case 'a':
      if (stringInit(&s) == EALLOC)
        return EALLOC;
      while ((c = getchar()) != EOF) {
        if (addChar(&s, c) == EALLOC)
          return EALLOC;
      }
      data = newData(DSTRING, &s);
      stringFree(&s);
      break;
    }
  }

  if (data == NULL)
      return EALLOC;
  if (dsPush(stack, data) == EALLOC)
    return EALLOC;
  return EOK;
}

/**
 * Instrukce IWRITE - vypí¹e hodnotu promìné na vrcholu zásobníku.
 * @param op Operand instrukce (NULL)
 * @return Chybový kód
 */
int iwrite(void *op)
{
  Data *d;
  dsPop(stack, &d);
  if (d->type == DNIL || d->type == DBOOLEAN) { //chyba interpretace
    freeData(d);
    return RUN_ERR;
  }
  else if (d->type == DSTRING) {
    printf("%s", ((tString *) d->data)->str);
  }
  else {
    printf("%g", *(double *)d->data);
  }
  freeData(d);
  return EOK;
}

/**
 * Instrukce IMARK - vlo¾í znaèku na vrchol zásobníku.
 * Znaèka slou¾í pro ulo¾ení adresy instrukce ICALL.
 * @param op Operand instrukce (NULL)
 * @return Chybový kód
 */
int imark(void *op)
{
  Data *m = newData(DMARK, NULL);
  if (m == NULL)
    return EALLOC;
  if (dsPush(stack, m) == EALLOC)
    return EALLOC;
  if (dsMark(stack) == EALLOC)
    return EALLOC;
  return EOK;
}

/**
 * Instrukce ICALL - volání funkce.
 * Ulo¾í adresu instrukce ICALL na døíve oznaèené místo
 * istrukcí IMARK a nastaví následující instrukci dle operandu.
 * @param op Operand instrukce (adresa volané funkce)
 * @return Chybový kód
 */
int icall(void *op)
{
  Data *m = newData(DMARK, ilistGetPtr(list));
  if (m == NULL)
    return EALLOC;
  dsActivateMark(stack);
  dsActualizeAt(stack, 0, m);
  ilistGoto(list, op);
  return EOK;
}

/**
 * Instrukce IRET - návrat z funkce.
 * Sma¾e zásobník a¾ po oznaèené místo instrukcí IMARK(vèetnì).
 * Na vrchol zásobníku ulo¾í návratovou hodnotu a provede
 * skok na adresu ulo¾enou na oznaèeném místì.
 * @param op Operand instrukce (NULL)
 * @return Chybový kód
 */
int iret(void *op)
{
  Data *retVal, *inst;
  dsPop(stack, &retVal);
  dsClearToMark(stack);
  dsPop(stack, &inst);
  ilistGoto(list, inst->data);
  freeData(inst);
  if (dsPush(stack, retVal) == EALLOC)
    return EALLOC;
  return EOK;
}

/**
 * Instrukce IJMPZ - pokud je hodnota na zásobníku false nebo nil,
 * provede skok na adresu operandu.
 * @param op Operand instrukce (adresa skoku)
 * @return Chybový kód
 */
int ijmpz(void *op)
{
  Data *d;
  dsPop(stack, &d);
  if (d->type == DNIL || (d->type == DBOOLEAN && *(bool *) (d->data) == false)) {
    ilistGoto(list, op);
  }
  freeData(d);
  return EOK;
}

/**
 * Instrukce IJMP - provede skok na adresu operandu.
 * @param op Operand instrukce (adresa skoku)
 * @return Chybový kód
 */
int ijmp(void *op)
{
  ilistGoto(list, op);
  return EOK;
}

/**
 * Instrukce IHALT - konec programu.
 * @param op Operand instrukce (NULL)
 * @return Chybový kód
 */
int ihalt(void *op)
{
  return IHALT;
}

/**
 * Instrukce IADD - seète dvì hodnoty na zásobníku
 * a výsledek ulo¾í na vrchol zásobníku.
 * @param op Operand instrukce (NULL)
 * @return Chybový kód
 */
int iadd(void *op)
{
  Data *x, *y;
  dsPop(stack, &x);
  dsPop(stack, &y);
  if (x->type == DNUMBER && y->type == DNUMBER) {
    *(double *)x->data = *(double *)x->data + *(double *)y->data;
    if (!isfinite(*(double *)x->data)) {  //chyba interpretace
      freeData(x);
      freeData(y);
      return RUN_ERR;
    }
    if (dsPush(stack, x) == EALLOC) {
      freeData(y);
      return EALLOC;
    }
  }
  else {                        //chyba interpretace
    freeData(x);
    freeData(y);
    return RUN_ERR;
  }
  freeData(y);
  return EOK;
}

/**
 * Instrukce ISUB - provede rozdíl dvou hodnot na zásobníku
 * a výsledek ulo¾í na vrchol zásobníku.
 * @param op Operand instrukce (NULL)
 * @return Chybový kód
 */
int isub(void *op)
{
  Data *x, *y;
  dsPop(stack, &x);
  dsPop(stack, &y);
  if (x->type == DNUMBER && y->type == DNUMBER) {
    *(double *)x->data = *(double *)y->data - *(double *)x->data;
    if (!isfinite(*(double *)x->data)) {  //chyba interpretace
      freeData(x);
      freeData(y);
      return RUN_ERR;
    }
    if (dsPush(stack, x) == EALLOC) {
      freeData(y);
      return EALLOC;
    }
  }
  else {                        //chyba interpretace
    freeData(x);
    freeData(y);
    return RUN_ERR;
  }
  freeData(y);
  return EOK;
}

/**
 * Instrukce IMUL - vynásobí dvì hodnoty na zásobníku
 * a výsledek ulo¾í na vrchol zásobníku.
 * @param op Operand instrukce (NULL)
 * @return Chybový kód
 */
int imul(void *op)
{
  Data *x, *y;
  dsPop(stack, &x);
  dsPop(stack, &y);
  if (x->type == DNUMBER && y->type == DNUMBER) {
    *(double *)x->data = *(double *)x->data * *(double *)y->data;
    if (!isfinite(*(double *)x->data)) {  //chyba interpretace
      freeData(x);
      freeData(y);
      return RUN_ERR;
    }
    if (dsPush(stack, x) == EALLOC) {
      freeData(y);
      return EALLOC;
    }
  }
  else {                        //chyba interpretace
    freeData(x);
    freeData(y);
    return RUN_ERR;
  }
  freeData(y);
  return EOK;
}

/**
 * Instrukce IDIV - vypoète podíl dvou hodnot na zásobníku
 * a výsledek ulo¾í na vrchol zásobníku.
 * @param op Operand instrukce (NULL)
 * @return Chybový kód
 */
int idiv(void *op)
{
  Data *x, *y;
  dsPop(stack, &x);
  dsPop(stack, &y);
  if (x->type == DNUMBER && y->type == DNUMBER) {
    *(double *)x->data = *(double *)y->data / *(double *)x->data;
    if (!isfinite(*(double *)x->data)) {  //chyba interpretace
      freeData(x);
      freeData(y);
      return RUN_ERR;
    }
    if (dsPush(stack, x) == EALLOC) {
      freeData(y);
      return EALLOC;
    }
  }
  else {                        //chyba interpretace
    freeData(x);
    freeData(y);
    return RUN_ERR;
  }
  freeData(y);
  return EOK;
}

/**
 * Instrukce IPOW - umocni hodnotu na zásobníku
 * dle mocnitele ulo¾eným na zásobníku
 * a výsledek ulo¾í na vrchol zásobníku.
 * @param op Operand instrukce (NULL)
 * @return Chybový kód
 */
int ipow(void *op)
{
  Data *x, *y;
  dsPop(stack, &x);
  dsPop(stack, &y);
  if (x->type == DNUMBER && y->type == DNUMBER) {
    *(double *)x->data = pow(*(double *)y->data, *(double *)x->data);
    if (!isfinite(*(double *)x->data)) {  //chyba interpretace
      freeData(x);
      freeData(y);
      return RUN_ERR;
    }
    if (dsPush(stack, x) == EALLOC) {
      freeData(y);
      return EALLOC;
    }
  }
  else {                        //chyba interpretace
    freeData(x);
    freeData(y);
    return RUN_ERR;
  }
  freeData(y);
  return EOK;
}

/**
 * Instrukce ICMP_EQ - porovná dvì hodnoty na zásobníku
 * na rovnost a výsledek ulo¾í na vrchol zásobníku.
 * @param op Operand instrukce (NULL)
 * @return Chybový kód
 */
int icmp_eq(void *op)
{
  Data *x, *y;
  bool z;
  dsPop(stack, &x);
  dsPop(stack, &y);
  z = isEqData(x, y);
  freeData(x);
  freeData(y);
  x = newData(DBOOLEAN, &z);
  if (x == NULL)
    return EALLOC;
  if (dsPush(stack, x) == EALLOC)
    return EALLOC;
  return EOK;
}

/**
 * Instrukce ICMP_NEQ - porovná dvì hodnoty na zásobníku
 * na nerovnost a výsledek ulo¾í na vrchol zásobníku.
 * @param op Operand instrukce (NULL)
 * @return Chybový kód
 */
int icmp_neq(void *op)
{
  Data *x, *y;
  bool z;
  dsPop(stack, &x);
  dsPop(stack, &y);
  z = !isEqData(x, y);
  freeData(x);
  freeData(y);
  x = newData(DBOOLEAN, &z);
  if (x == NULL)
    return EALLOC;
  if (dsPush(stack, x) == EALLOC)
    return EALLOC;
  return EOK;
}

/**
 * Instrukce ICMP_GREATER - porovná dvì hodnoty na zásobníku
 * jetli je první hodnota vìt¹í ne¾ druhá
 * a výsledek ulo¾í na vrchol zásobníku.
 * @param op Operand instrukce (NULL)
 * @return Chybový kód
 */
int icmp_qreater(void *op)
{
  Data *x, *y;
  bool z;
  dsPop(stack, &x);
  dsPop(stack, &y);
  if (x->type == DNUMBER && y->type == DNUMBER) {
    z = *(double *)y->data > *(double *)x->data;
  }
  else if (x->type == DSTRING && y->type == DSTRING) {
    z = strcmp(((tString *) y->data)->str, ((tString *) x->data)->str) > 0;
  }
  else {                        //chyba interpretace
    freeData(x);
    freeData(y);
    return RUN_ERR;
  }
  freeData(x);
  freeData(y);
  x = newData(DBOOLEAN, &z);
  if (x == NULL)
    return EALLOC;
  if (dsPush(stack, x) == EALLOC)
    return EALLOC;
  return EOK;
}

/**
 * Instrukce ICMP_LESS - porovná dvì hodnoty na zásobníku
 * jetli je první hodnota men¹í ne¾ druhá
 * a výsledek ulo¾í na vrchol zásobníku.
 * @param op Operand instrukce (NULL)
 * @return Chybový kód
 */
int icmp_less(void *op)
{
  Data *x, *y;
  bool z;
  dsPop(stack, &x);
  dsPop(stack, &y);
  if (x->type == DNUMBER && y->type == DNUMBER) {
    z = *(double *)y->data < *(double *)x->data;
  }
  else if (x->type == DSTRING && y->type == DSTRING) {
    z = strcmp(((tString *) y->data)->str, ((tString *) x->data)->str) < 0;
  }
  else {                        //chyba interpretace
    freeData(x);
    freeData(y);
    return RUN_ERR;
  }
  freeData(x);
  freeData(y);
  x = newData(DBOOLEAN, &z);
  if (x == NULL)
    return EALLOC;
  if (dsPush(stack, x) == EALLOC)
    return EALLOC;
  return EOK;
}

/**
 * Instrukce ICMP_GREATER_EQ - porovná dvì hodnoty na zásobníku
 * jetli je první hodnota vìt¹í ne¾ druhá nebo rovna
 * a výsledek ulo¾í na vrchol zásobníku.
 * @param op Operand instrukce (NULL)
 * @return Chybový kód
 */
int icmp_greater_eq(void *op)
{
  Data *x, *y;
  bool z;
  dsPop(stack, &x);
  dsPop(stack, &y);
  if (x->type == DNUMBER && y->type == DNUMBER) {
    z = *(double *)y->data >= *(double *)x->data;
  }
  else if (x->type == DSTRING && y->type == DSTRING) {
    z = strcmp(((tString *) y->data)->str, ((tString *) x->data)->str) >= 0;
  }
  else {                        //chyba interpretace
    freeData(x);
    freeData(y);
    return RUN_ERR;
  }
  freeData(x);
  freeData(y);
  x = newData(DBOOLEAN, &z);
  if (x == NULL)
    return EALLOC;
  if (dsPush(stack, x) == EALLOC)
    return EALLOC;
  return EOK;
}

/**
 * Instrukce ICMP_LESS_EQ - porovná dvì hodnoty na zásobníku
 * jetli je první hodnota men¹í ne¾ druhá nebo rovna
 * a výsledek ulo¾í na vrchol zásobníku.
 * @param op Operand instrukce (NULL)
 * @return Chybový kód
 */
int icmp_less_eq(void *op)
{
  Data *x, *y;
  bool z;
  dsPop(stack, &x);
  dsPop(stack, &y);
  if (x->type == DNUMBER && y->type == DNUMBER) {
    z = *(double *)y->data <= *(double *)x->data;
  }
  else if (x->type == DSTRING && y->type == DSTRING) {
    z = strcmp(((tString *) y->data)->str, ((tString *) x->data)->str) <= 0;
  }
  else {                        //chyba interpretace
    freeData(x);
    freeData(y);
    return RUN_ERR;
  }
  freeData(x);
  freeData(y);
  x = newData(DBOOLEAN, &z);
  if (x == NULL)
    return EALLOC;
  if (dsPush(stack, x) == EALLOC)
    return EALLOC;
  return EOK;
}

/**
 * Instrukce IAPPEND - provede konkatenaci dvou hodnot na zásobníku
 * a výsledek ulo¾í na vrchol zásobníku.
 * @param op Operand instrukce (NULL)
 * @return Chybový kód
 */
int iappend(void *op)
{
  Data *x, *y;
  dsPop(stack, &x);
  dsPop(stack, &y);
  if (x->type == DSTRING && y->type == DSTRING) {
    if (stringAppend(y->data, x->data) == EALLOC) {
      freeData(x);
      freeData(y);
      return EALLOC;
    }
    if (dsPush(stack, y) == EALLOC) {
      freeData(x);
      return EALLOC;
    }
  }
  else {                        //chyba interpretace
    freeData(x);
    freeData(y);
    return RUN_ERR;
  }
  freeData(x);
  return EOK;
}

/**
 * Instrukce ITYPE - na vrchol zásobníku pøidá øetìzec
 * obsahující datový typ pøede¹lého vrcholu
 * @param op Operand instrukce (NULL)
 * @return Chybový kód
 */
int itype(void *op)
{
  Data *d;
  tString *s = malloc(sizeof(tString));
  if (s == NULL)
    return EALLOC;
  dsPop(stack, &d);
  switch (d->type) {
  case DNIL:
    if (stringCopy(s, &STR_NIL) == EALLOC) {
      free(s);
      freeData(d);
      return EALLOC;
    }
    break;
  case DNUMBER:
    if (stringCopy(s, &STR_NUMBER) == EALLOC) {
      free(s);
      freeData(d);
      return EALLOC;
    }
    break;
  case DSTRING:
    if (stringCopy(s, &STR_STRING) == EALLOC) {
      free(s);
      freeData(d);
      return EALLOC;
    }
    break;
  case DBOOLEAN:
    if (stringCopy(s, &STR_BOOLEAN) == EALLOC) {
      free(s);
      freeData(d);
      return EALLOC;
    }
    break;
  default:
    break;
  }
  freeData(d);
  d = newData(DSTRING, s);
  stringFree(s);
  free(s);
  if (d == NULL)
    return EALLOC;
  if (dsPush(stack, d) == EALLOC)
    return EALLOC;
  return EOK;
}

/**
 * Instrukce ISUBSTR - na vrchol zásobníku pøidá
 * podøetìzec øetìzce na zadané pozici.
 * @param op Operand instrukce (NULL)
 * @return Chybový kód
 */
int isubstr(void *op)
{
  Data *text, *first, *last;
  dsPop(stack, &last);
  dsPop(stack, &first);
  dsPop(stack, &text);
  if (text->type == DSTRING && first->type == DNUMBER && last->type == DNUMBER) {
    stringSubstr(text->data, *(double *)first->data, *(double *)last->data);
    freeData(first);
    freeData(last);
  }
  else {
    freeData(text);
    freeData(first);
    freeData(last);
    text = newData(DNIL, NULL);
    if (text == NULL)
      return EALLOC;
  }
  if (dsPush(stack, text) == EALLOC)
    return EALLOC;
  return EOK;
}

/**
 * Instrukce IFIND - na vrchol zásobníku pøidá èíslo,
 * udávající pozici nalezeného podøetìzce v øetìzci
 * @param op Operand instrukce (NULL)
 * @return Chybový kód
 */
int ifind(void *op)
{
  Data *x, *y;
  int pos;
  bool found;
  dsPop(stack, &x);
  dsPop(stack, &y);
  if (x->type == DSTRING && y->type == DSTRING) {
    pos = find(y->data, x->data);
    found = ((tString *) y->data)->length >= pos;
    freeData(x);
    freeData(y);
    if (pos == EALLOC)
      return EALLOC;
    if (found) {
      double dpos = pos;
      x = newData(DNUMBER, &dpos);
      if (x == NULL)
        return EALLOC;
    }
    else {
      x = newData(DBOOLEAN, &found);
      if (x == NULL)
        return EALLOC;
    }
  }
  else {
    freeData(x);
    freeData(y);
    x = newData(DNIL, NULL);
    if (x == NULL)
      return EALLOC;
  }
  if (dsPush(stack, x) == EALLOC)
    return EALLOC;
  return EOK;
}

/**
 * Instrukce ISORT - na vrchol zásobníku pøidá seøazený øetìzec,
 * který byl na vrcholu nebo NIL
 * @param op Operand instrukce (NULL)
 * @return Chybový kód
 */
int isort(void *op)
{
  Data *d;
  dsPop(stack, &d);
  if (d->type == DSTRING) {
    heapSort(((tString *) d->data)->length, ((tString *) d->data)->str);
  }
  else {
    freeData(d);
    d = newData(DNIL, NULL);
    if (d == NULL)
      return EALLOC;
  }
  if (dsPush(stack, d) == EALLOC)
    return EALLOC;
  return EOK;
}
