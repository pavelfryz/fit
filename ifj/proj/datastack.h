/*
 * Soubor:   datastack.h
 * Resitele: xbartl03, xfryzp00, xhorak34, xstane31
 * Projekt:  Pøekladaè jazyka IFJ11
 * Popis:    Deklarace funkcí pro práci se zásobníkem vyu¾ívané interpretem
 *           + potøebné struktury
 */

struct Data;

/**
 * Struktura pro polo¾ku zásobníku
 */
typedef struct Node {
  struct Data *data;            /**< data */
  struct Node *next;            /**< následující polo¾ka */
  struct Node *prev;            /**< pøedchozí polo¾ka */
} Node;

/**
 * Struktura pro znaèku
 */
typedef struct Mark {
  Node *mark;                   /**< oznaèený vrchol */
  struct Mark *nextMark;        /**< dal¹í znaèka */
} Mark;

/**
 * Struktura pro zásobník
 */
typedef struct DataStack {
  Node *top;                    /**< vrchol zásobníku */
  Mark *mark;                   /**< seznam oznaèených vrcholù */
  Mark *active;                 /**< seznam aktivnì oznaèených vrcholù */
} DataStack;

/**
 * Inicializuje zásobník.
 * @param s Zásobník
 */
void dsInit(DataStack * s);

/**
 * Sma¾e zásobník.
 * @param s Zásobník
 */
void dsClear(DataStack * s);

/**
 * Sma¾e zásobník po poslední aktivní znaèku.
 * Oznaèené místo zùstává na vrcholu zásobníku.
 * @param s Zásobník
 */
void dsClearToMark(DataStack * s);

/**
 * Vlo¾í data na vrchol zásobníku.
 * @param s Zásobník
 * @param d Data
 * @return Chybový kód
 */
int dsPush(DataStack * s, struct Data *d);

/**
 * Odebere vrchol zásobníku a vrátí data.
 * Sma¾e pøípadnou aktivní znaèku.
 * @param s Zásobník
 * @param d Ukazatel na vrácená data
 */
void dsPop(DataStack * s, struct Data **d);

/**
 * Aktivuje poslední znaèku.
 * @param s Zásobník
 */
void dsActivateMark(DataStack * s);

/**
 * Oznaèí souèasný vrchol zásobníku.
 * @param s Zásobník
 * @return Chybový kód
 */
int dsMark(DataStack * s);

/**
 * Vrátí data z poslední aktivované pozice + posun.
 * @param s Zásobník
 * @param pos Posun, pouze ve smeru na dalsi prvky(0,1,..)
 * @param d Ukazatel na vrácená data
 * @return Chybový kód
 */
int dsAt(DataStack * s, int pos, struct Data **d);

/**
 * Pøepí¹e data poslední aktivované pozice + posun.
 * @param s Zásobník
 * @param pos Posun, pouze ve smeru na dalsi prvky(0,1,..)
 * @param d Nová data
 */
void dsActualizeAt(DataStack * s, int pos, struct Data *d);
