
/* c206.c **********************************************************}
{* Téma: Dvousmìrnì vázaný lineární seznam
**
**                   Návrh a referenèní implementace: Bohuslav Køena, øíjen 2001
**                            Pøepracované do jazyka C: Martin Tuèek, øíjen 2004
**                                            Úpravy: Bohuslav Køena, øíjen 2010
**
** Implementujte abstraktní datový typ dvousmìrnì vázaný lineární seznam.
** U¾iteèným obsahem prvku seznamu je hodnota typu int.
** Seznam bude jako datová abstrakce reprezentován promìnnou
** typu tDLList (DL znamená Double-Linked a slou¾í pro odli¹ení
** jmen konstant, typù a funkcí od jmen u jednosmìrnì vázaného lineárního
** seznamu). Definici konstant a typù naleznete v hlavièkovém souboru c206.h.
**
** Va¹ím úkolem je implementovat následující operace, které spolu
** s vý¹e uvedenou datovou èástí abstrakce tvoøí abstraktní datový typ
** obousmìrnì vázaný lineární seznam:
**
**      DLInitList ...... inicializace seznamu pøed prvním pou¾itím,
**      DLDisposeList ... zru¹ení v¹ech prvkù seznamu,
**      DLInsertFirst ... vlo¾ení prvku na zaèátek seznamu,
**      DLInsertLast .... vlo¾ení prvku na konec seznamu,
**      DLFirst ......... nastavení aktivity na první prvek,
**      DLLast .......... nastavení aktivity na poslední prvek,
**      DLCopyFirst ..... vrací hodnotu prvního prvku,
**      DLCopyLast ...... vrací hodnotu posledního prvku,
**      DLDeleteFirst ... zru¹í první prvek seznamu,
**      DLDeleteLast .... zru¹í poslední prvek seznamu,
**      DLPostDelete .... ru¹í prvek za aktivním prvkem,
**      DLPreDelete ..... ru¹í prvek pøed aktivním prvkem,
**      DLPostInsert .... vlo¾í nový prvek za aktivní prvek seznamu,
**      DLPreInsert ..... vlo¾í nový prvek pøed aktivní prvek seznamu,
**      DLCopy .......... vrací hodnotu aktivního prvku,
**      DLActualize ..... pøepí¹e obsah aktivního prvku novou hodnotou,
**      DLSucc .......... posune aktivitu na dal¹í prvek seznamu,
**      DLPred .......... posune aktivitu na pøedchozí prvek seznamu,
**      DLActive ........ zji¹»uje aktivitu seznamu.
**
** Pøi implementaci jednotlivých funkcí nevolejte ¾ádnou z funkcí
** implementovaných v rámci tohoto pøíkladu, není-li u funkce
** explicitnì uvedeno nìco jiného.
**
** Nemusíte o¹etøovat situaci, kdy místo legálního ukazatele na seznam
** pøedá nìkdo jako parametr hodnotu NULL.
**
** Svou implementaci vhodnì komentujte!
**
** Terminologická poznámka: Jazyk C nepou¾ívá pojem procedura.
** Proto zde pou¾íváme pojem funkce i pro operace, které by byly
** v algoritmickém jazyce Pascalovského typu implemenovány jako
** procedury (v jazyce C procedurám odpovídají funkce vracející typ void).
**/

#include "c206.h"

int solved;
int errflg;

void DLError() {
/*
** Vytiskne upozornìní na to, ¾e do¹lo k chybì.
** Tato funkce bude volána z nìkterých dále implementovaných operací.
**/
    printf ("*ERROR* Chyba pøi práci se seznamem.\n");
    errflg = TRUE;             /* globální promìnná -- pøíznak o¹etøení chyby */
    return;
}

void DLInitList (tDLList *L)  {
/*
** Provede inicializaci seznamu L pøed jeho prvním pou¾itím (tzn. ¾ádná
** z následujících funkcí nebude volána nad neinicializovaným seznamem).
** Tato inicializace se nikdy nebude provádìt nad ji¾ inicializovaným
** seznamem, a proto tuto mo¾nost neo¹etøujte. V¾dy pøedpokládejte,
** ¾e neinicializované promìnné mají nedefinovanou hodnotu.
**/
  L->First = NULL;
  L->Act = NULL;
  L->Last = NULL;
}

void DLDisposeList (tDLList *L) {
/*
** Zru¹í v¹echny prvky seznamu L a uvede seznam do stavu, v jakém
** se nacházel po inicializaci. Ru¹ené prvky seznamu budou korektnì
** uvolnìny voláním operace free.
**/
  tDLElemPtr p = L->First;
  while (p != NULL) {
    tDLElemPtr tmp = p;
    p = tmp->rptr;
    free(tmp);
  }
  L->Act = NULL;
  L->First = NULL;
  L->Last = NULL;
}

void DLInsertFirst (tDLList *L, int val)  {
/*
** Vlo¾í nový prvek na zaèátek seznamu L.
** V pøípadì, ¾e není dostatek pamìti pro nový prvek pøi operaci malloc,
** volá funkci DLError().
**/
  tDLElemPtr p = malloc(sizeof(struct tDLElem));
  if (p == NULL)
    DLError();
  else {
    p->data = val;
    p->lptr = NULL;
    p->rptr = L->First;
    L->First = p;
    if (p->rptr != NULL)        // vkládání do neprázdného seznamu
      p->rptr->lptr = p;
    else                        // vkládání do prázdného seznamu
      L->Last = p;
  }
}

void DLInsertLast(tDLList *L, int val)  {
/*
** Vlo¾í nový prvek na konec seznamu L (symetrická operace k DLInsertFirst).
** V pøípadì, ¾e není dostatek pamìti pro nový prvek pøi operaci malloc,
** volá funkci DLError().
**/
  tDLElemPtr p = malloc(sizeof(struct tDLElem));
  if (p == NULL)
    DLError();
  else {
    p->data = val;
    p->rptr = NULL;
    p->lptr = L->Last;
    L->Last = p;
    if (p->lptr != NULL)        // vkládání do neprázdného seznamu
      p->lptr->rptr = p;
    else                        // vkládání do prázdného seznamu
      L->First = p;
  }
}

void DLFirst (tDLList *L) {
/*
** Nastaví aktivitu na první prvek seznamu L.
** Funkci implementujte jako jediný pøíkaz (nepoèítáme-li return),
** ani¾ byste testovali, zda je seznam L prázdný.
**/
  L->Act = L->First;
}

void DLLast (tDLList *L)  {
/*
** Nastaví aktivitu na poslední prvek seznamu L.
** Funkci implementujte jako jediný pøíkaz (nepoèítáme-li return),
** ani¾ byste testovali, zda je seznam L prázdný.
**/
  L->Act = L->Last;
}

void DLCopyFirst (tDLList *L, int *val) {
/*
** Vrátí hodnotu prvního prvku seznamu L.
** Pokud je seznam L prázdný, volá funkci DLError().
**/
  if (L->First == NULL)
    DLError();
  else
    *val = L->First->data;
}

void DLCopyLast (tDLList *L, int *val)  {
/*
** Vrátí hodnotu posledního prvku seznamu L.
** Pokud je seznam L prázdný, volá funkci DLError().
**/
  if (L->Last == NULL)
    DLError();
  else
    *val = L->Last->data;
}

void DLDeleteFirst (tDLList *L) {
/*
** Zru¹í první prvek seznamu L. Pokud byl první prvek aktivní, aktivita
** se ztrácí. Pokud byl seznam L prázdný, nic se nedìje.
**/
  if (L->First != NULL) {
    if (L->First == L->Act)
      L->Act = NULL;
    tDLElemPtr p = L->First;
    L->First = L->First->rptr;
    if (L->First != NULL)       // seznam s dvì a více polo¾kami
      L->First->lptr = NULL;
    else                        // seznam s jednou polo¾kou
      L->Last = NULL;
    free(p);
  }
}

void DLDeleteLast (tDLList *L)  {
/*
** Zru¹í poslední prvek seznamu L. Pokud byl poslední prvek aktivní,
** aktivita seznamu se ztrácí. Pokud byl seznam L prázdný, nic se nedìje.
**/
  if (L->Last != NULL) {
    if (L->Last == L->Act)
      L->Act = NULL;
    tDLElemPtr p = L->Last;
    L->Last = L->Last->lptr;
    if (L->Last != NULL)        // seznam s dvì a více polo¾kami
      L->Last->rptr = NULL;
    else                        // seznam s jednou polo¾kou
      L->First = NULL;
    free(p);
  }
}

void DLPostDelete (tDLList *L)  {
/*
** Zru¹í prvek seznamu L za aktivním prvkem.
** Pokud je seznam L neaktivní nebo pokud je aktivní prvek
** posledním prvkem seznamu, nic se nedìje.
**/
  if (L->Act != NULL && L->Act->rptr != NULL) {
    tDLElemPtr p = L->Act->rptr;
    L->Act->rptr = p->rptr;
    if (L->Act->rptr != NULL)   // nema¾u poslední polo¾ku
      L->Act->rptr->lptr = L->Act;
    else                        // ma¾u poslední polo¾ku
      L->Last = L->Act;
    free(p);
  }
}

void DLPreDelete (tDLList *L) {
/*
** Zru¹í prvek pøed aktivním prvkem seznamu L .
** Pokud je seznam L neaktivní nebo pokud je aktivní prvek
** prvním prvkem seznamu, nic se nedìje.
**/
  if (L->Act != NULL && L->Act->lptr != NULL) {
    tDLElemPtr p = L->Act->lptr;
    L->Act->lptr = p->lptr;
    if (L->Act->lptr != NULL)   // nema¾u první polo¾ku
      L->Act->lptr->rptr = L->Act;
    else                        // ma¾u první polo¾ku
      L->First = L->Act;
    free(p);
  }
}

void DLPostInsert (tDLList *L, int val) {
/*
** Vlo¾í prvek za aktivní prvek seznamu L.
** Pokud nebyl seznam L aktivní, nic se nedìje.
** V pøípadì, ¾e není dostatek pamìti pro nový prvek pøi operaci malloc,
** volá funkci DLError().
**/
  if (L->Act != NULL) {
    tDLElemPtr p = malloc(sizeof(struct tDLElem));
    if (p == NULL)
      DLError();
    else {
      p->data = val;
      p->rptr = L->Act->rptr;
      p->lptr = L->Act;
      L->Act->rptr = p;
      if (p->rptr != NULL)      // nevkládám za poslední polo¾ku
        p->rptr->lptr = p;
      else                      // vkládám za poslední polo¾ku
        L->Last = p;
    }
  }
}

void DLPreInsert (tDLList *L, int val)    {
/*
** Vlo¾í prvek pøed aktivní prvek seznamu L.
** Pokud nebyl seznam L aktivní, nic se nedìje.
** V pøípadì, ¾e není dostatek pamìti pro nový prvek pøi operaci malloc,
** volá funkci DLError().
**/
  if (L->Act != NULL) {
    tDLElemPtr p = malloc(sizeof(struct tDLElem));
    if (p == NULL)
      DLError();
    else {
      p->data = val;
      p->lptr = L->Act->lptr;
      p->rptr = L->Act;
      L->Act->lptr = p;
      if (p->lptr != NULL)      // nevkládám pøed první polo¾ku
        p->lptr->rptr = p;
      else                      // vkládám pøed první polo¾ku
        L->First = p;
    }
  }
}

void DLCopy (tDLList *L, int *val)  {
/*
** Vrátí hodnotu aktivního prvku seznamu L.
** Pokud seznam L není aktivní, volá funkci DLError ().
**/
  if (L->Act != NULL)
    *val = L->Act->data;
  else
    DLError();
}

void DLActualize (tDLList *L, int val) {
/*
** Pøepí¹e obsah aktivního prvku seznamu L.
** Pokud seznam L není aktivní, nedìlá nic.
**/
  if (L->Act != NULL)
    L->Act->data = val;
}

void DLSucc (tDLList *L)  {
/*
** Posune aktivitu na následující prvek seznamu L.
** Není-li seznam aktivní, nedìlá nic.
** V¹imnìte si, ¾e pøi aktivitì na posledním prvku se seznam stane neaktivním.
**/
  if (L->Act != NULL)
    L->Act = L->Act->rptr;
}


void DLPred (tDLList *L)  {
/*
** Posune aktivitu na pøedchozí prvek seznamu L.
** Není-li seznam aktivní, nedìlá nic.
** V¹imnìte si, ¾e pøi aktivitì na prvním prvku se seznam stane neaktivním.
**/
  if (L->Act != NULL)
    L->Act = L->Act->lptr;
}

int DLActive (tDLList *L) {
/*
** Je-li seznam aktivní, vrací true. V opaèném pøípadì vrací false.
** Funkci implementujte jako jediný pøíkaz.
**/
  return (L->Act != NULL);
}

/* Konec c206.c*/
