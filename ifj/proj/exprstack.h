/*
 * Soubor:   exprstack.h
 * Resitele: xbartl03, xfryzp00, xhorak34, xstane31
 * Projekt:  P�eklada� jazyka IFJ11
 * Popis:    Deklarace funkc� pro pr�ci se z�sobn�kem vyu��van� precenden�n�m
 *           syntaktick�m analyz�torem + pot�ebn� struktury
 */

/**
 * Struktura pro polo�ku z�sobn�ku
 */
typedef struct tElem {
  int val;                      /**< hodnota */
  struct tElem *next;           /**< n�sleduj�c� polo�ka */
} tElem;

/**
 * Struktura pro z�sobn�k
 */
typedef struct {
  tElem *top;                   /**< vrchol z�sobn�ku */
} tStack;

/**
 * Inicializuje z�sobn�k.
 * @param s Z�sobn�k
 */
void initStack(tStack * s);

/**
 * Sma�e z�sobn�k.
 * @param s Z�sobn�k
 */
void freeStack(tStack * s);

/**
 * Vlo�� hodnotu na vrchol z�sobn�ku.
 * @param s Z�sobn�k
 * @param val Hodnota
 * @return Chybov� k�d
 */
int push(tStack * s, int val);

/**
 * Odebere vrchol z�sobn�ku a vr�t� hodnotu.
 * @param s Z�sobn�k
 * @return Hodnotu na vrcholu
 */
int pop(tStack * s);

/**
 * Vr�t� nejvrchn�j�� termin�ln� symbol.
 * @param s Z�sobn�k
 * @return Hodnotu nejvy���ho termin�lu
 */
int topTerm(tStack * s);

/**
 * Vlo�� ��d�c� znak CLT('<') za nejvy��� termin�l.
 * @param s Z�sobn�k
 * @return Chybov� k�d
 */
int pushCLT(tStack * s);
