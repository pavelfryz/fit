/*
 * Soubor:   expr.c
 * Resitele: xbartl03, xfryzp00, xhorak34, xstane31
 * Projekt:  Pøekladaè jazyka IFJ11
 * Popis:    Modul pro zpracování výrazù precendenèní syntaktickou
 *           analýzou a volání funkcí.
 */

#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "expr.h"
#include "exprstack.h"
#include "scanner.h"
#include "instruction.h"
#include "ilist.h"
#include "stringoperation.h"
#include "data.h"
#include "ial.h"
#include "error.h"

#define DUNDEF DMARK            // pro neznamy datavy typ vyrazu

#define GETTOKEN()\
  if ((token = getToken(attr, file)) < 0){\
    freeStack(&stack);\
    freeStack(&etype);\
    return token;\
  }

#define GENERATEINSTR(var,type,op)\
  if(generate){\
    if ((var=newInstr(type,op))==NULL){\
      freeStack(&stack);\
      freeStack(&etype);\
      return EALLOC;\
    }\
    if (ilistInsertLast(list,var) < 0){\
      freeStack(&stack);\
      freeStack(&etype);\
      return EALLOC;\
    }\
  }

// deklarace funkce pro zpracovani volani funkce
int functcall(SFunct * funct);

extern InstrList *list;         // seznam istrukcí
extern STable table;            // tabulka symbolù
extern FILE *file;              // vstupní soubor
extern int token;               // aktuální token
extern int result;              // promìnná pro návratové hodnoty
extern tString *attr;           // attribut tokenu
extern tString *functID;        // identifikátor zpracovávané funkce
static bool generate = true;    // zda se maji generovat instrukce

// referencni promenna
static tString STR_NIL = {.str = "nil",.length = 3, };

/**
 * Precedenèní tabulka
 */
static const int PTable[16][16] = {
/*       ^    *    /    +    -    ..   ==   ~=   <    >    <=   >=   (    )    i    $  */
/* ^ */ {CLT, CGT, CGT, CGT, CGT, CGT, CGT, CGT, CGT, CGT, CGT, CGT, CLT, CGT, CLT, CGT},
/* * */ {CLT, CGT, CGT, CGT, CGT, CGT, CGT, CGT, CGT, CGT, CGT, CGT, CLT, CGT, CLT, CGT},
/* / */ {CLT, CGT, CGT, CGT, CGT, CGT, CGT, CGT, CGT, CGT, CGT, CGT, CLT, CGT, CLT, CGT},
/* + */ {CLT, CLT, CLT, CGT, CGT, CGT, CGT, CGT, CGT, CGT, CGT, CGT, CLT, CGT, CLT, CGT},
/* - */ {CLT, CLT, CLT, CGT, CGT, CGT, CGT, CGT, CGT, CGT, CGT, CGT, CLT, CGT, CLT, CGT},
/* .. */{CLT, CLT, CLT, CLT, CLT, CGT, CGT, CGT, CGT, CGT, CGT, CGT, CLT, CGT, CLT, CGT},
/* == */{CLT, CLT, CLT, CLT, CLT, CLT, CGT, CGT, CLT, CLT, CLT, CLT, CLT, CGT, CLT, CGT},
/* ~= */{CLT, CLT, CLT, CLT, CLT, CLT, CGT, CGT, CLT, CLT, CLT, CLT, CLT, CGT, CLT, CGT},
/* < */ {CLT, CLT, CLT, CLT, CLT, CLT, CGT, CGT, CGT, CGT, CGT, CGT, CLT, CGT, CLT, CGT},
/* > */ {CLT, CLT, CLT, CLT, CLT, CLT, CGT, CGT, CGT, CGT, CGT, CGT, CLT, CGT, CLT, CGT},
/* <= */{CLT, CLT, CLT, CLT, CLT, CLT, CGT, CGT, CGT, CGT, CGT, CGT, CLT, CGT, CLT, CGT},
/* >= */{CLT, CLT, CLT, CLT, CLT, CLT, CGT, CGT, CGT, CGT, CGT, CGT, CLT, CGT, CLT, CGT},
/* ( */ {CLT, CLT, CLT, CLT, CLT, CLT, CLT, CLT, CLT, CLT, CLT, CLT, CLT, CEQ, CLT, CER},
/* ) */ {CGT, CGT, CGT, CGT, CGT, CGT, CGT, CGT, CGT, CGT, CGT, CGT, CER, CGT, CER, CGT},
/* i */ {CGT, CGT, CGT, CGT, CGT, CGT, CGT, CGT, CGT, CGT, CGT, CGT, CER, CGT, CER, CGT},
/* $ */ {CLT, CLT, CLT, CLT, CLT, CLT, CLT, CLT, CLT, CLT, CLT, CLT, CLT, CGT, CLT, CER},
};

/**
 * Pøevede vstupní token na odpovídající
 * symbol precendenèní analýzy.
 * @return Odpovídajicí symbol
 */
static inline PSymbol tokenToPSymbol()
{
  static const PSymbol ret[] = {
    POPERAND, POPERAND, POPERAND, PDOLLAR, PDOLLAR, PDOLLAR, POPERAND, POPERAND,
    POPERAND, PDOLLAR, PDOLLAR, PDOLLAR, PDOLLAR, PDOLLAR, PDOLLAR, PDOLLAR, PDOLLAR,
    PDOLLAR, PDOLLAR, PDOLLAR, PDOLLAR, PDOLLAR, PDOLLAR, PDOLLAR, PDOLLAR, PDOLLAR,
    PDOLLAR, PADD, PSUB, PMUL, PDIV, PPOW, PKONKATENACE, PEQ, PNEQ, PLESS, PGREATER,
    PLESS_EQ, PGREATER_EQ, PLEFT_BRACKET, PRIGHT_BRACKET, PDOLLAR, PDOLLAR, PDOLLAR
  };
  return ret[token];
}

/**
 * Pøevede operátor na instrukci.
 * @param operator Operátor
 * @return Odpovídajicí typ instrukce
 */
static inline int operatorToInst(unsigned char operator)
{
  static const int ret[] = {
    IPOW, IMUL, IDIV, IADD, ISUB, IAPPEND, ICMP_EQ, ICMP_NEQ,
    ICMP_LESS, ICMP_GREATER, ICMP_LESS_EQ, ICMP_GREATER_EQ
  };
  return ret[operator];
}

/**
 * Urèí zda je daný symbol operátor
 * @param symbol Symbol
 * @return
 */
static inline int isOperator(unsigned char symbol)
{
  return symbol < PLEFT_BRACKET;
}

/**
 * Urèí zda je mo¾né pou¾ít operátor
 * nad operandy daného typu.
 * @param operator Operátor
 * @param loperand Levý operand
 * @param roperand Pravý operand
 * @return Kompatibilitu operandù k operátoru
 */
static inline int isCompat(unsigned char operator, unsigned char loperand,
                           unsigned char roperand)
{
  static const int ret[12][5][5] = {
/*      | NUMBER(N) | STRING(S) | _BOOL(B)_ | __NIL(n)_ | _UNDEF(U) |*/
/*      | N,S,B,n,U | N,S,B,n,U | N,S,B,n,U | N,S,B,n,U | N,S,B,n,U |*/
/* ^ */ {{1,0,0,0,1},{0,0,0,0,0},{0,0,0,0,0},{0,0,0,0,0},{1,0,0,0,1}},
/* * */ {{1,0,0,0,1},{0,0,0,0,0},{0,0,0,0,0},{0,0,0,0,0},{1,0,0,0,1}},
/* / */ {{1,0,0,0,1},{0,0,0,0,0},{0,0,0,0,0},{0,0,0,0,0},{1,0,0,0,1}},
/* + */ {{1,0,0,0,1},{0,0,0,0,0},{0,0,0,0,0},{0,0,0,0,0},{1,0,0,0,1}},
/* - */ {{1,0,0,0,1},{0,0,0,0,0},{0,0,0,0,0},{0,0,0,0,0},{1,0,0,0,1}},
/* .. */{{0,0,0,0,0},{0,1,0,0,1},{0,0,0,0,0},{0,0,0,0,0},{0,1,0,0,1}},
/* == */{{1,1,1,1,1},{1,1,1,1,1},{1,1,1,1,1},{1,1,1,1,1},{1,1,1,1,1}},
/* ~= */{{1,1,1,1,1},{1,1,1,1,1},{1,1,1,1,1},{1,1,1,1,1},{1,1,1,1,1}},
/* < */ {{1,0,0,0,1},{0,1,0,0,1},{0,0,0,0,0},{0,0,0,0,0},{1,1,0,0,1}},
/* > */ {{1,0,0,0,1},{0,1,0,0,1},{0,0,0,0,0},{0,0,0,0,0},{1,1,0,0,1}},
/* <= */{{1,0,0,0,1},{0,1,0,0,1},{0,0,0,0,0},{0,0,0,0,0},{1,1,0,0,1}},
/* >= */{{1,0,0,0,1},{0,1,0,0,1},{0,0,0,0,0},{0,0,0,0,0},{1,1,0,0,1}}
  };
  return ret[operator][loperand][roperand];
}

/**
 * Urèí datový typ výsledku operace daným operátorem.
 * @param operator Operátor
 * @return Datový typ výsledku operace
 */
static inline int operatorToType(unsigned char operator)
{
  static const int ret[] = {
    DNUMBER, DNUMBER, DNUMBER, DNUMBER, DNUMBER, DSTRING,
    DBOOLEAN, DBOOLEAN, DBOOLEAN, DBOOLEAN, DBOOLEAN, DBOOLEAN
  };
  return ret[operator];
}

/**
 * Zpracování výrazù precendenèní syntaktickou analýzou.
 * @return Chybový kód
 */
int expr()
{
  tStack stack;
  tStack etype;
  PSymbol term;
  PSymbol in;
  initStack(&stack);
  initStack(&etype);
  if(push(&stack, PDOLLAR)==EALLOC)
    return EALLOC;
  term = PDOLLAR;
  in = tokenToPSymbol();

  do {
    int top;
    switch (PTable[term][in]) {
    case CEQ:
      term = PRIGHT_BRACKET;
      if(push(&stack, term)==EALLOC){
        result = EALLOC;
        goto cleanup;
      }
      GETTOKEN();
      in = tokenToPSymbol();
      break;
    case CLT:
      if (token == ID) {
        Symbol *symbol;
        if ((symbol = stableSearch(table, attr, functID)) == NULL)
          goto sem_err;
        if (symbol->type == SVARIABLE) {
          Instr *ins;
          GENERATEINSTR(ins, IPUSH, symbol)
        }
        else {
          GETTOKEN();
          if ((result = functcall(symbol->data)) < 0)
            goto cleanup;
        }
        if(push(&etype, DUNDEF)==EALLOC){
          result = EALLOC;
          goto cleanup;
        }
      }
      else if (token == NUMBER) {
        Symbol *symbol;
        if ((symbol = stableSearch(table, attr, NULL)) == NULL) {
          double d;
          if(stringToDouble(attr, &d)!=EOK)
            goto sem_err;
          if ((symbol = newSymbol(SCONST, attr, newData(DNUMBER, &d))) == NULL) {
            result = EALLOC;
            goto cleanup;
          }
          stableInsert(table, symbol);
        }
        Instr *ins;
        GENERATEINSTR(ins, IPUSH, symbol)
        if(push(&etype, DNUMBER)==EALLOC){
          result = EALLOC;
          goto cleanup;
        }
      }
      else if (token == STRING) {
        Symbol *symbol;
        tString *key = malloc(sizeof(tString));
        if (key == NULL) {
          result = EALLOC;
          goto cleanup;
        }
        if (stringCopy(key, attr) == EALLOC) {
          free(key);
          result = EALLOC;
          goto cleanup;
        }
        if (addChar(key, '$') == EALLOC) {
          free(key);
          result = EALLOC;
          goto cleanup;
        }
        if ((symbol = stableSearch(table, key, NULL)) == NULL) {
          if ((symbol = newSymbol(SCONST, key, newData(DSTRING, attr))) == NULL) {
            result = EALLOC;
            goto cleanup;
          }
          stableInsert(table, symbol);
        }
        stringFree(key);
        free(key);
        Instr *ins;
        GENERATEINSTR(ins, IPUSH, symbol)
        if(push(&etype, DSTRING)==EALLOC){
          result = EALLOC;
          goto cleanup;
        }
      }
      else if (token == TRUE) {
        Symbol *symbol;
        if ((symbol = stableSearch(table, attr, NULL)) == NULL) {
          bool b = true;
          if ((symbol = newSymbol(SCONST, attr, newData(DBOOLEAN, &b))) == NULL) {
            result = EALLOC;
            goto cleanup;
          }
          stableInsert(table, symbol);
        }
        Instr *ins;
        GENERATEINSTR(ins, IPUSH, symbol)
        if(push(&etype, DBOOLEAN)==EALLOC){
          result = EALLOC;
          goto cleanup;
        }
      }
      else if (token == FALSE) {
        Symbol *symbol;
        if ((symbol = stableSearch(table, attr, NULL)) == NULL) {
          bool b = false;
          if ((symbol = newSymbol(SCONST, attr, newData(DBOOLEAN, &b))) == NULL) {
            result = EALLOC;
            goto cleanup;
          }
          stableInsert(table, symbol);
        }
        Instr *ins;
        GENERATEINSTR(ins, IPUSH, symbol)
        if(push(&etype, DBOOLEAN)==EALLOC){
          result = EALLOC;
          goto cleanup;
        }
      }
      else if (token == NIL) {
        Symbol *symbol;
        symbol = stableSearch(table, attr, NULL);
        Instr *ins;
        GENERATEINSTR(ins, IPUSH, symbol)
        if(push(&etype, DNIL)==EALLOC){
          result = EALLOC;
          goto cleanup;
        }
      }
      if(pushCLT(&stack)==EALLOC){
        result = EALLOC;
        goto cleanup;
      }
      term = in;
      if(push(&stack, term)==EALLOC){
        result = EALLOC;
        goto cleanup;
      }
      GETTOKEN();
      in = tokenToPSymbol();
      break;
    case CGT:
      top = pop(&stack);
      if (top == POPERAND) {
        if (pop(&stack) != CLT)
          goto synt_err;
      }
      else if (top == PRIGHT_BRACKET) {
        if (pop(&stack) != NONTERM)
          goto synt_err;
        if (pop(&stack) != PLEFT_BRACKET)
          goto synt_err;
        if (pop(&stack) != CLT)
          goto synt_err;
      }
      else if (top == NONTERM) {
        int p = pop(&stack);
        if (p == PDOLLAR)
          in = PDOLLAR;
        else {
          int operator= p;
          if (isOperator(operator)) {
            Instr *ins;
            GENERATEINSTR(ins, operatorToInst(operator), NULL);
          }
          else
            goto synt_err;
          if (pop(&stack) != NONTERM)
            goto synt_err;
          if (!isCompat(operator, pop(&etype), pop(&etype)))
            goto sem_err;
          if(push(&etype, operatorToType(operator))==EALLOC){
            result = EALLOC;
            goto cleanup;
          }
          if (pop(&stack) != CLT)
            goto synt_err;
        }
      }
      else
        goto synt_err;
      if(push(&stack, NONTERM)==EALLOC){
        result = EALLOC;
        goto cleanup;
      }
      term = topTerm(&stack);
      break;
    case CER:
      goto synt_err;
      break;
    }
  } while (term != PDOLLAR || in != PDOLLAR);
  freeStack(&stack);
  return pop(&etype);

 cleanup:
  freeStack(&stack);
  freeStack(&etype);
  return result;
 synt_err:
  freeStack(&stack);
  freeStack(&etype);
  return SYNT_ERR;
 sem_err:
  freeStack(&stack);
  freeStack(&etype);
  return SEM_ERR;
}

/**
 * Funkce pro analýzu volnání funkce.
 * Pravidlo:
 *   <functcall> -> "(" [<expr> {,<expr>}] ")"
 * @param funct Volaná funkce
 * @return Chybový kód
 */
int functcall(SFunct * funct)
{
  bool gen = generate;
  int paramCnt = funct->paramCnt;
  Instr *ins;
  if (generate) {
    if ((ins = newInstr(IMARK, NULL)) == NULL)
      return EALLOC;
    if (ilistInsertLast(list, ins) < 0)
      return EALLOC;
  }
  if (token != LEFT_BRACKET) {
    fprintf(stderr, "Chyba: Ocekavana leva zavorka '(' pri volani funkce.\n");
    return SYNT_ERR;
  }
  if ((token = getToken(attr, file)) < 0)
    return token;
  if (token != RIGHT_BRACKET) {
    if (paramCnt == 0)
      generate = false;
    if ((result = expr()) < 0)
      return result;
    paramCnt--;
    while (token != RIGHT_BRACKET) {
      if (paramCnt == 0)        // zpracovano dostatek parametru, pro dalsi
        generate = false;       // parametry se nebudou generovat instrukce
      if (token != COMMA) {
        fprintf(stderr,
                "Chyba: Ocekavana prava zavorka ')' pri volani funkce.\n");
        return SYNT_ERR;
      }
      if ((token = getToken(attr, file)) < 0)
        return token;
      if ((result = expr()) < 0)
        return result;
      paramCnt--;
    }
  }
  // urcite byla prava zavorka
  if (paramCnt > 0) {           // funkce byla volana s mene parametry
    Instr *ins;                 // dalsi parametry doplnime hodnotou NIL
    Symbol *symbol = stableSearch(table, &STR_NIL, NULL);
    do {
      if (generate) {
        if ((ins = newInstr(IPUSH, symbol)) == NULL)
          return EALLOC;
        if (ilistInsertLast(list, ins) < 0)
          return EALLOC;
      }
    } while (--paramCnt);
  }
  generate = gen;
  if (generate) {
    if ((ins = newInstr(ICALL, funct->addr)) == NULL)
      return EALLOC;
    if (ilistInsertLast(list, ins) < 0)
      return EALLOC;
  }
  return EOK;
}
