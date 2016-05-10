/*
 * Soubor:   data.h
 * Resitele: xbartl03, xfryzp00, xhorak34, xstane31
 * Projekt:  Pøekladaè jazyka IFJ11
 * Popis:    Deklarace funkcí pro práci s daty ulo¾enými v zásobníku
 *           + potøebné struktury
 */

/**
 * Výèet typu dat
 */
typedef enum DataType {         // POZOR, pou¾ito pøi indexování
  DNUMBER,                      /**< datový typ number(double) */
  DSTRING,                      /**< datový typ string(tString) */
  DBOOLEAN,                     /**< datový typ boolean(bool) */
  DNIL,                         /**< datový typ nil(NULL) */
  DMARK                         /**< ukazatel na instrukci */
} DataType;

/**
 * Struktura pro data
 */
typedef struct Data {
  DataType type;                /**< typ dat */
  void *data;                   /**< vlastní data */
} Data;

/**
 * Vytvoøí novì alokovaný objekt typu Data.
 * V pøípadì chyby alokace vrací NULL.
 * @param t Typ dat
 * @param d Data
 * @return Novì vytvoøený objekt
 */
Data *newData(DataType t, void *d);

/**
 * Vytvoøí kopii dat.
 * V pøípadì chyby alokace vrací NULL.
 * @param d Data
 * @return Ukazatel na zkopírovaná data
 */
Data *copyData(Data * d);

/**
 * Sma¾e data.
 * @param d Data
 */
void freeData(Data * d);

/**
 * Porovná dvoje data na rovnost.
 * Operace není definována pro typ DMARK.
 * @param a První data
 * @param b Druhá data
 * @return Výsledek porovnání
 */
_Bool isEqData(Data * a, Data * b);
