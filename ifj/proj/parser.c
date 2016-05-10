/*
 * Soubor:   parser.c
 * Resitele: xbartl03, xfryzp00, xhorak34, xstane31
 * Projekt:  Pøekladaè jazyka IFJ11
 * Popis:    Modul parseru
 */

#include <stdlib.h>
#include <string.h>
#include "parser.h"
#include "scanner.h"
#include "instruction.h"
#include "ilist.h"
#include "stringoperation.h"
#include "data.h"
#include "ial.h"
#include "error.h"
#include "expr.h"

#define GETTOKEN() if ((token = getToken(attr, file)) < 0) return token

#define GENERATEINSTR(var,type,op)\
  if ((var=newInstr(type,op))==NULL) return EALLOC;\
  if (ilistInsertLast(list,var) < 0) return EALLOC

// deklarace pomocných funkcí
int program();
int functlist();
int funct();
int paramlist();
int param();
int declrlist();
int declr();
int statlist();
int stat();
int ifstat();
int exprlist();
int writexpr();
int assign();
// konec deklarací pomocných funkcí

InstrList *list;                // seznam istrukcí
STable table;                   // tabulka symbolù
FILE *file;                     // vstupní soubor
int token;                      // aktuální token
int result;                     // promìnná pro návratové hodnoty
tString *attr;                  // attribut tokenu
tString *functID;               // identifikátor zpracovávané funkce
static int paramCnt;            // poèet parametrù aktuální funkce

// referencni promenne
static tString STR_NIL = {.str = "nil",.length = 3, };
static tString STR_TYPE = {.str = "type",.length = 4, };
static tString STR_SORT = {.str = "sort",.length = 4, };
static tString STR_FIND = {.str = "find",.length = 4, };
static tString STR_SUBSTR = {.str = "substr",.length = 6, };

/**
 * Vstupní funkce rekurzivního sestupu.
 * Pravidlo:
 *   <program> -> <functionlist> "EOF"
 * @return Chybový kód
 */
int program()
{
  Instr *ins;
  Symbol *symbol;

  // Vygenerovani instrukci pro volani funkce main, instrukci
  // pro vestavene funkce a vlozeni identifikatoru pro vestavene
  // funkce do tabulky symbolu
  if ((symbol = newSymbol(SCONST, &STR_NIL, newData(DNIL, NULL))) == NULL)
    return EALLOC;
  stableInsert(table, symbol);
  GENERATEINSTR(ins, IMARK, NULL);
  GENERATEINSTR(ins, ICALL, NULL);
  GENERATEINSTR(ins, IHALT, NULL);
  if ((symbol =
       newSymbol(SFUNCTION, &STR_TYPE,
                 newSFunct(ilistGetPtrLast(list), 1))) == NULL)
    return EALLOC;
  stableInsert(table, symbol);
  GENERATEINSTR(ins, ITYPE, NULL);
  GENERATEINSTR(ins, IRET, NULL);
  if ((symbol =
       newSymbol(SFUNCTION, &STR_SORT,
                 newSFunct(ilistGetPtrLast(list), 1))) == NULL)
    return EALLOC;
  stableInsert(table, symbol);
  GENERATEINSTR(ins, ISORT, NULL);
  GENERATEINSTR(ins, IRET, NULL);
  if ((symbol =
       newSymbol(SFUNCTION, &STR_SUBSTR,
                 newSFunct(ilistGetPtrLast(list), 3))) == NULL)
    return EALLOC;
  stableInsert(table, symbol);
  GENERATEINSTR(ins, ISUBSTR, NULL);
  GENERATEINSTR(ins, IRET, NULL);
  if ((symbol =
       newSymbol(SFUNCTION, &STR_FIND,
                 newSFunct(ilistGetPtrLast(list), 2))) == NULL)
    return EALLOC;
  stableInsert(table, symbol);
  GENERATEINSTR(ins, IFIND, NULL);
  GENERATEINSTR(ins, IRET, NULL);

  GETTOKEN();
  if ((result = functlist()) < 0)
    return result;
  if (token != END_OF_FILE) {
    fprintf(stderr, "Chyba: Za funkci main neni konec souboru.\n");
    return SYNT_ERR;
  }
  return EOK;
}

/**
 * Funkce pro analýzu seznamu funkcí.
 * Pravidlo:
 *   <functlist> -> "function" <funct>
 * @return Chybový kód
 */
int functlist()
{
  if (token != FUNCTION) {
    fprintf(stderr, "Chyba: Ocekavano klicove slovo 'function'.\n");
    return SYNT_ERR;
  }
  GETTOKEN();
  paramCnt = 0;
  if ((result = funct()) < 0)
    return result;
  GETTOKEN();
  return EOK;
}

/**
 * Funkce pro analýzu funkce.
 * Pravidla:
 *   <funct> -> "id" "(" <paramlist> <declrlist> <functionlist>
 *   <funct> -> "main" "(" ")" <declrlist> ";"
 * @return Chybový kód
 */
int funct()
{
  Symbol *symbol;
  if (token != ID) {
    fprintf(stderr, "Chyba: Ocekavan identifikator funkce.\n");
    return SYNT_ERR;
  }

  if (stableSearch(table, attr, NULL) != NULL) {
    fprintf(stderr, "Chyba: Funkce %s jiz byla definovana.\n", attr->str);
    return SEM_ERR;
  }
  if ((symbol = newSymbol(SFUNCTION, attr, newSFunct(NULL, 0))) == NULL)
    return EALLOC;
  stableInsert(table, symbol);
  functID = symbol->key;

  if (strcmp(attr->str, "main") == 0) { // <funct> -> "main" "(" ")" <declrlist> ";"
    Instr *ins;
    ilistFirst(list);
    ilistNext(list);
    ilistGetInstr(list, &ins);
    ins->op = ilistGetPtrLast(list);
    ((SFunct *) symbol->data)->addr = ins->op;
    GETTOKEN();
    if (token != LEFT_BRACKET) {
      fprintf(stderr, "Chyba: Ocekavana leva zavorka '(' u funkce main.\n");
      return SYNT_ERR;
    }
    GETTOKEN();
    if (token != RIGHT_BRACKET) {
      fprintf(stderr, "Chyba: Ocekavana prava zavorka ')' u funkce main.\n");
      return SYNT_ERR;
    }
    GETTOKEN();
    if ((result = declrlist()) < 0)
      return result;
    if (token != SEMICOLON) {
      fprintf(stderr, "Chyba: Chybi strednik ';' za funkci main.\n");
      return SYNT_ERR;
    }
  }
  else {                        // <funct> -> "id" "(" <paramlist> <declrlist> <functionlist>
    GETTOKEN();
    if (token != LEFT_BRACKET) {
      fprintf(stderr, "Chyba: Ocekavana leva zavorka '(' u funkce %s.\n",
              functID->str);
      return SYNT_ERR;
    }
    GETTOKEN();
    if ((result = paramlist()) < 0)
      return result;
    ((SFunct *) symbol->data)->addr = ilistGetPtrLast(list);
    ((SFunct *) symbol->data)->paramCnt = paramCnt;
    if ((result = declrlist()) < 0)
      return result;
    if ((result = functlist()) < 0)
      return result;
  }
  return EOK;
}

/**
 * Funkce pro analýzu seznamu parametrù funkce.
 * Pravidla:
 *   <paramlist>  -> "id" <param>
 *   <paramlist>  -> ")"
 * @return Chybový kód
 */
int paramlist()
{
  Symbol *symbol;
  switch (token) {
  case ID:                     // <paramlist>  -> "id" <param>
    if (stableSearch(table, attr, functID) != NULL) {
      fprintf(stderr, "Chyba: Konflikt názvu parametru %s v funkci %s. "
              "Jiz existuje funkce nebo parametr se stejnym jmenem.\n",
              attr->str, functID->str);
      return SEM_ERR;
    }
    if ((symbol =
         newSymbol(SVARIABLE, attr, newSVar(functID, ++paramCnt))) == NULL)
      return EALLOC;
    stableInsert(table, symbol);
    GETTOKEN();
    if ((result = param()) < 0)
      return result;
    break;
  case RIGHT_BRACKET:          // <paramlist>  -> ")"
    break;
  default:
    fprintf(stderr, "Chyba: Ocekavana prava zavorka ')' u funkce %s.\n",
            functID->str);
    return SYNT_ERR;
  }
  GETTOKEN();
  return EOK;
}

/**
 * Funkce pro analýzu parametru funkce.
 * Pravidla:
 *   <param> -> "," "id" <param>
 *   <param> -> ")"
 * @return Chybový kód
 */
int param()
{
  Symbol *symbol;
  switch (token) {
  case COMMA:                  // <param> -> "," "id" <param>
    GETTOKEN();
    if (token != ID) {
      fprintf(stderr, "Chyba: Ocekavan identifikator parametru v funkci %s.\n",
              functID->str);
      return SYNT_ERR;
    }
    if (stableSearch(table, attr, functID) != NULL) {
      fprintf(stderr, "Chyba: Konflikt názvu parametru %s v funkci %s. "
              "Jiz existuje funkce nebo parametr se stejnym jmenem.\n",
              attr->str, functID->str);
      return SEM_ERR;
    }
    if ((symbol =
         newSymbol(SVARIABLE, attr, newSVar(functID, ++paramCnt))) == NULL)
      return EALLOC;
    stableInsert(table, symbol);
    GETTOKEN();
    if ((result = param()) < 0)
      return result;
    break;
  case RIGHT_BRACKET:          // <param> -> ")"
    break;
  default:
    fprintf(stderr, "Chyba: Ocekavana prava zavorka ')' u funkce %s.\n",
            functID->str);
    return SYNT_ERR;
  }
  return EOK;
}

/**
 * Funkce pro analýzu seznamu deklarací.
 * Pravidla:
 *   <declrlist> -> "local" "id" <declr> <declrlist>
 *   <declrlist> -> <statlist> "end"
 * @return Chybový kód
 */
int declrlist()
{
  Instr *ins;
  Symbol *symbol;
  if (token == LOCAL) {         // <declrlist> -> "local" "id" <declr> <declrlist>
    GETTOKEN();
    if (token != ID) {
      fprintf(stderr, "Chyba: Ocekavana identifikator promene "
              "po klicovem slove local v funkci %s.\n", functID->str);
      return SYNT_ERR;
    }
    if (stableSearch(table, attr, functID) != NULL) {
      fprintf(stderr, "Chyba: Redeklarace promene %s v funkci %s.\n", attr->str,
              functID->str);
      return SEM_ERR;
    }
    if ((symbol =
         newSymbol(SVARIABLE, attr, newSVar(functID, ++paramCnt))) == NULL)
      return EALLOC;
    if ((token = getToken(attr, file)) < 0) {
      freeSymbol(symbol);
      return token;
    }
    if ((result = declr()) < 0) {
      freeSymbol(symbol);
      return result;
    }
    stableInsert(table, symbol);
    if ((result = declrlist()) < 0)
      return result;
  }
  else {                        // <declrlist> -> <statlist> "end"
    if ((result = statlist()) < 0)
      return result;
    if (token != END) {
      fprintf(stderr,
              "Chyba: Ocekavano klicove slovo 'end' na konci funkce %s.\n",
              functID->str);
      return SYNT_ERR;
    }
    symbol = stableSearch(table, &STR_NIL, NULL);
    GENERATEINSTR(ins, IPUSH, symbol);
    GENERATEINSTR(ins, IRET, NULL);
    GETTOKEN();
  }
  return EOK;
}

/**
 * Funkce pro analýzu deklarace.
 * Pravidla:
 *   <declr> -> "=" <expr> ";"
 *   <declr> -> ";"
 * @return Chybový kód
 */
int declr()
{
  Instr *ins;
  Symbol *symbol;
  switch (token) {
  case SEMICOLON:              // <declr> -> ";"
    symbol = stableSearch(table, &STR_NIL, NULL);
    GENERATEINSTR(ins, IPUSH, symbol);
    break;
  case ASSIGN:                 // <declr> -> "=" <expr> ";"
    GETTOKEN();
    if ((result = expr()) < 0)
      return result;
    if (token != SEMICOLON) {
      fprintf(stderr,
              "Chyba: Ocekavan strednik ';' za deklaci promenne v funkci %s.\n",
              functID->str);
      return SYNT_ERR;
    }
    break;
  default:
    fprintf(stderr,
            "Chyba: Ocekavan strednik ';' za deklaci promenne v funkci %s.\n",
            functID->str);
    return SYNT_ERR;
  }
  GETTOKEN();
  return EOK;
}

/**
 * Funkce pro analýzu seznamu pøíkazù.
 * Pravidla:
 *   <statlist> -> <stat> <statlist>
 *   <statlist> -> "end"
 *   <statlist> -> "else"
 *   <statlist> -> "elseif"
 *   <statlist> -> "until"
 * @return Chybový kód
 */
int statlist()
{
  switch (token) {
  case END:                    // <statlist> -> "end"
  case ELSE:                   // <statlist> -> "else"
  case ELSEIF:                 // <statlist> -> "elseif"
  case UNTIL:                  // <statlist> -> "until"
    break;
  default:                     // <statlist> -> <stat> <statlist>
    if ((result = stat()) < 0)
      return result;
    if ((result = statlist()) < 0)
      return result;
  }
  return EOK;
}

/**
 * Funkce pro analýzu pøíkazu.
 * Pravidla:
 *   <stat> -> "while" <expr> "do" <statlist> "end" ";"
 *   <stat> -> "return" <expr> ";"
 *   <stat> -> "write" "(" <exprlist> ";"
 *   <stat> -> "id" "=" <assign> ";"
 *   <stat> -> "repeat" <statlist> "until" <expr> ";"
 *   <stat> -> "if" <expr> "then" <statlist> <ifstat> ";"
 * @return Chybový kód
 */
int stat()
{
  Instr *jmpz;
  Instr *jmp;
  Instr *ins;
  void *start;
  Symbol *symbol;

  switch (token) {
  case WHILE:                  // <stat> -> "while" <expr> "do" <statlist> "end" ";"
    start = ilistGetPtrLast(list);
    GETTOKEN();
    if ((result = expr()) < 0)
      return result;
    if (token != DO) {
      fprintf(stderr,
              "Chyba: Ocekavan klicove slovo 'do' v cyklu while v funkci %s.\n",
              functID->str);
      return SYNT_ERR;
    }
    GENERATEINSTR(jmpz, IJMPZ, NULL);
    GETTOKEN();
    if ((result = statlist()) < 0)
      return result;
    if (token != END) {
      fprintf(stderr,
              "Chyba: Ocekavano klicove slovo 'end' na konci cyklu while v funkci %s.\n",
              functID->str);
      return SYNT_ERR;
    }
    GENERATEINSTR(jmp, IJMP, start);
    GETTOKEN();
    jmpz->op = ilistGetPtrLast(list);
    break;
  case RETURN:                 // <stat> -> "return" <expr> ";"
    GETTOKEN();
    if ((result = expr()) < 0)
      return result;
    GENERATEINSTR(ins, IRET, NULL);
    break;
  case WRITE:                  // <stat> -> "write" "(" <exprlist> ";"
    GETTOKEN();
    if (token != LEFT_BRACKET) {
      fprintf(stderr,
              "Chyba: Ocekavana leve zavorka '(' u prikazu write v funkci %s.\n",
              functID->str);
      return SYNT_ERR;
    }
    GETTOKEN();
    if ((result = exprlist()) < 0)
      return result;
    break;
  case ID:                     // <stat> -> "id" "=" <assign> ";"
    if ((symbol = stableSearch(table, attr, functID)) == NULL) {
      fprintf(stderr, "Chyba: Identifikator %s v funkci %s nebyl deklarovan.\n",
              attr->str, functID->str);
      return SEM_ERR;
    }
    if (symbol->type != SVARIABLE) {
      fprintf(stderr, "Chyba: Volani funkce %s v funkci %s bez prirazeni.\n",
              attr->str, functID->str);
      return SEM_ERR;
    }
    GETTOKEN();
    if (token != ASSIGN) {
      fprintf(stderr, "Chyba: Ocekavano '=' v funkci %s.\n", functID->str);
      return SYNT_ERR;
    }
    GETTOKEN();
    if ((result = assign()) < 0)
      return result;
    GENERATEINSTR(ins, IPOP, symbol->data);
    break;
  case REPEAT:                 // <stat> -> "repeat" <statlist> "until" <expr> ";"
    start = ilistGetPtrLast(list);
    GETTOKEN();
    if ((result = statlist()) < 0)
      return result;
    if (token != UNTIL) {
      fprintf(stderr,
              "Chyba: Ocekavan klicove slovo 'until' v cyklu repeat v funkci %s.\n",
              functID->str);
      return SYNT_ERR;
    }
    GETTOKEN();
    if ((result = expr()) < 0)
      return result;
    GENERATEINSTR(jmpz, IJMPZ, start);
    break;
  case IF:                     // <stat> -> "if" <expr> "then" <statlist> <ifstat> ";"
    GETTOKEN();
    if ((result = expr()) < 0)
      return result;
    if (token != THEN) {
      fprintf(stderr,
              "Chyba: Ocekavan klicove slovo 'then' u prikazu if v funkci %s.\n",
              functID->str);
      return SYNT_ERR;
    }
    GENERATEINSTR(jmpz, IJMPZ, NULL);
    GETTOKEN();
    if ((result = statlist()) < 0)
      return result;
    GENERATEINSTR(jmp, IJMP, NULL);
    jmpz->op = ilistGetPtrLast(list);
    if ((result = ifstat()) < 0)
      return result;
    jmp->op = ilistGetPtrLast(list);
    GETTOKEN();
    break;
  default:
    fprintf(stderr, "Chyba: Neznamy prikaz v funkci %s.\n", functID->str);
    return SYNT_ERR;
  }

  if (token != SEMICOLON) {
    fprintf(stderr,
            "Chyba: Ocekavan strednik ';' na konci prikazu v funkci %s.\n",
            functID->str);
    return SYNT_ERR;
  }
  GETTOKEN();
  return EOK;
}

/**
 * Funkce pro analýzu pøíkazu if.
 * Pravidla:
 *   <ifstat> -> "end"
 *   <ifstat> -> "else" <statlist> "end"
 *   <ifstat> -> "elseif" <expr> "then" <statlist> <ifstat>
 * @return Chybový kód
 */
int ifstat()
{
  Instr *jmpz;
  Instr *jmp;

  switch (token) {
  case END:                    // <ifstat> -> "end"
    break;
  case ELSE:                   // <ifstat> -> "else" <statlist> "end"
    GETTOKEN();
    if ((result = statlist()) < 0)
      return result;
    if (token != END) {
      fprintf(stderr,
              "Chyba: Ocekavano klicove slovo 'end' na konci prikazu if v funkci %s.\n",
              functID->str);
      return SYNT_ERR;
    }
    break;
  case ELSEIF:                 // <ifstat> -> "elseif" <expr> "then" <statlist> <ifstat>
    GETTOKEN();
    if ((result = expr()) < 0)
      return result;
    if (token != THEN) {
      fprintf(stderr,
              "Chyba: Ocekavan klicove slovo 'then' u prikazu elseif v funkci %s.\n",
              functID->str);
      return SYNT_ERR;
    }
    GENERATEINSTR(jmpz, IJMPZ, NULL);
    GETTOKEN();
    if ((result = statlist()) < 0)
      return result;
    GENERATEINSTR(jmp, IJMP, NULL);
    jmpz->op = ilistGetPtrLast(list);
    if ((result = ifstat()) < 0)
      return result;
    jmp->op = ilistGetPtrLast(list);
    break;
  default:
    fprintf(stderr,
            "Chyba: Ocekavano klicove slovo 'end' na konci prikazu if v funkci %s.\n",
            functID->str);
    return SYNT_ERR;
  }
  return EOK;
}

/**
 * Funkce pro analýzu seznamu parametrù pøíkazu write.
 * Pravidla:
 *   <exprlist> -> <expr> <writexpr>
 *   <exprlist> -> ")"
 * @return Chybový kód
 */
int exprlist()
{
  // <exprlist> -> <expr> <writexpr>
  if (token != RIGHT_BRACKET) {
    if ((result = expr()) < 0)
      return result;
    if (result == DBOOLEAN || result == DNIL) {
      fprintf(stderr,
              "Chyba: Vyraz v prikazu write je typu NIL, nebo BOOLEAN v funkci %s.\n",
              functID->str);
      return SEM_ERR;
    }
    if ((result = writexpr()) < 0)
      return result;
    // <exprlist> -> ")"
  }
  GETTOKEN();
  return EOK;
}

/**
 * Funkce pro analýzu parametru pøíkazu write.
 * Pravidla:
 *   <writexpr> -> "," <expr> <writexpr>
 *   <writexpr> -> ")"
 * @return Chybový kód
 */
int writexpr()
{
  Instr *ins;
  switch (token) {
  case RIGHT_BRACKET:          // <writexpr> -> ")"
    GENERATEINSTR(ins, IWRITE, NULL);
    break;
  case COMMA:                  // <writexpr> -> "," <expr> <writexpr>
    GENERATEINSTR(ins, IWRITE, NULL);
    GETTOKEN();
    if ((result = expr()) < 0)
      return result;
    if (result == DBOOLEAN || result == DNIL) {
      fprintf(stderr,
              "Chyba: Vyraz v prikazu write je typu NIL, nebo BOOLEAN v funkci %s.\n",
              functID->str);
      return SEM_ERR;
    }
    if ((result = writexpr()) < 0)
      return result;
    break;
  default:
    fprintf(stderr,
            "Chyba: Ocekavana prava zavorka ')' u prikazu write v funkci %s.\n",
            functID->str);
    return SYNT_ERR;
  }
  return EOK;
}

/**
 * Funkce pro analýzu pøiøazení.
 * Pravidla:
 *   <assign> -> <expr>
 *   <assign> -> "read" "(" "format" ")"
 * @return Chybový kód
 */
int assign()
{
  Instr *ins;
  double i;
  Symbol *symbol;
  if (token != READ) {          // <assign> -> <expr>
    if ((result = expr()) < 0)
      return result;
  }
  else {                        // <assign> -> "read" "(" "format" ")"
    GETTOKEN();
    if (token != LEFT_BRACKET) {
      fprintf(stderr,
              "Chyba: Ocekava leva zavorka '(' u prikazu read v funkci %s.\n",
              functID->str);
      return SYNT_ERR;
    }
    GETTOKEN();
    if ((token != STRING) && (token != NUMBER)) {
      fprintf(stderr,
              "Chyba: Parametr prikazu read v funkci %s neni retezec ani cislo.\n",
              functID->str);
      return SYNT_ERR;
    }
    if (token == NUMBER) {
      if(stringToDouble(attr, &i)!=EOK)
        return SEM_ERR;
      if ((symbol = stableSearch(table, attr, NULL)) == NULL) {
        if ((symbol = newSymbol(SCONST, attr, newData(DNUMBER, &i))) == NULL)
          return EALLOC;
        stableInsert(table, symbol);
      }
    }
    else {
      if (attr->length < 2)
        return SEM_ERR;
      else {
        attr->str[2] = '\0';
        attr->length = 2;
      }
      if (!(!strcmp(attr->str, "*n") || !strcmp(attr->str, "*l")
            || !strcmp(attr->str, "*a"))) {
        fprintf(stderr,
                "Chyba: Neznamy formatovaci retezec prikazu read v funkci %s.\n",
                functID->str);
        return SEM_ERR;
      }
      tString *key = malloc(sizeof(tString));
      if (key == NULL)
        return EALLOC;
      if (stringCopy(key, attr) == EALLOC) {
        free(key);
        return EALLOC;
      }
      if (addChar(key, '$') == EALLOC) {
        free(key);
        return EALLOC;
      }
      if ((symbol = stableSearch(table, key, NULL)) == NULL) {
        if ((symbol = newSymbol(SCONST, key, newData(DSTRING, attr))) == NULL)
          return EALLOC;
        stableInsert(table, symbol);
      }
      stringFree(key);
      free(key);
    }
    GENERATEINSTR(ins, IREAD, symbol->data);
    GETTOKEN();
    if (token != RIGHT_BRACKET) {
      fprintf(stderr,
              "Chyba: Ocekava prava zavorka ')' u prikazu read v funkci %s.\n",
              functID->str);
      return SYNT_ERR;
    }
    GETTOKEN();
  }
  return EOK;
}

/**
 * Analyzuje zdrojový kód,
 * generuje instrukce a
 * pøidává symboly do tabulky symbolù.
 * @param argList Seznam instrukcí
 * @param argTable Tabulka symbolù
 * @param argF Zdrojový soubor
 * @return Chybový kód
 */
int parse(InstrList * l, STable t, FILE * f)
{
  tString s;
  list = l;
  table = t;
  file = f;
  attr = &s;
  if (stringInit(attr) == EALLOC)
    return EALLOC;
  result = program();
  stringFree(attr);
  return result;
}
