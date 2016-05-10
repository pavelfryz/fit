/*
 * Soubor:   data.h
 * Resitele: xbartl03, xfryzp00, xhorak34, xstane31
 * Projekt:  P�eklada� jazyka IFJ11
 * Popis:    Deklarace funkc� pro pr�ci s daty ulo�en�mi v z�sobn�ku
 *           + pot�ebn� struktury
 */

/**
 * V��et typu dat
 */
typedef enum DataType {         // POZOR, pou�ito p�i indexov�n�
  DNUMBER,                      /**< datov� typ number(double) */
  DSTRING,                      /**< datov� typ string(tString) */
  DBOOLEAN,                     /**< datov� typ boolean(bool) */
  DNIL,                         /**< datov� typ nil(NULL) */
  DMARK                         /**< ukazatel na instrukci */
} DataType;

/**
 * Struktura pro data
 */
typedef struct Data {
  DataType type;                /**< typ dat */
  void *data;                   /**< vlastn� data */
} Data;

/**
 * Vytvo�� nov� alokovan� objekt typu Data.
 * V p��pad� chyby alokace vrac� NULL.
 * @param t Typ dat
 * @param d Data
 * @return Nov� vytvo�en� objekt
 */
Data *newData(DataType t, void *d);

/**
 * Vytvo�� kopii dat.
 * V p��pad� chyby alokace vrac� NULL.
 * @param d Data
 * @return Ukazatel na zkop�rovan� data
 */
Data *copyData(Data * d);

/**
 * Sma�e data.
 * @param d Data
 */
void freeData(Data * d);

/**
 * Porovn� dvoje data na rovnost.
 * Operace nen� definov�na pro typ DMARK.
 * @param a Prvn� data
 * @param b Druh� data
 * @return V�sledek porovn�n�
 */
_Bool isEqData(Data * a, Data * b);
