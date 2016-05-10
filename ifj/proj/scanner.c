/*
 * Soubor:   scanner.c
 * Resitele: xbartl03, xfryzp00, xhorak34, xstane31
 * Projekt:  Pøekladaè jazyka IFJ11
 * Popis:    Modul scanneru
 */

#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include "scanner.h"
#include "stringoperation.h"
#include "error.h"

/**
 * Výèet stavu automatu
 */
typedef enum TState {
  init,                         /**< Poèáteèní stav automatu */
  minus,                        /**< Naèten znak mínus -> mo¾ný komentáø */
  comment,                      /**< Naèteny 2 mínus po sobì (jednoøádkový nebo blokový komentáø) */
  rim,                          /**< Naèten znak [ -> mo¾ný blokový komentáø */
  block,                        /**< Dal¹í znak [ -> blokový komentáø */
  rimEnd,                       /**< Naèten znak ] -> mù¾e nastat ukonèení blokového komentáøe */
  identificator,                /**< První naètený znak je písmeno nebo znak podtr¾ítka -> identifikátor */
  number,                       /**< První naètený znak je èíslice -> naèítáme èíslo */
  equal,                        /**< První naètený znak je = -> buï pøiøazení, nebo ekvivalence */
  dotNumber,                    /**< V naèítaném èísle byla naètena teèka */
  decimal,                      /**< Po naètení teèky se u¾ v èísle nesmí objevit dal¹í teèka */
  concatenation,                /**< Byla naètena teèka -> mo¾ná konkatenace */
  notEq,                        /**< Naèten znak ~ -> oèekava se nonekvivalence */
  lessChar,                     /**< Naèten znak < */
  greaterChar,                  /**< Naèten znak > */
  invComma,                     /**< Naèten znak " -> oèekává se øetìzec */
  expNumber,                    /**< Po naètení exponentu u¾ se oèekávají jen èíslice */
  finishHim,                    /**< Oèekává se ukonèení jednoøádkového komentáøe */
  exp,
  expSign,                      /**< Po zadání znaménka u exponentu se musí naèíst alespoò jedna èíslice */
  escape                        /**< Naèten znak \ -> oèekává se escape sekvence */
} TState;

/** Tabulka klíèových a rezervovaných slov */
const char *KEYWORDS[] = {
  "if", "then", "else", "true", "false", "nil", "while", "do",
  "end", "function", "local", "return", "read", "write", "and",
  "or", "not", "break", "elseif", "for", "in", "repeat", "until"
};

/**
 * Vrátí token ze zdrojového souboru.
 * @param s Øetìzec
 * @param f Zdrojový soubor
 * @return Typ tokenu
 */
int getToken(tString * s, FILE * f)
{
  TState state = init;
  int c;
  stringClear(s);               //vymaze se retezec
  while (1) {
    c = getc(f);                //nacteni znaku ze souboru
    switch (state) {
    case init:                 //pocatecni stav automatu
      if (isspace(c))           //bila mista se preskakuji
        state = init;
      else if (isalpha(c) || c == '_') {
        if (addChar(s, c) == EALLOC)
          return EALLOC;
        state = identificator;
      }
      else if (isdigit(c)) {
        if (addChar(s, c) == EALLOC)
          return EALLOC;
        state = number;
      }
      else if (c == ';')
        return SEMICOLON;
      else if (c == '+')
        return ADD;
      else if (c == '*')
        return MUL;
      else if (c == '/')
        return DIV;
      else if (c == '^')
        return POW;
      else if (c == '-')
        state = minus;
      else if (c == '=')
        state = equal;
      else if (c == '.')
        state = concatenation;
      else if (c == '~')
        state = notEq;
      else if (c == '<')
        state = lessChar;
      else if (c == '>')
        state = greaterChar;
      else if (c == '"')
        state = invComma;
      else if (c == '(')
        return LEFT_BRACKET;
      else if (c == ')')
        return RIGHT_BRACKET;
      else if (c == ',')
        return COMMA;
      else if (c == EOF)
        return END_OF_FILE;
      else
        return LEX_ERR;
      break;
    case identificator:
      if (isalnum(c) || c == '_') { //nacten dalsi znak - vlozi se do retezce
        if (addChar(s, c) == EALLOC)
          return EALLOC;
      }
      else {                    //jinak konec identifikatoru
        ungetc(c, f);           //musi se vratit posledni nacteny znak
        for (int i = IF; i < ASSIGN; i++) {
          if (strcmp(s->str, KEYWORDS[i - IF]) == 0)  //a pak se porovnava s tabulkou klicovych a rezervovanych slov
            return (i);
        }
        return ID;
      }
      break;
    case minus:
      if (c == '-')
        state = comment;        //komentar
      else {
        ungetc(c, f);           //pokud nenasleduje dalsi minus nebude to komentar
        return SUB;
      }
      break;
    case comment:
      if (c == '[')
        state = rim;            //pokud by se nacetl znak [ -> ocekava se blokovy komentar
      else if (c == '\n')
        state = init;
      else
        state = finishHim;      //jinak pouze jednoradkovy komentar
      break;
    case rim:
      if (c == '[')             //pokud by se nacetl dalsi znak [ -> blokovy komentar
        state = block;
      else if (c == '\n')
        state = init;
      else
        state = finishHim;      //pokud byl nacten pouze jeden znak [ -> je to jednoradkovy komentar
      break;
    case block:
      if (c == ']')             //ocekavam ukonceni blokoveho komentare
        state = rimEnd;
      else if (c == EOF)        //pokud by nastal EOF v blokovem komentari, byla by to chyba
        return LEX_ERR;
      break;
    case rimEnd:
      if (c == ']')
        state = init;           //blokovy komentar ukoncen, navrat do pocatecniho stavu
      else if (c == EOF)        //pokud by nastal EOF v blokovem komentari, byla by to chyba
        return LEX_ERR;
      else
        state = block;          //pokud nebyl kometar ukoncen, vraci se do predesleho stavu
      break;
    case number:
      if (isdigit(c)) {
        if (addChar(s, c) == EALLOC)
          return EALLOC;        //cislice se vkladaji do retezce
      }
      else if (c == 'e' || c == 'E') {
        if (addChar(s, c) == EALLOC)
          return EALLOC;
        state = exp;
      }
      else if (c == '.') {      //pokud byla nactena tecka, bude se jednat o desetinne cislo
        if (addChar(s, c) == EALLOC)
          return EALLOC;
        state = dotNumber;
      }
      else {
        ungetc(c, f);           //u jinych znaku -> posledni znak se vrati
        return NUMBER;
      }
      break;
    case equal:
      if (c == '=')             //pokud bylo nacten dalsi znak '=' -> jde o ==
        return EQ;
      else {
        ungetc(c, f);
        return ASSIGN;          //jinak bylo zadano jen =
      }
      break;
    case dotNumber:
      if (isdigit(c)) {         //po nacteni tecky v cisle se ocekava cislice
        if (addChar(s, c) == EALLOC)
          return EALLOC;
        state = decimal;
      }
      else
        return LEX_ERR;         //pokud nebyla za teckou nactena cislice, je to chyba
      break;
    case decimal:
      if (isdigit(c)) {
        if (addChar(s, c) == EALLOC)
          return EALLOC;        //cislice se vkladaji do retezce
      }
      else if (c == 'e' || c == 'E') {
        if (addChar(s, c) == EALLOC)
          return EALLOC;
        state = exp;
      }
      else {
        ungetc(c, f);           //u jinych znaku -> posledni znak se vrati
        return NUMBER;
      }
      break;
    case concatenation:
      if (c == '.')             //pokud by se po tecce nacetla dalsi tecka, jedna se o konkatenaci
        return KONKATENACE;
      else {
        ungetc(c, f);
        return LEX_ERR;         //nactena jen jedna tecka - vracim error
      }
      break;
    case notEq:
      if (c == '=')             //po nacteni znaku '~' ocekavam '='
        return NEQ;
      else {
        ungetc(c, f);
        return LEX_ERR;         //jinak je to chyba
      }
      break;
    case lessChar:
      if (c == '=')
        return LESS_EQ;         //po nacteni '<' a '='
      else {
        ungetc(c, f);
        return LESS;
      }
      break;
    case greaterChar:
      if (c == '=')
        return GREATER_EQ;      //po nacteni '>' a '='
      else {
        ungetc(c, f);
        return GREATER;
      }
      break;
    case invComma:
      if (c == '"')
        return STRING;          //ceka se na ukonceni retezce
      else if (c == EOF)
        return LEX_ERR;         //pokud by nastal EOF, je to chyba
      else if (c == '\\')
        state = escape;         //escape sekvence
      else if(c<32)
        fprintf(stderr, "VAROVÁNÍ: Øetìzec obsahuje znak s ascii hodnotou men¹í jak 32\n");
      else if (addChar(s, c) == EALLOC)
        return EALLOC;          //jinak se znaky vkladaji
      break;
    case escape:
      if (c == 'n') {
        if (addChar(s, '\n') == EALLOC)
          return EALLOC;
      }
      else if (c == 't') {
        if (addChar(s, '\t') == EALLOC)
          return EALLOC;
      }
      else if (c == '\\') {
        if (addChar(s, '\\') == EALLOC)
          return EALLOC;
      }
      else if (c == '"') {
        if (addChar(s, '"') == EALLOC)
          return EALLOC;
      }
      else if (isdigit(c)) {
        int i = c - '0';
        if (!isdigit(c = getc(f)))
          return LEX_ERR;
        i = 10 * i + c - '0';
        if (!isdigit(c = getc(f)))
          return LEX_ERR;
        i = 10 * i + c - '0';
        if (i == 0 || i > 255)
          return LEX_ERR;
        if (addChar(s, i) == EALLOC)
          return EALLOC;
      }
      else
        return LEX_ERR;
      state = invComma;
      break;
    case expNumber:
      if (isdigit(c)) {
        if (addChar(s, c) == EALLOC)
          return EALLOC;        //po nacteni exponentu se ocekavaji pouze cislice
      }
      else {
        ungetc(c, f);
        return NUMBER;
      }
      break;
    case finishHim:
      if (c == '\n')
        state = init;
      break;
    case exp:
      if (isdigit(c)) {
        if (addChar(s, c) == EALLOC)
          return EALLOC;
        state = expNumber;
      }
      else if (c == '-' || c == '+') {
        if (addChar(s, c) == EALLOC)
          return EALLOC;
        state = expSign;
      }
      else
        return LEX_ERR;
      break;
    case expSign:
      if (isdigit(c)) {
        if (addChar(s, c) == EALLOC)
          return EALLOC;        //po nacteni exponentu se ocekavaji pouze cislice
        state = expNumber;
      }
      else
        return LEX_ERR;
      break;
    default:
      return LEX_ERR;
    }
  }
}
