
/* c206.c **********************************************************}
{* T�ma: Dvousm�rn� v�zan� line�rn� seznam
**
**                   N�vrh a referen�n� implementace: Bohuslav K�ena, ��jen 2001
**                            P�epracovan� do jazyka C: Martin Tu�ek, ��jen 2004
**                                            �pravy: Bohuslav K�ena, ��jen 2010
**
** Implementujte abstraktn� datov� typ dvousm�rn� v�zan� line�rn� seznam.
** U�ite�n�m obsahem prvku seznamu je hodnota typu int.
** Seznam bude jako datov� abstrakce reprezentov�n prom�nnou
** typu tDLList (DL znamen� Double-Linked a slou�� pro odli�en�
** jmen konstant, typ� a funkc� od jmen u jednosm�rn� v�zan�ho line�rn�ho
** seznamu). Definici konstant a typ� naleznete v hlavi�kov�m souboru c206.h.
**
** Va��m �kolem je implementovat n�sleduj�c� operace, kter� spolu
** s v��e uvedenou datovou ��st� abstrakce tvo�� abstraktn� datov� typ
** obousm�rn� v�zan� line�rn� seznam:
**
**      DLInitList ...... inicializace seznamu p�ed prvn�m pou�it�m,
**      DLDisposeList ... zru�en� v�ech prvk� seznamu,
**      DLInsertFirst ... vlo�en� prvku na za��tek seznamu,
**      DLInsertLast .... vlo�en� prvku na konec seznamu,
**      DLFirst ......... nastaven� aktivity na prvn� prvek,
**      DLLast .......... nastaven� aktivity na posledn� prvek,
**      DLCopyFirst ..... vrac� hodnotu prvn�ho prvku,
**      DLCopyLast ...... vrac� hodnotu posledn�ho prvku,
**      DLDeleteFirst ... zru�� prvn� prvek seznamu,
**      DLDeleteLast .... zru�� posledn� prvek seznamu,
**      DLPostDelete .... ru�� prvek za aktivn�m prvkem,
**      DLPreDelete ..... ru�� prvek p�ed aktivn�m prvkem,
**      DLPostInsert .... vlo�� nov� prvek za aktivn� prvek seznamu,
**      DLPreInsert ..... vlo�� nov� prvek p�ed aktivn� prvek seznamu,
**      DLCopy .......... vrac� hodnotu aktivn�ho prvku,
**      DLActualize ..... p�ep�e obsah aktivn�ho prvku novou hodnotou,
**      DLSucc .......... posune aktivitu na dal�� prvek seznamu,
**      DLPred .......... posune aktivitu na p�edchoz� prvek seznamu,
**      DLActive ........ zji��uje aktivitu seznamu.
**
** P�i implementaci jednotliv�ch funkc� nevolejte ��dnou z funkc�
** implementovan�ch v r�mci tohoto p��kladu, nen�-li u funkce
** explicitn� uvedeno n�co jin�ho.
**
** Nemus�te o�et�ovat situaci, kdy m�sto leg�ln�ho ukazatele na seznam
** p�ed� n�kdo jako parametr hodnotu NULL.
**
** Svou implementaci vhodn� komentujte!
**
** Terminologick� pozn�mka: Jazyk C nepou��v� pojem procedura.
** Proto zde pou��v�me pojem funkce i pro operace, kter� by byly
** v algoritmick�m jazyce Pascalovsk�ho typu implemenov�ny jako
** procedury (v jazyce C procedur�m odpov�daj� funkce vracej�c� typ void).
**/

#include "c206.h"

int solved;
int errflg;

void DLError() {
/*
** Vytiskne upozorn�n� na to, �e do�lo k chyb�.
** Tato funkce bude vol�na z n�kter�ch d�le implementovan�ch operac�.
**/
    printf ("*ERROR* Chyba p�i pr�ci se seznamem.\n");
    errflg = TRUE;             /* glob�ln� prom�nn� -- p��znak o�et�en� chyby */
    return;
}

void DLInitList (tDLList *L)  {
/*
** Provede inicializaci seznamu L p�ed jeho prvn�m pou�it�m (tzn. ��dn�
** z n�sleduj�c�ch funkc� nebude vol�na nad neinicializovan�m seznamem).
** Tato inicializace se nikdy nebude prov�d�t nad ji� inicializovan�m
** seznamem, a proto tuto mo�nost neo�et�ujte. V�dy p�edpokl�dejte,
** �e neinicializovan� prom�nn� maj� nedefinovanou hodnotu.
**/
  L->First = NULL;
  L->Act = NULL;
  L->Last = NULL;
}

void DLDisposeList (tDLList *L) {
/*
** Zru�� v�echny prvky seznamu L a uvede seznam do stavu, v jak�m
** se nach�zel po inicializaci. Ru�en� prvky seznamu budou korektn�
** uvoln�ny vol�n�m operace free.
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
** Vlo�� nov� prvek na za��tek seznamu L.
** V p��pad�, �e nen� dostatek pam�ti pro nov� prvek p�i operaci malloc,
** vol� funkci DLError().
**/
  tDLElemPtr p = malloc(sizeof(struct tDLElem));
  if (p == NULL)
    DLError();
  else {
    p->data = val;
    p->lptr = NULL;
    p->rptr = L->First;
    L->First = p;
    if (p->rptr != NULL)        // vkl�d�n� do nepr�zdn�ho seznamu
      p->rptr->lptr = p;
    else                        // vkl�d�n� do pr�zdn�ho seznamu
      L->Last = p;
  }
}

void DLInsertLast(tDLList *L, int val)  {
/*
** Vlo�� nov� prvek na konec seznamu L (symetrick� operace k DLInsertFirst).
** V p��pad�, �e nen� dostatek pam�ti pro nov� prvek p�i operaci malloc,
** vol� funkci DLError().
**/
  tDLElemPtr p = malloc(sizeof(struct tDLElem));
  if (p == NULL)
    DLError();
  else {
    p->data = val;
    p->rptr = NULL;
    p->lptr = L->Last;
    L->Last = p;
    if (p->lptr != NULL)        // vkl�d�n� do nepr�zdn�ho seznamu
      p->lptr->rptr = p;
    else                        // vkl�d�n� do pr�zdn�ho seznamu
      L->First = p;
  }
}

void DLFirst (tDLList *L) {
/*
** Nastav� aktivitu na prvn� prvek seznamu L.
** Funkci implementujte jako jedin� p��kaz (nepo��t�me-li return),
** ani� byste testovali, zda je seznam L pr�zdn�.
**/
  L->Act = L->First;
}

void DLLast (tDLList *L)  {
/*
** Nastav� aktivitu na posledn� prvek seznamu L.
** Funkci implementujte jako jedin� p��kaz (nepo��t�me-li return),
** ani� byste testovali, zda je seznam L pr�zdn�.
**/
  L->Act = L->Last;
}

void DLCopyFirst (tDLList *L, int *val) {
/*
** Vr�t� hodnotu prvn�ho prvku seznamu L.
** Pokud je seznam L pr�zdn�, vol� funkci DLError().
**/
  if (L->First == NULL)
    DLError();
  else
    *val = L->First->data;
}

void DLCopyLast (tDLList *L, int *val)  {
/*
** Vr�t� hodnotu posledn�ho prvku seznamu L.
** Pokud je seznam L pr�zdn�, vol� funkci DLError().
**/
  if (L->Last == NULL)
    DLError();
  else
    *val = L->Last->data;
}

void DLDeleteFirst (tDLList *L) {
/*
** Zru�� prvn� prvek seznamu L. Pokud byl prvn� prvek aktivn�, aktivita
** se ztr�c�. Pokud byl seznam L pr�zdn�, nic se ned�je.
**/
  if (L->First != NULL) {
    if (L->First == L->Act)
      L->Act = NULL;
    tDLElemPtr p = L->First;
    L->First = L->First->rptr;
    if (L->First != NULL)       // seznam s dv� a v�ce polo�kami
      L->First->lptr = NULL;
    else                        // seznam s jednou polo�kou
      L->Last = NULL;
    free(p);
  }
}

void DLDeleteLast (tDLList *L)  {
/*
** Zru�� posledn� prvek seznamu L. Pokud byl posledn� prvek aktivn�,
** aktivita seznamu se ztr�c�. Pokud byl seznam L pr�zdn�, nic se ned�je.
**/
  if (L->Last != NULL) {
    if (L->Last == L->Act)
      L->Act = NULL;
    tDLElemPtr p = L->Last;
    L->Last = L->Last->lptr;
    if (L->Last != NULL)        // seznam s dv� a v�ce polo�kami
      L->Last->rptr = NULL;
    else                        // seznam s jednou polo�kou
      L->First = NULL;
    free(p);
  }
}

void DLPostDelete (tDLList *L)  {
/*
** Zru�� prvek seznamu L za aktivn�m prvkem.
** Pokud je seznam L neaktivn� nebo pokud je aktivn� prvek
** posledn�m prvkem seznamu, nic se ned�je.
**/
  if (L->Act != NULL && L->Act->rptr != NULL) {
    tDLElemPtr p = L->Act->rptr;
    L->Act->rptr = p->rptr;
    if (L->Act->rptr != NULL)   // nema�u posledn� polo�ku
      L->Act->rptr->lptr = L->Act;
    else                        // ma�u posledn� polo�ku
      L->Last = L->Act;
    free(p);
  }
}

void DLPreDelete (tDLList *L) {
/*
** Zru�� prvek p�ed aktivn�m prvkem seznamu L .
** Pokud je seznam L neaktivn� nebo pokud je aktivn� prvek
** prvn�m prvkem seznamu, nic se ned�je.
**/
  if (L->Act != NULL && L->Act->lptr != NULL) {
    tDLElemPtr p = L->Act->lptr;
    L->Act->lptr = p->lptr;
    if (L->Act->lptr != NULL)   // nema�u prvn� polo�ku
      L->Act->lptr->rptr = L->Act;
    else                        // ma�u prvn� polo�ku
      L->First = L->Act;
    free(p);
  }
}

void DLPostInsert (tDLList *L, int val) {
/*
** Vlo�� prvek za aktivn� prvek seznamu L.
** Pokud nebyl seznam L aktivn�, nic se ned�je.
** V p��pad�, �e nen� dostatek pam�ti pro nov� prvek p�i operaci malloc,
** vol� funkci DLError().
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
      if (p->rptr != NULL)      // nevkl�d�m za posledn� polo�ku
        p->rptr->lptr = p;
      else                      // vkl�d�m za posledn� polo�ku
        L->Last = p;
    }
  }
}

void DLPreInsert (tDLList *L, int val)    {
/*
** Vlo�� prvek p�ed aktivn� prvek seznamu L.
** Pokud nebyl seznam L aktivn�, nic se ned�je.
** V p��pad�, �e nen� dostatek pam�ti pro nov� prvek p�i operaci malloc,
** vol� funkci DLError().
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
      if (p->lptr != NULL)      // nevkl�d�m p�ed prvn� polo�ku
        p->lptr->rptr = p;
      else                      // vkl�d�m p�ed prvn� polo�ku
        L->First = p;
    }
  }
}

void DLCopy (tDLList *L, int *val)  {
/*
** Vr�t� hodnotu aktivn�ho prvku seznamu L.
** Pokud seznam L nen� aktivn�, vol� funkci DLError ().
**/
  if (L->Act != NULL)
    *val = L->Act->data;
  else
    DLError();
}

void DLActualize (tDLList *L, int val) {
/*
** P�ep�e obsah aktivn�ho prvku seznamu L.
** Pokud seznam L nen� aktivn�, ned�l� nic.
**/
  if (L->Act != NULL)
    L->Act->data = val;
}

void DLSucc (tDLList *L)  {
/*
** Posune aktivitu na n�sleduj�c� prvek seznamu L.
** Nen�-li seznam aktivn�, ned�l� nic.
** V�imn�te si, �e p�i aktivit� na posledn�m prvku se seznam stane neaktivn�m.
**/
  if (L->Act != NULL)
    L->Act = L->Act->rptr;
}


void DLPred (tDLList *L)  {
/*
** Posune aktivitu na p�edchoz� prvek seznamu L.
** Nen�-li seznam aktivn�, ned�l� nic.
** V�imn�te si, �e p�i aktivit� na prvn�m prvku se seznam stane neaktivn�m.
**/
  if (L->Act != NULL)
    L->Act = L->Act->lptr;
}

int DLActive (tDLList *L) {
/*
** Je-li seznam aktivn�, vrac� true. V opa�n�m p��pad� vrac� false.
** Funkci implementujte jako jedin� p��kaz.
**/
  return (L->Act != NULL);
}

/* Konec c206.c*/
