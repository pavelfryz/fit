/*
 * Soubor:   ilist.h
 * Resitele: xbartl03, xfryzp00, xhorak34, xstane31
 * Projekt:  Pøekladaè jazyka IFJ11
 * Popis:    Deklarace funkcí pro práci se seznamem instrukcí
 *           + potøebné struktury
 */

struct Instr;

/**
 * Struktura pro polo¾ku seznamu instrukcí
 */
typedef struct ListItem {
  struct Instr *i;              /**< instrukce */
  struct ListItem *next;        /**< dal¹í polo¾ka */
} ListItem;

/**
 * Struktura pro seznam instrukcí
 */
typedef struct InstrList {
  ListItem *first;              /**< ukazatel na první instrukci */
  ListItem *last;               /**< ukazatel na poslední instrukci */
  ListItem *active;             /**< ukazatel na aktivní instrukci */
} InstrList;

/**
 * Inicializuje seznam.
 * @param l Seznam
 */
void ilistInit(InstrList * l);

/**
 * Sma¾e seznam.
 * @param l Seznam
 */
void ilistFree(InstrList * l);

/**
 * Vlo¾í instrukci na konec seznamu.
 * @param l Seznam
 * @param i Instrukce
 * @return Chybový kód
 */
int ilistInsertLast(InstrList * l, struct Instr *i);

/**
 * Nastaví aktivitu seznamu na první instrukci.
 * @param l Seznam
 */
void ilistFirst(InstrList * l);

/**
 * Nastaví aktivitu seznamu na poslední instrukci.
 * @param l Seznam
 */
void ilistLast(InstrList * l);

/**
 * Nastaví aktivitu seznamu na dal¹í instrukci.
 * @param l Seznam
 */
void ilistNext(InstrList * l);

/**
 * Nastaví aktivitu seznamu na zadanou adresu.
 * @param l Seznam
 * @param gotoInstr Adresa
 */
void ilistGoto(InstrList * l, void *gotoInstr);

/**
 * Vrátí adresu aktivní polo¾ky seznamu.
 * @param l Seznam
 * @return Adresa polo¾ky seznamu
 */
void *ilistGetPtr(InstrList * l);

/**
 * Vrátí adresu poslední polo¾ky seznamu.
 * @param l Seznam
 * @return Adresa polo¾ky seznamu
 */
void *ilistGetPtrLast(InstrList * l);

/**
 * Vrátí adresu aktivní instrukce.
 * @param l Seznam
 * @param i Ukazatel na vrácenou instrukci
 */
void ilistGetInstr(InstrList * l, struct Instr **i);
