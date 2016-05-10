/*
 * Soubor:  proj4list.h
 * Datum:   12.12.2010
 * Autor:   Pavel Frýz, xfryzp00@stud.fit.vutbr.cz
 * Projekt: Èeské øazení, projekt è. 4 pro pøedmìt IZP
 * Popis:   Hlavièkovy soubor k souboru proj4list.c
 */

/**
 * Struktura pro polo¾ku seznamu.
 */
typedef struct titem TItem;
struct titem
{
  void *data;  // ukazatel na data polo¾ky
  TItem *next; // ukazatel na dal¹í polo¾ku
  TItem *prev; // ukazatel na pøedchozí polo¾ku
};

/**
 * Struktura pro seznam.
 */
typedef struct {
  TItem *head;       // ukazatel na první polo¾ku seznamu
  TItem *tail;       // ukazatel na poslední polo¾ku seznamu
  unsigned int count;// poèet polo¾ek seznamu
} TList;

/**
 * Inicializuje seznam.
 * @param list Seznam
 */
void listInit (TList *list);

/**
 * Pøipojí k cílovému seznamu zdrojový seznam.
 * @param dest Cílový seznam.
 * @param source Zdrojový seznam.
 */
void addList(TList *dest,TList *source);

/**
 * Vytvoøí novou polo¾ku seznamu.
 * @param data Data polo¾ky.
 * @return Ukazatel na novì vytvoøenou polo¾ku.
 */
TItem *newItem(void *data);

/**
 * Pøidá polo¾ku na zaèátek seznamu.
 * @param list Seznam.
 * @param item Polo¾ka.
 */
void insertFirst (TList *list, TItem *item);

/**
 * Pøidá polo¾ku na konec seznamu.
 * @param list Seznam.
 * @param item Polo¾ka.
 */
void insertLast (TList *list, TItem *item);

/**
 * Odstraní polo¾ku ze seznamu.
 * @param list Seznam.
 * @param item Polo¾ka.
 * @param freeData Funkce, která uvolní data polo¾ky.
 */
void deleteItem (TList *list,TItem *item,void (*freeData)(void *));

/**
 * Odstraní polo¾ku ze zaèátku seznamu.
 * @param list Seznam.
 * @param item Polo¾ka.
 * @param freeData Funkce, která uvolní data polo¾ky.
 */
void deleteFirst(TList *list,void (*freeData)(void *));

/**
 * Odstraní polo¾ku z konce seznamu.
 * @param list Seznam.
 * @param item Polo¾ka.
 * @param freeData Funkce, která uvolní data polo¾ky.
 */
void deleteLast(TList *list,void (*freeData)(void *));

/**
 * Prohodí data dvou polo¾ek.
 * @param item1 První polo¾ka.
 * @param item2 Druhá polo¾ka.
 */
void swapItem(TItem *item1,TItem *item2);

/**
 * Seøadí seznam metodou merge sort.
 * @param list Seznam.
 * @param cmpData Funkce na porovnání dat polo¾ek.
 */
void sortList(TList *list,int (*cmpData)(const void *,const void *));

/**
 * Spojí dva seøazené seznamy do jednoho seøazeného seznamu.
 * @param list Výsledný seøazený seznam, levá plus pravá èást.
 * @param leftList Levá èást seznamu.
 * @param rightList Pravá èást seznamu.
 * @param cmpData Funkce na porovnání dat polo¾ek.
 */
void merge(TList *list, TList *leftList, TList *rightList,int (*cmpData)(const void *,const void *));

/**
 * Seøadí seznam metodou quick sort.
 * @param list Seznam.
 * @param cmpData Funkce na porovnání dat polo¾ek.
 */
void quickSort(TList *list,int (*cmpData)(const void *,const void *));

/**
 * Vytiskne seznam do souboru.
 * @param list Seznam.
 * @param stream Soubor.
 * @param printFunction Funkce na vytisknutí dat polo¾ek.
 */
int printList(TList *list,FILE * stream,int (*printFunction)(void *,FILE *));

/* konec proj4list.h */
