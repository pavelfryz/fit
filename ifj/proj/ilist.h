/*
 * Soubor:   ilist.h
 * Resitele: xbartl03, xfryzp00, xhorak34, xstane31
 * Projekt:  P�eklada� jazyka IFJ11
 * Popis:    Deklarace funkc� pro pr�ci se seznamem instrukc�
 *           + pot�ebn� struktury
 */

struct Instr;

/**
 * Struktura pro polo�ku seznamu instrukc�
 */
typedef struct ListItem {
  struct Instr *i;              /**< instrukce */
  struct ListItem *next;        /**< dal�� polo�ka */
} ListItem;

/**
 * Struktura pro seznam instrukc�
 */
typedef struct InstrList {
  ListItem *first;              /**< ukazatel na prvn� instrukci */
  ListItem *last;               /**< ukazatel na posledn� instrukci */
  ListItem *active;             /**< ukazatel na aktivn� instrukci */
} InstrList;

/**
 * Inicializuje seznam.
 * @param l Seznam
 */
void ilistInit(InstrList * l);

/**
 * Sma�e seznam.
 * @param l Seznam
 */
void ilistFree(InstrList * l);

/**
 * Vlo�� instrukci na konec seznamu.
 * @param l Seznam
 * @param i Instrukce
 * @return Chybov� k�d
 */
int ilistInsertLast(InstrList * l, struct Instr *i);

/**
 * Nastav� aktivitu seznamu na prvn� instrukci.
 * @param l Seznam
 */
void ilistFirst(InstrList * l);

/**
 * Nastav� aktivitu seznamu na posledn� instrukci.
 * @param l Seznam
 */
void ilistLast(InstrList * l);

/**
 * Nastav� aktivitu seznamu na dal�� instrukci.
 * @param l Seznam
 */
void ilistNext(InstrList * l);

/**
 * Nastav� aktivitu seznamu na zadanou adresu.
 * @param l Seznam
 * @param gotoInstr Adresa
 */
void ilistGoto(InstrList * l, void *gotoInstr);

/**
 * Vr�t� adresu aktivn� polo�ky seznamu.
 * @param l Seznam
 * @return Adresa polo�ky seznamu
 */
void *ilistGetPtr(InstrList * l);

/**
 * Vr�t� adresu posledn� polo�ky seznamu.
 * @param l Seznam
 * @return Adresa polo�ky seznamu
 */
void *ilistGetPtrLast(InstrList * l);

/**
 * Vr�t� adresu aktivn� instrukce.
 * @param l Seznam
 * @param i Ukazatel na vr�cenou instrukci
 */
void ilistGetInstr(InstrList * l, struct Instr **i);
