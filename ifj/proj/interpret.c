/*
 * Soubor:   interpret.c
 * Resitele: xbartl03, xfryzp00, xhorak34, xstane31
 * Projekt:  P�eklada� jazyka IFJ11
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

//deklarace pomocn�ch funkc� - ka�d� funkce odpov�d� jedn� instrukci
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
//konec deklarac� pomocn�ch funkc�

static DataStack *stack;
static InstrList *list;

// referencni promenne
static tString STR_NIL = {.str = "nil",.length = 3, };
static tString STR_STRING = {.str = "string",.length = 6, };
static tString STR_NUMBER = {.str = "number",.length = 6, };
static tString STR_BOOLEAN = {.str = "boolean",.length = 7, };

/**
 * Interpretuje seznam instrukc�.
 * @param l Seznam instrukc�
 * @return Chybov� k�d
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
 * Instrukce IPUSH - vlo�� operand instrukce na vrchol z�sobn�ku.
 * @param op Operand instrukce (adresa symbolu z tabulky symbol�)
 * @return Chybov� k�d
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
 * Instrukce IPOP - ulo�� hodnotu z vrcholu z�sobn�ku do prom�nn�.
 * @param op Operand instrukce (adresa prom�nn� obsahuj�c� pozici dat na z�sobn�ku)
 * @return Chybov� k�d
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
 * Funkce pro na�ten� cel� ��sti ��sla. Prvn�
 * znak se bere z parametru c, dal�� ze standartn�ho
 * vstupu. Do c ulo�en prvn� �patn� znak.
 * @param c Ukazatel na znak
 * @param s �et�zec kam se p�id�vaj� ��slice
 * @return Chybov� k�d
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
 * Funkce pro na�ten� desetinn� ��sti ��sla. Prvn�
 * znak se bere z parametru c, dal�� ze standartn�ho
 * vstupu. Do c ulo�en prvn� �patn� znak.
 * @param c Ukazatel na znak
 * @param s �et�zec kam se p�id�vaj� ��slice
 * @return Chybov� k�d
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
 * Funkce pro na�ten� exponenci�ln� ��sti ��sla. Prvn�
 * znak se bere z parametru c, dal�� ze standartn�ho
 * vstupu. Do c ulo�en prvn� �patn� znak.
 * @param c Ukazatel na znak
 * @param s �et�zec kam se p�id�vaj� ��slice
 * @return Chybov� k�d
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
 * Instrukce IREAD - p�e�te hodnotu ze vstupu a ulo�� ji na z�sobn�k.
 * @param op Operand instrukce (format nacteni)
 * @return Chybov� k�d
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
 * Instrukce IWRITE - vyp�e hodnotu prom�n� na vrcholu z�sobn�ku.
 * @param op Operand instrukce (NULL)
 * @return Chybov� k�d
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
 * Instrukce IMARK - vlo�� zna�ku na vrchol z�sobn�ku.
 * Zna�ka slou�� pro ulo�en� adresy instrukce ICALL.
 * @param op Operand instrukce (NULL)
 * @return Chybov� k�d
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
 * Instrukce ICALL - vol�n� funkce.
 * Ulo�� adresu instrukce ICALL na d��ve ozna�en� m�sto
 * istrukc� IMARK a nastav� n�sleduj�c� instrukci dle operandu.
 * @param op Operand instrukce (adresa volan� funkce)
 * @return Chybov� k�d
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
 * Instrukce IRET - n�vrat z funkce.
 * Sma�e z�sobn�k a� po ozna�en� m�sto instrukc� IMARK(v�etn�).
 * Na vrchol z�sobn�ku ulo�� n�vratovou hodnotu a provede
 * skok na adresu ulo�enou na ozna�en�m m�st�.
 * @param op Operand instrukce (NULL)
 * @return Chybov� k�d
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
 * Instrukce IJMPZ - pokud je hodnota na z�sobn�ku false nebo nil,
 * provede skok na adresu operandu.
 * @param op Operand instrukce (adresa skoku)
 * @return Chybov� k�d
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
 * @return Chybov� k�d
 */
int ijmp(void *op)
{
  ilistGoto(list, op);
  return EOK;
}

/**
 * Instrukce IHALT - konec programu.
 * @param op Operand instrukce (NULL)
 * @return Chybov� k�d
 */
int ihalt(void *op)
{
  return IHALT;
}

/**
 * Instrukce IADD - se�te dv� hodnoty na z�sobn�ku
 * a v�sledek ulo�� na vrchol z�sobn�ku.
 * @param op Operand instrukce (NULL)
 * @return Chybov� k�d
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
 * Instrukce ISUB - provede rozd�l dvou hodnot na z�sobn�ku
 * a v�sledek ulo�� na vrchol z�sobn�ku.
 * @param op Operand instrukce (NULL)
 * @return Chybov� k�d
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
 * Instrukce IMUL - vyn�sob� dv� hodnoty na z�sobn�ku
 * a v�sledek ulo�� na vrchol z�sobn�ku.
 * @param op Operand instrukce (NULL)
 * @return Chybov� k�d
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
 * Instrukce IDIV - vypo�te pod�l dvou hodnot na z�sobn�ku
 * a v�sledek ulo�� na vrchol z�sobn�ku.
 * @param op Operand instrukce (NULL)
 * @return Chybov� k�d
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
 * Instrukce IPOW - umocni hodnotu na z�sobn�ku
 * dle mocnitele ulo�en�m na z�sobn�ku
 * a v�sledek ulo�� na vrchol z�sobn�ku.
 * @param op Operand instrukce (NULL)
 * @return Chybov� k�d
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
 * Instrukce ICMP_EQ - porovn� dv� hodnoty na z�sobn�ku
 * na rovnost a v�sledek ulo�� na vrchol z�sobn�ku.
 * @param op Operand instrukce (NULL)
 * @return Chybov� k�d
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
 * Instrukce ICMP_NEQ - porovn� dv� hodnoty na z�sobn�ku
 * na nerovnost a v�sledek ulo�� na vrchol z�sobn�ku.
 * @param op Operand instrukce (NULL)
 * @return Chybov� k�d
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
 * Instrukce ICMP_GREATER - porovn� dv� hodnoty na z�sobn�ku
 * jetli je prvn� hodnota v�t�� ne� druh�
 * a v�sledek ulo�� na vrchol z�sobn�ku.
 * @param op Operand instrukce (NULL)
 * @return Chybov� k�d
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
 * Instrukce ICMP_LESS - porovn� dv� hodnoty na z�sobn�ku
 * jetli je prvn� hodnota men�� ne� druh�
 * a v�sledek ulo�� na vrchol z�sobn�ku.
 * @param op Operand instrukce (NULL)
 * @return Chybov� k�d
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
 * Instrukce ICMP_GREATER_EQ - porovn� dv� hodnoty na z�sobn�ku
 * jetli je prvn� hodnota v�t�� ne� druh� nebo rovna
 * a v�sledek ulo�� na vrchol z�sobn�ku.
 * @param op Operand instrukce (NULL)
 * @return Chybov� k�d
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
 * Instrukce ICMP_LESS_EQ - porovn� dv� hodnoty na z�sobn�ku
 * jetli je prvn� hodnota men�� ne� druh� nebo rovna
 * a v�sledek ulo�� na vrchol z�sobn�ku.
 * @param op Operand instrukce (NULL)
 * @return Chybov� k�d
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
 * Instrukce IAPPEND - provede konkatenaci dvou hodnot na z�sobn�ku
 * a v�sledek ulo�� na vrchol z�sobn�ku.
 * @param op Operand instrukce (NULL)
 * @return Chybov� k�d
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
 * Instrukce ITYPE - na vrchol z�sobn�ku p�id� �et�zec
 * obsahuj�c� datov� typ p�ede�l�ho vrcholu
 * @param op Operand instrukce (NULL)
 * @return Chybov� k�d
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
 * Instrukce ISUBSTR - na vrchol z�sobn�ku p�id�
 * pod�et�zec �et�zce na zadan� pozici.
 * @param op Operand instrukce (NULL)
 * @return Chybov� k�d
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
 * Instrukce IFIND - na vrchol z�sobn�ku p�id� ��slo,
 * ud�vaj�c� pozici nalezen�ho pod�et�zce v �et�zci
 * @param op Operand instrukce (NULL)
 * @return Chybov� k�d
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
 * Instrukce ISORT - na vrchol z�sobn�ku p�id� se�azen� �et�zec,
 * kter� byl na vrcholu nebo NIL
 * @param op Operand instrukce (NULL)
 * @return Chybov� k�d
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
