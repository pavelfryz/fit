/*
 * Soubor:   ial.h
 * Resitele: xbartl03, xfryzp00, xhorak34, xstane31
 * Projekt:  Pøekladaè jazyka IFJ11
 * Popis:    Deklarace funkcí pro práci s tabulkou symbolù(hashovací tabulka),
 *           vyhledávání podøetìzce v øetìzci(BMA) a øazení(heapsort)
 *           + potøebné struktury
 */

struct tString;

/**
 *  Výèet typù pro tabulku symbolù
 */
typedef enum SymbolType {
  SFUNCTION,                    /**< Funkce */
  SVARIABLE,                    /**< Promìnná */
  SCONST                        /**< Literál */
} SymbolType;

/**
 * Struktura pro symboly
 */
typedef struct Symbol {
  SymbolType type;              /**< Typ symbolu */
  struct tString *key;          /**< Klíè symbolu */
  void *data;                   /**< Data */
  struct Symbol *next;
} Symbol;

/**
 * Struktura pro data v pøípadì promìnné
 */
typedef struct SVar {
  struct tString *funct;        /**< Název funkce ve které se promìnná vyskytuje */
  int pos;                      /**< Pozice promìnné */
} SVar;

/**
 * Struktura pro data v pøípadì funkce
 */
typedef struct SFunct {
  void *addr;                   /**< Adresa funkce */
  int paramCnt;                 /**< Poèet parametrù */
} SFunct;

/**
 * Definování typu tabulky symbolù
 */
typedef Symbol **STable;

////////////////////////////////////////////////////////////////////////
//                Tabulka symbolu (hashovaci tabulka)                 //
////////////////////////////////////////////////////////////////////////

/**
 * Inicializuje tabulku symbolù.
 * @param t Hashovací tabulka
 * @return Chybový kód
 */
int stableInit(STable * t);

/**
 * Vyhledá symbol v hashovací tabulce. Pokud najde funkci nebo
 * konstantu konèí hledání. Promìnou vrací pouze pokud nenajde
 * funkci.
 * @param t Hashovací tabulka
 * @param key Primární klíè
 * @param key2 Sekundární klíè(mù¾e být NULL)
 * @return Nalezený symbol
 */
Symbol *stableSearch(STable t, struct tString *key, struct tString *key2);

/**
 * Vlo¾í symbol do hashovací tabulky.
 * @param t Hashovací tabulka
 * @param item Symbol
 */
void stableInsert(STable t, Symbol * item);

/**
 * Sma¾e tabulku symbolù.
 * @param t Hashovací tabulka
 */
void stableFree(STable t);

/**
 * Vytvoøí novì alokovaný objekt typu Symbol.
 * V pøípadì chyby alokace vrací NULL.
 * Pokud jsou data NULL vrací také NULL.
 * @param type Typ symbolu
 * @param key Klíè
 * @param data Data symbolu
 * @return Novì vytvoøený objekt
 */
Symbol *newSymbol(SymbolType type, struct tString *key, void *data);

/**
 * Sma¾e symbol.
 * @param s Symbol
 */
void freeSymbol(Symbol * s);

/**
 * Vytvoøí novì alokovaný objekt typu SVar.
 * V pøípadì chyby alokace vrací NULL.
 * @param funct Funkce ve které se promìnná nalézá
 * @param pos Poøadí promìnné ve funkci
 * @return Novì vytvoøený objekt
 */
SVar *newSVar(struct tString *funct, int pos);

/**
 * Vytvoøí novì alokovaný objekt typu SFunct.
 * V pøípadì chyby alokace vrací NULL.
 * @param addr Adresa funkce
 * @param paramCnt Poèet parametrù funkce
 * @return Novì vytvoøený objekt
 */
SFunct *newSFunct(void *addr, int paramCnt);

////////////////////////////////////////////////////////////////////////
//                      Øadící metoda (heapsort)                      //
////////////////////////////////////////////////////////////////////////

/**
 * Seøadí øetìzec znakù metodou heapsort.
 * @param count Poèet znakù v øetìzci
 * @param a Øetìzec
 */
void heapSort(int count, char *a);

////////////////////////////////////////////////////////////////////////
//          Vyhledání podøetìzce (BMA)Øadící metoda (heapsort)        //
////////////////////////////////////////////////////////////////////////

/**
 * Vyhledá podøetìzce v øetìzci (BMA).
 * @param text Prohledávaný øetìzec
 * @param search Hledaný podøetìzec
 * @return Pozice podøetìzce v øetìzci
 */
int find(struct tString *text, struct tString *search);
