/*
 * Soubor:   exprstack.h
 * Resitele: xbartl03, xfryzp00, xhorak34, xstane31
 * Projekt:  Pøekladaè jazyka IFJ11
 * Popis:    Deklarace funkcí pro práci se zásobníkem vyu¾ívané precendenèním
 *           syntaktickým analyzátorem + potøebné struktury
 */

/**
 * Struktura pro polo¾ku zásobníku
 */
typedef struct tElem {
  int val;                      /**< hodnota */
  struct tElem *next;           /**< následující polo¾ka */
} tElem;

/**
 * Struktura pro zásobník
 */
typedef struct {
  tElem *top;                   /**< vrchol zásobníku */
} tStack;

/**
 * Inicializuje zásobník.
 * @param s Zásobník
 */
void initStack(tStack * s);

/**
 * Sma¾e zásobník.
 * @param s Zásobník
 */
void freeStack(tStack * s);

/**
 * Vlo¾í hodnotu na vrchol zásobníku.
 * @param s Zásobník
 * @param val Hodnota
 * @return Chybový kód
 */
int push(tStack * s, int val);

/**
 * Odebere vrchol zásobníku a vrátí hodnotu.
 * @param s Zásobník
 * @return Hodnotu na vrcholu
 */
int pop(tStack * s);

/**
 * Vrátí nejvrchnìj¹í terminální symbol.
 * @param s Zásobník
 * @return Hodnotu nejvy¹¹ího terminálu
 */
int topTerm(tStack * s);

/**
 * Vlo¾í øídící znak CLT('<') za nejvy¹¹í terminál.
 * @param s Zásobník
 * @return Chybový kód
 */
int pushCLT(tStack * s);
