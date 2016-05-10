/*
 * Soubor:   stringoperation.h
 * Resitele: xbartl03, xfryzp00, xhorak34, xstane31
 * Projekt:  P�eklada� jazyka IFJ11
 * Popis:    Deklarace funkc� pro pr�ci s �et�zci
 *           + pot�ebn� struktury
 */

/**
 * Struktura pro �et�zec
 */
typedef struct tString {
  char *str;                    /**< �et�zec */
  int allocated;                /**< velikost alokovan�ho m�sta */
  int length;                   /**< d�lka �et�zce */
} tString;

/**
 * Inicializuje �et�zec.
 * @param s �et�zec
 * @return Chybov� k�d
 */
int stringInit(tString * s);

/**
 * Sma�e �et�zec.
 * @param s �et�zec
 */
void stringFree(tString * s);

/**
 * Sma�e obsah �et�zece.
 * @param s �et�zec
 */
void stringClear(tString * s);

/**
 * Vytvo�� kopii �et�zce.
 * @param dest C�lov� �et�zec
 * @param source Zdrojov� �et�zec
 * @return Chybov� k�d
 */
int stringCopy(tString * dest, tString * source);

/**
 * Provede konkatenaci c�lov�ho a zdrojov�ho �et�zce.
 * @param dest C�lov� �et�zec
 * @param source Zdrojov� �et�zec
 * @return Chybov� k�d
 */
int stringAppend(tString * dest, tString * source);

/**
 * O��zne �et�zec podle zadan�ch index�.
 * @param s �et�zec
 * @param first Index za��tku pod�et�zce
 * @param last Index konce pod�et�zce
 */
void stringSubstr(tString * s, int first, int last);

/**
 * P�id� znak na konec �et�zce.
 * @param s �et�zec
 * @param c Znak
 * @return Chybov� k�d
 */
int addChar(tString * s, char c);

/**
 * P�evede �et�zec na double.
 * @param s �et�zec
 * @param pNum P�eveden� ��slo
 * @return Chybov� k�d
 */
int stringToDouble(tString * s, double *pNum);
