/*
 * Soubor:   ial.h
 * Resitele: xbartl03, xfryzp00, xhorak34, xstane31
 * Projekt:  P�eklada� jazyka IFJ11
 * Popis:    Funkce pro pr�ci s tabulkou symbol�(hashovac� tabulka),
 *           vyhled�v�n� pod�et�zce v �et�zci(BMA) a �azen�(heapsort)
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
 * Hashovac� funkce.
 * @param s �et�zec
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
 * Inicializuje tabulku symbol�.
 * @param t Hashovac� tabulka
 * @return Chybov� k�d
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
 * Vyhled� symbol v hashovac� tabulce. Pokud najde funkci nebo
 * konstantu kon�� hled�n�. Prom�nou vrac� pouze pokud nenajde
 * funkci.
 * @param t Hashovac� tabulka
 * @param key Prim�rn� kl��
 * @param key2 Sekund�rn� kl��(m��e b�t NULL)
 * @return Nalezen� symbol
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
 * Vlo�� symbol do hashovac� tabulky.
 * @param t Hashovac� tabulka
 * @param item Symbol
 */
void stableInsert(STable t, Symbol * item)
{
  int index = hashFunction(item->key);
  item->next = t[index];
  t[index] = item;
}

/**
 * Sma�e tabulku symbol�.
 * @param t Hashovac� tabulka
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
 * Vytvo�� nov� alokovan� objekt typu Symbol.
 * V p��pad� chyby alokace vrac� NULL.
 * Pokud jsou data NULL vrac� tak� NULL.
 * @param type Typ symbolu
 * @param key Kl��
 * @param data Data symbolu
 * @return Nov� vytvo�en� objekt
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
 * Sma�e symbol.
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
 * Vytvo�� nov� alokovan� objekt typu SVar.
 * V p��pad� chyby alokace vrac� NULL.
 * @param funct Funkce ve kter� se prom�nn� nal�z�
 * @param pos Po�ad� prom�nn� ve funkci
 * @return Nov� vytvo�en� objekt
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
 * Vytvo�� nov� alokovan� objekt typu SFunct.
 * V p��pad� chyby alokace vrac� NULL.
 * @param addr Adresa funkce
 * @param paramCnt Po�et parametr� funkce
 * @return Nov� vytvo�en� objekt
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
//                      �ad�c� metoda (heapsort)                      //
////////////////////////////////////////////////////////////////////////

/**
 * Pomocn� algoritmus, kter� prodlou�� haldu o jeden prvek
 * @param pole, na kter�m se tvo�� halda
 * @param za��tek haldy
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
 * Se�ad� �et�zec znak� metodou heapsort.
 * @param count Po�et znak� v �et�zci
 * @param a �et�zec
 */
void heapSort(int count, char *a)
{
  //prodlou�en� haldy na cel� pole
  int beginning = count / 2 - 1;
  while (beginning >= 0) {
    heapExtend(a, beginning, count - 1);
    beginning--;
  }
  //max. prvek haldy je p�em�st�n na konec, zbytek pole znovu zhaldov�n
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
//          Vyhled�n� pod�et�zce (BMA)�ad�c� metoda (heapsort)        //
////////////////////////////////////////////////////////////////////////

/**
 * Vypo��ta skoky pro vzorek pro BMA.
 * @param s Vzorov� �et�zec
 * @param charJump Tabulka pro vypo��tan� skoky pro jednotliv� znaky
 */
void computeJumps(tString * s, int *charJump)
{
  for (int i = 0; i < CHAR_COUNT; i++)
    charJump[i] = s->length;
  for (int i = 0; i < s->length; i++)
    charJump[(unsigned char)s->str[i]] = s->length - i - 1;
}

/**
 * Algoritmus pro vyhled�n� pod�et�zce v �et�zci
 * @param �et�zec, ve kter�m se vyhled�v�
 * @param hledan� pod�et�zec
 * @return pozice pod�et�zce v �et�zci
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
