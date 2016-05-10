/*
 * Soubor:  proj4list.h
 * Datum:   12.12.2010
 * Autor:   Pavel Fr�z, xfryzp00@stud.fit.vutbr.cz
 * Projekt: �esk� �azen�, projekt �. 4 pro p�edm�t IZP
 * Popis:   Hlavi�kovy soubor k souboru proj4list.c
 */

/**
 * Struktura pro polo�ku seznamu.
 */
typedef struct titem TItem;
struct titem
{
  void *data;  // ukazatel na data polo�ky
  TItem *next; // ukazatel na dal�� polo�ku
  TItem *prev; // ukazatel na p�edchoz� polo�ku
};

/**
 * Struktura pro seznam.
 */
typedef struct {
  TItem *head;       // ukazatel na prvn� polo�ku seznamu
  TItem *tail;       // ukazatel na posledn� polo�ku seznamu
  unsigned int count;// po�et polo�ek seznamu
} TList;

/**
 * Inicializuje seznam.
 * @param list Seznam
 */
void listInit (TList *list);

/**
 * P�ipoj� k c�lov�mu seznamu zdrojov� seznam.
 * @param dest C�lov� seznam.
 * @param source Zdrojov� seznam.
 */
void addList(TList *dest,TList *source);

/**
 * Vytvo�� novou polo�ku seznamu.
 * @param data Data polo�ky.
 * @return Ukazatel na nov� vytvo�enou polo�ku.
 */
TItem *newItem(void *data);

/**
 * P�id� polo�ku na za��tek seznamu.
 * @param list Seznam.
 * @param item Polo�ka.
 */
void insertFirst (TList *list, TItem *item);

/**
 * P�id� polo�ku na konec seznamu.
 * @param list Seznam.
 * @param item Polo�ka.
 */
void insertLast (TList *list, TItem *item);

/**
 * Odstran� polo�ku ze seznamu.
 * @param list Seznam.
 * @param item Polo�ka.
 * @param freeData Funkce, kter� uvoln� data polo�ky.
 */
void deleteItem (TList *list,TItem *item,void (*freeData)(void *));

/**
 * Odstran� polo�ku ze za��tku seznamu.
 * @param list Seznam.
 * @param item Polo�ka.
 * @param freeData Funkce, kter� uvoln� data polo�ky.
 */
void deleteFirst(TList *list,void (*freeData)(void *));

/**
 * Odstran� polo�ku z konce seznamu.
 * @param list Seznam.
 * @param item Polo�ka.
 * @param freeData Funkce, kter� uvoln� data polo�ky.
 */
void deleteLast(TList *list,void (*freeData)(void *));

/**
 * Prohod� data dvou polo�ek.
 * @param item1 Prvn� polo�ka.
 * @param item2 Druh� polo�ka.
 */
void swapItem(TItem *item1,TItem *item2);

/**
 * Se�ad� seznam metodou merge sort.
 * @param list Seznam.
 * @param cmpData Funkce na porovn�n� dat polo�ek.
 */
void sortList(TList *list,int (*cmpData)(const void *,const void *));

/**
 * Spoj� dva se�azen� seznamy do jednoho se�azen�ho seznamu.
 * @param list V�sledn� se�azen� seznam, lev� plus prav� ��st.
 * @param leftList Lev� ��st seznamu.
 * @param rightList Prav� ��st seznamu.
 * @param cmpData Funkce na porovn�n� dat polo�ek.
 */
void merge(TList *list, TList *leftList, TList *rightList,int (*cmpData)(const void *,const void *));

/**
 * Se�ad� seznam metodou quick sort.
 * @param list Seznam.
 * @param cmpData Funkce na porovn�n� dat polo�ek.
 */
void quickSort(TList *list,int (*cmpData)(const void *,const void *));

/**
 * Vytiskne seznam do souboru.
 * @param list Seznam.
 * @param stream Soubor.
 * @param printFunction Funkce na vytisknut� dat polo�ek.
 */
int printList(TList *list,FILE * stream,int (*printFunction)(void *,FILE *));

/* konec proj4list.h */
