/*
 * Soubor:   stringoperation.h
 * Resitele: xbartl03, xfryzp00, xhorak34, xstane31
 * Projekt:  Pøekladaè jazyka IFJ11
 * Popis:    Deklarace funkcí pro práci s øetìzci
 *           + potøebné struktury
 */

/**
 * Struktura pro øetìzec
 */
typedef struct tString {
  char *str;                    /**< øetìzec */
  int allocated;                /**< velikost alokovaného místa */
  int length;                   /**< délka øetìzce */
} tString;

/**
 * Inicializuje øetìzec.
 * @param s Øetìzec
 * @return Chybový kód
 */
int stringInit(tString * s);

/**
 * Sma¾e øetìzec.
 * @param s Øetìzec
 */
void stringFree(tString * s);

/**
 * Sma¾e obsah øetìzece.
 * @param s Øetìzec
 */
void stringClear(tString * s);

/**
 * Vytvoøí kopii øetìzce.
 * @param dest Cílový øetìzec
 * @param source Zdrojový øetìzec
 * @return Chybový kód
 */
int stringCopy(tString * dest, tString * source);

/**
 * Provede konkatenaci cílového a zdrojového øetìzce.
 * @param dest Cílový øetìzec
 * @param source Zdrojový øetìzec
 * @return Chybový kód
 */
int stringAppend(tString * dest, tString * source);

/**
 * Oøízne øetìzec podle zadaných indexù.
 * @param s Øetìzec
 * @param first Index zaèátku podøetìzce
 * @param last Index konce podøetìzce
 */
void stringSubstr(tString * s, int first, int last);

/**
 * Pøidá znak na konec øetìzce.
 * @param s Øetìzec
 * @param c Znak
 * @return Chybový kód
 */
int addChar(tString * s, char c);

/**
 * Pøevede øetìzec na double.
 * @param s Øetìzec
 * @param pNum Pøevedené èíslo
 * @return Chybový kód
 */
int stringToDouble(tString * s, double *pNum);
