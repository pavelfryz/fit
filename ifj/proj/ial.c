/*
 * Soubor:   ial.h
 * Resitele: xbartl03, xfryzp00, xhorak34, xstane31
 * Projekt:  Pøekladaè jazyka IFJ11
 * Popis:    Funkce pro práci s tabulkou symbolù(hashovací tabulka),
 *           vyhledávání podøetìzce v øetìzci(BMA) a øazení(heapsort)
 */

#include <stdlib.h>
#include <string.h>
#include "ial.h"
#include "stringoperation.h"
#include "error.h"
#include "data.h"

#define CHAR_COUNT 256
#define HASH_TABLE_SIZE 193

////////////////////////////////////////////////////////////////////////
//                Tabulka symbolu (hashovaci tabulka)                 //
////////////////////////////////////////////////////////////////////////

/**
 * Hashovací funkce.
 * @param s Øetìzec
 * @return Index do tabulky
 */
int hashFunction(tString * s)
{
  unsigned int h = 0;
  char *p;
  for (p = s->str; *p != '\0'; p++)
    h = 31 * h + *p;
  return h % HASH_TABLE_SIZE;
}

/**
 * Inicializuje tabulku symbolù.
 * @param t Hashovací tabulka
 * @return Chybový kód
 */
int stableInit(STable * t)
{
  *t = malloc(HASH_TABLE_SIZE * sizeof(struct Symbol *));
  if (*t == NULL)
    return EALLOC;
  for (int i = 0; i < HASH_TABLE_SIZE; (*t)[i++] = NULL) ;
  return EOK;
}

/**
 * Vyhledá symbol v hashovací tabulce. Pokud najde funkci nebo
 * konstantu konèí hledání. Promìnou vrací pouze pokud nenajde
 * funkci.
 * @param t Hashovací tabulka
 * @param key Primární klíè
 * @param key2 Sekundární klíè(mù¾e být NULL)
 * @return Nalezený symbol
 */
Symbol *stableSearch(STable t, struct tString * key, struct tString * key2)
{
  int index = hashFunction(key);
  Symbol *item = t[index];
  Symbol *ret = NULL;
  while (item != NULL) {
    if (!strcmp(item->key->str, key->str)) {
      if (item->type == SFUNCTION || item->type == SCONST)
        return item;
      if (key2 != NULL && !strcmp(((SVar *) item->data)->funct->str, key2->str))
        ret = item;
    }
    item = item->next;
  }
  return ret;
}

/**
 * Vlo¾í symbol do hashovací tabulky.
 * @param t Hashovací tabulka
 * @param item Symbol
 */
void stableInsert(STable t, Symbol * item)
{
  int index = hashFunction(item->key);
  item->next = t[index];
  t[index] = item;
}

/**
 * Sma¾e tabulku symbolù.
 * @param t Hashovací tabulka
 */
void stableFree(STable t)
{
  for (int i = 0; i < HASH_TABLE_SIZE; i++) {
    Symbol *item;;
    while (t[i] != NULL) {
      item = t[i];
      t[i] = t[i]->next;
      freeSymbol(item);
    }
  }
  free(t);
}

/**
 * Vytvoøí novì alokovaný objekt typu Symbol.
 * V pøípadì chyby alokace vrací NULL.
 * Pokud jsou data NULL vrací také NULL.
 * @param type Typ symbolu
 * @param key Klíè
 * @param data Data symbolu
 * @return Novì vytvoøený objekt
 */
Symbol *newSymbol(SymbolType type, tString * key, void *data)
{
  if (data == NULL)
    return NULL;
  tString *new = malloc(sizeof(tString));
  if (new == NULL)
    return NULL;
  if (stringCopy(new, key) == EALLOC) {
    free(new);
    return NULL;
  }
  Symbol *item = malloc(sizeof(struct Symbol));
  if (item == NULL) {
    stringFree(new);
    free(new);
    return NULL;
  }
  item->type = type;
  item->key = new;
  item->data = data;
  item->next = NULL;
  return item;
}

/**
 * Sma¾e symbol.
 * @param s Symbol
 */
void freeSymbol(Symbol * s)
{
  stringFree(s->key);
  free(s->key);
  if (s->type == SCONST)
    freeData(s->data);
  else
    free(s->data);
  free(s);
}

/**
 * Vytvoøí novì alokovaný objekt typu SVar.
 * V pøípadì chyby alokace vrací NULL.
 * @param funct Funkce ve které se promìnná nalézá
 * @param pos Poøadí promìnné ve funkci
 * @return Novì vytvoøený objekt
 */
SVar *newSVar(tString * funct, int pos)
{
  SVar *new = malloc(sizeof(SVar));
  if (new == NULL)
    return NULL;
  new->funct = funct;
  new->pos = pos;
  return new;
}

/**
 * Vytvoøí novì alokovaný objekt typu SFunct.
 * V pøípadì chyby alokace vrací NULL.
 * @param addr Adresa funkce
 * @param paramCnt Poèet parametrù funkce
 * @return Novì vytvoøený objekt
 */
SFunct *newSFunct(void *addr, int paramCnt)
{
  SFunct *new = malloc(sizeof(SFunct));
  if (new == NULL)
    return NULL;
  new->addr = addr;
  new->paramCnt = paramCnt;
  return new;
}

////////////////////////////////////////////////////////////////////////
//                      Øadící metoda (heapsort)                      //
////////////////////////////////////////////////////////////////////////

/**
 * Pomocný algoritmus, který prodlou¾í haldu o jeden prvek
 * @param pole, na kterém se tvoøí halda
 * @param zaèátek haldy
 * @param konec haldy
 */
void heapExtend(char *a, int beginning, int end)
{
  int root = beginning;
  while (root * 2 < end) {
    int child = root * 2 + 1;
    int swap = root;
    if (a[swap] < a[child])
      swap = child;
    if ((child < end) && (a[swap] < a[child + 1]))
      swap = child + 1;
    if (swap != root) {
      char temp = a[swap];
      a[swap] = a[root];
      a[root] = temp;
      root = swap;
    }
    else
      return;
  }
}

/**
 * Seøadí øetìzec znakù metodou heapsort.
 * @param count Poèet znakù v øetìzci
 * @param a Øetìzec
 */
void heapSort(int count, char *a)
{
  //prodlou¾ení haldy na celé pole
  int beginning = count / 2 - 1;
  while (beginning >= 0) {
    heapExtend(a, beginning, count - 1);
    beginning--;
  }
  //max. prvek haldy je pøemístìn na konec, zbytek pole znovu zhaldován
  int end = count - 1;
  while (end > 0) {
    char temp = a[end];
    a[end] = a[0];
    a[0] = temp;
    end--;
    heapExtend(a, 0, end);
  }
}

////////////////////////////////////////////////////////////////////////
//          Vyhledání podøetìzce (BMA)Øadící metoda (heapsort)        //
////////////////////////////////////////////////////////////////////////

/**
 * Vypoèíta skoky pro vzorek pro BMA.
 * @param s Vzorový øetìzec
 * @param charJump Tabulka pro vypoèítané skoky pro jednotlivé znaky
 */
void computeJumps(tString * s, int *charJump)
{
  for (int i = 0; i < CHAR_COUNT; i++)
    charJump[i] = s->length;
  for (int i = 0; i < s->length; i++)
    charJump[(unsigned char)s->str[i]] = s->length - i - 1;
}

/**
 * Algoritmus pro vyhledání podøetìzce v øetìzci
 * @param øetìzec, ve kterém se vyhledává
 * @param hledaný podøetìzec
 * @return pozice podøetìzce v øetìzci
 */
int find(tString * text, tString * search)
{
  int j = search->length - 1, k = j;
  if (search->length == 0)
    return 0;
  int charJump[CHAR_COUNT];
  computeJumps(search, charJump);
  while (j < text->length && k >= 0) {
    if (text->str[j] == search->str[k]) {
      j--;
      k--;
    }
    else {
      j += charJump[(unsigned char)text->str[j]];
      k = search->length - 1;
    }
  }
  return (k < 0) ? (j + 2) : (text->length + 1);
}
