/*
 * Soubor:   ial.h
 * Resitele: xbartl03, xfryzp00, xhorak34, xstane31
 * Projekt:  P�eklada� jazyka IFJ11
 * Popis:    Deklarace funkc� pro pr�ci s tabulkou symbol�(hashovac� tabulka),
 *           vyhled�v�n� pod�et�zce v �et�zci(BMA) a �azen�(heapsort)
 *           + pot�ebn� struktury
 */

struct tString;

/**
 *  V��et typ� pro tabulku symbol�
 */
typedef enum SymbolType {
  SFUNCTION,                    /**< Funkce */
  SVARIABLE,                    /**< Prom�nn� */
  SCONST                        /**< Liter�l */
} SymbolType;

/**
 * Struktura pro symboly
 */
typedef struct Symbol {
  SymbolType type;              /**< Typ symbolu */
  struct tString *key;          /**< Kl�� symbolu */
  void *data;                   /**< Data */
  struct Symbol *next;
} Symbol;

/**
 * Struktura pro data v p��pad� prom�nn�
 */
typedef struct SVar {
  struct tString *funct;        /**< N�zev funkce ve kter� se prom�nn� vyskytuje */
  int pos;                      /**< Pozice prom�nn� */
} SVar;

/**
 * Struktura pro data v p��pad� funkce
 */
typedef struct SFunct {
  void *addr;                   /**< Adresa funkce */
  int paramCnt;                 /**< Po�et parametr� */
} SFunct;

/**
 * Definov�n� typu tabulky symbol�
 */
typedef Symbol **STable;

////////////////////////////////////////////////////////////////////////
//                Tabulka symbolu (hashovaci tabulka)                 //
////////////////////////////////////////////////////////////////////////

/**
 * Inicializuje tabulku symbol�.
 * @param t Hashovac� tabulka
 * @return Chybov� k�d
 */
int stableInit(STable * t);

/**
 * Vyhled� symbol v hashovac� tabulce. Pokud najde funkci nebo
 * konstantu kon�� hled�n�. Prom�nou vrac� pouze pokud nenajde
 * funkci.
 * @param t Hashovac� tabulka
 * @param key Prim�rn� kl��
 * @param key2 Sekund�rn� kl��(m��e b�t NULL)
 * @return Nalezen� symbol
 */
Symbol *stableSearch(STable t, struct tString *key, struct tString *key2);

/**
 * Vlo�� symbol do hashovac� tabulky.
 * @param t Hashovac� tabulka
 * @param item Symbol
 */
void stableInsert(STable t, Symbol * item);

/**
 * Sma�e tabulku symbol�.
 * @param t Hashovac� tabulka
 */
void stableFree(STable t);

/**
 * Vytvo�� nov� alokovan� objekt typu Symbol.
 * V p��pad� chyby alokace vrac� NULL.
 * Pokud jsou data NULL vrac� tak� NULL.
 * @param type Typ symbolu
 * @param key Kl��
 * @param data Data symbolu
 * @return Nov� vytvo�en� objekt
 */
Symbol *newSymbol(SymbolType type, struct tString *key, void *data);

/**
 * Sma�e symbol.
 * @param s Symbol
 */
void freeSymbol(Symbol * s);

/**
 * Vytvo�� nov� alokovan� objekt typu SVar.
 * V p��pad� chyby alokace vrac� NULL.
 * @param funct Funkce ve kter� se prom�nn� nal�z�
 * @param pos Po�ad� prom�nn� ve funkci
 * @return Nov� vytvo�en� objekt
 */
SVar *newSVar(struct tString *funct, int pos);

/**
 * Vytvo�� nov� alokovan� objekt typu SFunct.
 * V p��pad� chyby alokace vrac� NULL.
 * @param addr Adresa funkce
 * @param paramCnt Po�et parametr� funkce
 * @return Nov� vytvo�en� objekt
 */
SFunct *newSFunct(void *addr, int paramCnt);

////////////////////////////////////////////////////////////////////////
//                      �ad�c� metoda (heapsort)                      //
////////////////////////////////////////////////////////////////////////

/**
 * Se�ad� �et�zec znak� metodou heapsort.
 * @param count Po�et znak� v �et�zci
 * @param a �et�zec
 */
void heapSort(int count, char *a);

////////////////////////////////////////////////////////////////////////
//          Vyhled�n� pod�et�zce (BMA)�ad�c� metoda (heapsort)        //
////////////////////////////////////////////////////////////////////////

/**
 * Vyhled� pod�et�zce v �et�zci (BMA).
 * @param text Prohled�van� �et�zec
 * @param search Hledan� pod�et�zec
 * @return Pozice pod�et�zce v �et�zci
 */
int find(struct tString *text, struct tString *search);
