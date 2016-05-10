
/* c201.c *********************************************************************}
{* T�ma: Jednosm�rn� line�rn� seznam
**
**                     N�vrh a referen�n� implementace: Petr P�ikryl, ��jen 1994
**                                          �pravy: Andrea N�mcov� listopad 1996
**                                                   Petr P�ikryl, listopad 1997
**                                P�epracovan� zad�n�: Petr P�ikryl, b�ezen 1998
**                                  P�epis do jazyka C: Martin Tu�ek, ��jen 2004
**                                            �pravy: Bohuslav K�ena, ��jen 2010
**
** Implementujte abstraktn� datov� typ jednosm�rn� line�rn� seznam.
** U�ite�n�m obsahem prvku seznamu je cel� ��slo typu int.
** Seznam bude jako datov� abstrakce reprezentov�n prom�nnou typu tList.
** Definici konstant a typ� naleznete v hlavi�kov�m souboru c201.h.
**
** Va��m �kolem je implementovat n�sleduj�c� operace, kter� spolu s v��e
** uvedenou datovou ��st� abstrakce tvo�� abstraktn� datov� typ tList:
**
**      InitList ...... inicializace seznamu p�ed prvn�m pou�it�m,
**      DisposeList ... zru�en� v�ech prvk� seznamu,
**      InsertFirst ... vlo�en� prvku na za��tek seznamu,
**      First ......... nastaven� aktivity na prvn� prvek,
**      CopyFirst ..... vrac� hodnotu prvn�ho prvku,
**      DeleteFirst ... zru�� prvn� prvek seznamu,
**      PostDelete .... ru�� prvek za aktivn�m prvkem,
**      PostInsert .... vlo�� nov� prvek za aktivn� prvek seznamu,
**      Copy .......... vrac� hodnotu aktivn�ho prvku,
**      Actualize ..... p�ep��e obsah aktivn�ho prvku novou hodnotou,
**      Succ .......... posune aktivitu na dal�� prvek seznamu,
**      Active ........ zji��uje aktivitu seznamu.
**
** P�i implementaci funkc� nevolejte ��dnou z funkc� implementovan�ch v r�mci
** tohoto p��kladu, nen�-li u funkce explicitn� uvedeno n�co jin�ho.
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

#include "c201.h"

int solved;
int errflg;

void Error()  {
/*
** Vytiskne upozorn�n� na to, �e do�lo k chyb�.
** Tato funkce bude vol�na z n�kter�ch d�le implementovan�ch operac�.
**/
    printf ("*ERROR* Chyba p�i pr�ci se seznamem.\n");
    errflg = TRUE;                      /* glob�ln� prom�nn� -- p��znak chyby */
}

void InitList (tList *L)  {
/*
** Provede inicializaci seznamu L p�ed jeho prvn�m pou�it�m (tzn. ��dn�
** z n�sleduj�c�ch funkc� nebude vol�na nad neinicializovan�m seznamem).
** Tato inicializace se nikdy nebude prov�d�t nad ji� inicializovan�m
** seznamem, a proto tuto mo�nost neo�et�ujte. V�dy p�edpokl�dejte,
** �e neinicializovan� prom�nn� maj� nedefinovanou hodnotu.
**/
  L->Act = NULL;
  L->First = NULL;
}

void DisposeList (tList *L) {
/*
** Zru�� v�echny prvky seznamu L a uvede seznam L do stavu, v jak�m se nach�zel
** po inicializaci. V�echny prvky seznamu L budou korektn� uvoln�ny vol�n�m
** operace free.
***/
  tElemPtr p = L->First;
  while (p != NULL) {
    tElemPtr tmp = p;
    p = tmp->ptr;
    free(tmp);
  }
  L->Act = NULL;
  L->First = NULL;
}

void InsertFirst (tList *L, int val)  {
/*
** Vlo�� prvek s hodnotou val na za��tek seznamu L.
** V p��pad�, �e nen� dostatek pam�ti pro nov� prvek p�i operaci malloc,
** vol� funkci Error().
**/
  tElemPtr p = malloc(sizeof(struct tElem));
  if (p == NULL)
    Error();
  else {
    p->data = val;
    p->ptr = L->First;
    L->First = p;
  }
}

void First (tList *L)   {
/*
** Nastav� aktivitu seznamu L na jeho prvn� prvek.
** Funkci implementujte jako jedin� p��kaz, ani� byste testovali,
** zda je seznam L pr�zdn�.
**/
  L->Act = L->First;
}

void CopyFirst (tList *L, int *val) {
/*
** Vr�t� hodnotu prvn�ho prvku seznamu L.
** Pokud je seznam L pr�zdn�, vol� funkci Error().
**/
  if (L->First == NULL)
    Error();
  else
    *val = L->First->data;
}

void DeleteFirst (tList *L) {
/*
** Ru�� prvn� prvek seznamu L. Pokud byl ru�en� prvek aktivn�, aktivita seznamu
** se ztr�c�. Pokud byl seznam L pr�zdn�, nic se ned�je!
**/
  if (L->First != NULL) {
    if (L->First == L->Act)
      L->Act = NULL;
    tElemPtr p = L->First;
    L->First = L->First->ptr;
    free(p);
  }
}

void PostDelete (tList *L)      {
/*
** Ru�� prvek seznamu L za aktivn�m prvkem. Pokud nen� seznam L aktivn�
** nebo pokud je aktivn� posledn� prvek seznamu L, nic se ned�je!
**/
  if (L->Act != NULL && L->Act->ptr != NULL) {
    tElemPtr p = L->Act->ptr;
    L->Act->ptr = p->ptr;
    free(p);
  }
}

void PostInsert (tList *L, int val) {
/*
** Vlo�� prvek s hodnotou val za aktivn� prvek seznamu L.
** Pokud nebyl seznam L aktivn�, nic se ned�je!
** V p��pad�, �e nen� dostatek pam�ti pro nov� prvek p�i operaci malloc,
** zavol� funkci Error().
**/
  if (L->Act != NULL) {
    tElemPtr p = malloc(sizeof(struct tElem));
    if (p == NULL)
      Error();
    else {
      p->data = val;
      p->ptr = L->Act->ptr;
      L->Act->ptr = p;
    }
  }
}

void Copy (tList *L, int *val)    {
/*
** Vr�t� hodnotu aktivn�ho prvku seznamu L.
** Pokud seznam nen� aktivn�, zavol� funkci Error().
**/
  if (L->Act != NULL)
    *val = L->Act->data;
  else
    Error();
}

void Actualize (tList *L, int val)  {
/*
** P�ep��e data aktivn�ho prvku seznamu L.
** Pokud seznam L nen� aktivn�, ned�l� nic!
**/
  if (L->Act != NULL)
    L->Act->data = val;
}

void Succ (tList *L)  {
/*
** Posune aktivitu na n�sleduj�c� prvek seznamu L.
** V�imn�te si, �e touto operac� se m��e aktivn� seznam st�t neaktivn�m.
** Pokud seznam L nen� aktivn�, ned�l� nic!
**/
  if (L->Act != NULL)
    L->Act = L->Act->ptr;
}

int Active (tList *L)   {
/*
** Je-li seznam L aktivn�, vrac� True. V opa�n�m p��pad� vrac� false.
** Tuto funkci implementujte jako jedin� p��kaz return.
**/
  return (L->Act != NULL);
}

/* Konec c201.c */
