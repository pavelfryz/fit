/*
 * Soubor:   datastack.h
 * Resitele: xbartl03, xfryzp00, xhorak34, xstane31
 * Projekt:  P�eklada� jazyka IFJ11
 * Popis:    Deklarace funkc� pro pr�ci se z�sobn�kem vyu��van� interpretem
 *           + pot�ebn� struktury
 */

struct Data;

/**
 * Struktura pro polo�ku z�sobn�ku
 */
typedef struct Node {
  struct Data *data;            /**< data */
  struct Node *next;            /**< n�sleduj�c� polo�ka */
  struct Node *prev;            /**< p�edchoz� polo�ka */
} Node;

/**
 * Struktura pro zna�ku
 */
typedef struct Mark {
  Node *mark;                   /**< ozna�en� vrchol */
  struct Mark *nextMark;        /**< dal�� zna�ka */
} Mark;

/**
 * Struktura pro z�sobn�k
 */
typedef struct DataStack {
  Node *top;                    /**< vrchol z�sobn�ku */
  Mark *mark;                   /**< seznam ozna�en�ch vrchol� */
  Mark *active;                 /**< seznam aktivn� ozna�en�ch vrchol� */
} DataStack;

/**
 * Inicializuje z�sobn�k.
 * @param s Z�sobn�k
 */
void dsInit(DataStack * s);

/**
 * Sma�e z�sobn�k.
 * @param s Z�sobn�k
 */
void dsClear(DataStack * s);

/**
 * Sma�e z�sobn�k po posledn� aktivn� zna�ku.
 * Ozna�en� m�sto z�st�v� na vrcholu z�sobn�ku.
 * @param s Z�sobn�k
 */
void dsClearToMark(DataStack * s);

/**
 * Vlo�� data na vrchol z�sobn�ku.
 * @param s Z�sobn�k
 * @param d Data
 * @return Chybov� k�d
 */
int dsPush(DataStack * s, struct Data *d);

/**
 * Odebere vrchol z�sobn�ku a vr�t� data.
 * Sma�e p��padnou aktivn� zna�ku.
 * @param s Z�sobn�k
 * @param d Ukazatel na vr�cen� data
 */
void dsPop(DataStack * s, struct Data **d);

/**
 * Aktivuje posledn� zna�ku.
 * @param s Z�sobn�k
 */
void dsActivateMark(DataStack * s);

/**
 * Ozna�� sou�asn� vrchol z�sobn�ku.
 * @param s Z�sobn�k
 * @return Chybov� k�d
 */
int dsMark(DataStack * s);

/**
 * Vr�t� data z posledn� aktivovan� pozice + posun.
 * @param s Z�sobn�k
 * @param pos Posun, pouze ve smeru na dalsi prvky(0,1,..)
 * @param d Ukazatel na vr�cen� data
 * @return Chybov� k�d
 */
int dsAt(DataStack * s, int pos, struct Data **d);

/**
 * P�ep�e data posledn� aktivovan� pozice + posun.
 * @param s Z�sobn�k
 * @param pos Posun, pouze ve smeru na dalsi prvky(0,1,..)
 * @param d Nov� data
 */
void dsActualizeAt(DataStack * s, int pos, struct Data *d);
