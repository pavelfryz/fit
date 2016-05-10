/*
 * Soubor:  proj4list.c
 * Datum:   12.12.2010
 * Autor:   Pavel Fr�z, xfryzp00@stud.fit.vutbr.cz
 * Projekt: �esk� �azen�, projekt �. 4 pro p�edm�t IZP
 * Popis:   Funkce pro pr�ci se seznamem
 */

#include <stdio.h>
#include <stdlib.h>
#include "proj4list.h"

/**
 * Inicializuje seznam.
 * @param list Seznam
 */
void listInit (TList *list)
{  
  list->head = NULL;
  list->tail = NULL;
  list->count=0;
}

/**
 * P�ipoj� k c�lov�mu seznamu zdrojov� seznam.
 * @param dest C�lov� seznam.
 * @param source Zdrojov� seznam.
 */
void addList(TList *dest,TList *source)
{ 
  if(dest->count==0)
    dest->head=source->head;
  dest->tail->next=source->head;
  source->head->prev=dest->tail;
  dest->tail=source->tail;
  dest->count+=source->count;
}

/**
 * Vytvo�� novou polo�ku seznamu.
 * @param data Data polo�ky.
 * @return Ukazatel na nov� vytvo�enou polo�ku.
 */
TItem *newItem(void *data)
{
  TItem *item;
  if ((item = malloc (sizeof (TItem))) == NULL)
    return NULL;
  item->data=data;
  item->next=NULL;
  item->prev=NULL;
  return item;
}

/**
 * P�id� polo�ku na za��tek seznamu.
 * @param list Seznam.
 * @param item Polo�ka.
 */
void insertFirst (TList *list, TItem *item)
{ 
  item->next = list->head;
  item->prev = NULL;
  list->head = item;
  if(list->count==0) list->tail=item;
  else item->next->prev=item;
  list->count++;
}

/**
 * P�id� polo�ku na konec seznamu.
 * @param list Seznam.
 * @param item Polo�ka.
 */
void insertLast (TList *list, TItem *item)
{
  item->next = NULL;
  item->prev = list->tail;
  list->tail = item;
  if(list->count==0) list->head=item;
  else item->prev->next=item;
  list->count++;
}

/**
 * Odstran� polo�ku ze seznamu.
 * @param list Seznam.
 * @param item Polo�ka.
 * @param freeData Funkce, kter� uvoln� data polo�ky.
 */
void deleteItem (TList *list,TItem *item,void (*freeData)(void *))
{
  TItem *this=item;

  if(this->prev==NULL)
  { // prvek na zacatku
  	deleteFirst(list,freeData);
  }
  else if(this->next==NULL)
  { // prvek na konci
  	deleteLast(list,freeData);
  }
  else
  { // prvek uprostred
  	if(freeData!=NULL)
    {
  	  freeData(this->data);
    }
  	this->next->prev=this->prev;
  	this->prev->next=this->next;
  	list->count--;
  	free(this);
  }
}

/**
 * Odstran� polo�ku ze za��tku seznamu.
 * @param list Seznam.
 * @param item Polo�ka.
 * @param freeData Funkce, kter� uvoln� data polo�ky.
 */
void deleteFirst(TList *list,void (*freeData)(void *))
{
  if(freeData!=NULL)
  {
  	freeData(list->head->data);
  }
  if(list->count==1)
  { // jeden prvek
  	free(list->head);
  	list->head=NULL;
  	list->tail=NULL;
  	list->count=0;
  }
  else
  { // vice prvku
  	TItem *this=list->head;
  	list->head=this->next;
    list->count--;
    this->next->prev=NULL;
    free(this);
  }
}

/**
 * Odstran� polo�ku z konce seznamu.
 * @param list Seznam.
 * @param item Polo�ka.
 * @param freeData Funkce, kter� uvoln� data polo�ky.
 */
void deleteLast(TList *list,void (*freeData)(void *))
{
  if(freeData!=NULL)
  {
  	freeData(list->tail->data);
  }
  if(list->count==1)
  { // jeden prvek
  	free(list->tail);
  	list->head=NULL;
  	list->tail=NULL;
  	list->count=0;
  }
  else
  { // vice prvku
  	TItem *this=list->tail;
  	list->tail=this->prev;
    list->count--;
    this->prev->next=NULL;
    free(this);
  }
}

/**
 * Prohod� data dvou polo�ek.
 * @param item1 Prvn� polo�ka.
 * @param item2 Druh� polo�ka.
 */
void swapItem(TItem *item1,TItem *item2)
{
  TItem *temp=item1->data;
  item1->data=item2->data;
  item2->data=temp;
}

/**
 * Se�ad� seznam metodou merge sort.
 * @param list Seznam.
 * @param cmpData Funkce na porovn�n� dat polo�ek.
 */
void sortList(TList *list,int (*cmpData)(const void *,const void *))
{
  TList l,r,*leftList=&l,*rightList=&r;
  unsigned int distance=1,items=list->count;

  if(list->head==NULL) return; // prazdny seznam

  while (distance<items)
  { // vzdalenost leveho a praveho seznamu je mensi nez pocet polozek
    leftList->head=list->head;
    rightList->head=leftList->head;
    listInit(list);
    while (leftList->head!=NULL)
    { // nejsme na konci seznamu
      leftList->count=0;
      rightList->count=0;
      if(items>distance)
      { // dostatek polozek do obou seznamu
        leftList->count=distance;
        items-=distance;
        for(unsigned int i=0;i<distance;i++)
        { // posun na zacatek praveho seznamu
          rightList->head=rightList->head->next;
        }
        if(items<distance) rightList->count=items;
        else rightList->count=distance;
        items-=rightList->count;
      }
      else
      { // mene polozek nez vzdalenost seznamu => pravy seznam prazdny
        leftList->count=items;
        rightList->head=NULL;
      }
      // spoj pravy a levy seznam
      merge(list,leftList,rightList,cmpData);
      // nastav levy seznam na konec praveho seznamu
      leftList->head=rightList->head;
    }
    items=list->count;
    distance*=2; 
  }
}

/**
 * Spoj� dva se�azen� seznamy do jednoho se�azen�ho seznamu.
 * @param list V�sledn� se�azen� seznam, lev� plus prav� ��st.
 * @param leftList Lev� ��st seznamu.
 * @param rightList Prav� ��st seznamu.
 * @param cmpData Funkce na porovn�n� dat polo�ek.
 */
void merge(TList *list, TList *leftList, TList *rightList,int (*cmpData)(const void *,const void *))
{
  TItem *current;
  while(leftList->count>0 || rightList->count>0)
  { // jsou polo�ky v seznamech
    if(rightList->count==0)
    { // prazdny pravy seznam
      while(leftList->count>0)
      {
        current=leftList->head;
        leftList->head=current->next;
        leftList->count--;
        insertLast(list,current);
      }
    }
    else if(leftList->count==0)
    { // prazdny levy seznam
      while(rightList->count>0)
      {
        current=rightList->head;
        rightList->head=current->next;
        rightList->count--;
        insertLast(list,current);
      }
    }
    else if(cmpData(leftList->head->data,rightList->head->data)<=0)
    { // leva polozka mensi nebo rovna
      current=leftList->head;
      leftList->head=current->next;
      leftList->count--;
      insertLast(list,current);
    }
    else
    { // prava polozka mensi
      current=rightList->head;
      rightList->head=current->next;
      rightList->count--;
      insertLast(list,current);
    }
  }
}

/**
 * Vytiskne seznam do souboru.
 * @param list Seznam.
 * @param stream Soubor.
 * @param printFunction Funkce na vytisknut� dat polo�ek.
 */
int printList(TList *list,FILE * stream,int (*printFunction)(void *,FILE *))
{
  TItem *item;

  item=list->head;
  int errcode=0;
  while(item!=NULL)
  {
    if((errcode=printFunction(item->data,stream)))
      return errcode;
    item=item->next;
  }
  return errcode;
}

/**
 * Se�ad� seznam metodou quick sort.
 * @param list Seznam.
 * @param cmpData Funkce na porovn�n� dat polo�ek.
 */
void quickSort(TList *list,int (*cmpData)(const void *,const void *))
{
  TItem *pivot=list->tail;
  TItem *left=list->head;
  TItem *right=list->tail;
  int leftPos=0,rightPos=list->count-1;
  if(list->count<=1) return;
  while(rightPos>leftPos)
  {
   for(;leftPos<rightPos;leftPos++)
   {
   	 if(cmpData(left->data,pivot->data)>0) break;
   	 left=left->next;
   }
   for(;leftPos<rightPos;rightPos--)
   {
   	 if(cmpData(right->data,pivot->data)<0) break;
   	 right=right->prev;
   }
   if(leftPos<rightPos) swapItem(left,right);
  }
  swapItem(right,pivot); // umisti pivot na spravne misto

  rightPos=list->count;  // ulozi pocet prvku puvodniho seznamu
  left=list->head;       // ulozi hlavicku puvodniho seznamu

  list->tail=right->prev; // posune ocas seznamu pred pivot
  list->count=leftPos;    // pocet prvku od zacatku po pivot
  quickSort(list,cmpData);

  list->head=right->next; // posune hlavu seznamu za pivot
  list->tail=pivot;       // vrati ocas na konec seznamu
  list->count=rightPos-leftPos-1;// pocet prvku za pivotem
  quickSort(list,cmpData);

  list->head=left;       // obnovi hlavicku puvodniho seznamu
  list->count=rightPos;  // obnovi pocet prvku puvidniho seznamu
}
/* konec proj4list.c */
