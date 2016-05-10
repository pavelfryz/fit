/*
 * Soubor:  proj4.c
 * Datum:   12.12.2010
 * Autor:   Pavel Fr�z, xfryzp00@stud.fit.vutbr.cz
 * Projekt: �esk� �azen�, projekt �. 4 pro p�edm�t IZP
 * Popis:   Program vyb�r� sloupec z tabulky, kter� tiskne
 *          do souboru. M��e b�t pou��t jednoduch� filtr
 *          a data mohou b�t se�azena.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "proj4list.h"
#include "proj4string.h"


/** Stavov� k�dy programu */
enum tstates
{
  CHELP,       /**< N�pov�da */
  CPRINT,      /**< Tisknout. */
  CAFTER,      /**< Vybrat abecedne za, vytisknou. */
  CBEFORE,     /**< Vybrat abecedne pred, vytisknou. */
  CAFTER_SORT, /**< Vybrat, seradit vytisknou. */
  CBEFORE_SORT,/**< Vybrat, seradit vytisknou. */
  CSORT,       /**< Radit a tisknout. */
};

/** Chybov� hl�en� odpov�daj�c� chybov�m k�d�m. */
const char *ECODEMSG[] =
{
  /* EOK */
  "V�e v po��dku.\n",
  /* ECLWRONG */
  "Chybn� parametry p��kazov�ho ��dku!\n",
  /* EFOPEN */
  "Chyba p�i otev�r�n� souboru!\n",
  /* EFCREATE */  
  "Chyba! Nepoda�ilo se vytvo�it v�stupn� soubor.\n",
  /* EBADHEAD */
  "Chyba! �patn� hlavi�ka souboru.\n",
  /* EBADVAL */
  "Chyba! �patn� form�t vstupn�ch dat.\n",
  /* EALLOC */
  "Chyba! Nedostatek pam�ti\n",
  /* EPRINT */
  "Chyba p�i z�pisu!\n",
  
  
  "Nastala nep�edv�dan� chyba! Vypn�te po��ta� a rychle ute�te.\n",
};

const char *HELPMSG =
  "Program �esk� �azen�.\n"
  "Autor: Pavel Fr�z (c) 2010\n"
  "Program vypisuje slova v ur�en�m sloupci tabulky.\n"
  "P�i zad�n� p�ep�na�� after/before vyb�r� pouze n�kter� ��dky.\n"
  "P�i zad�n� p�ep�na�� sort/qsort se�ad� v�pis podle abecedy.\n"
  "Pou�it�: proj4 -h\n"
  "         proj4 [--after SLOUPEC �ET�ZEC]  --print SLOUPEC [--sort]\n"
  "         proj4 [--before SLOUPEC �ET�ZEC] --print SLOUPEC [--sort]\n"
  "         proj4 [--after SLOUPEC �ET�ZEC]  --print SLOUPEC [--qsort]\n"
  "         proj4 [--before SLOUPEC �ET�ZEC] --print SLOUPEC [--qsort]\n"
  "Popis parametr�:\n"
  "-h Vytiskne n�pov�du k pou�it� programu.\n"
  "--before SLOUPEC �ET�ZEC netiskne ��dky, kde slovo nen� abecedn� p�ed �ET�ZCEM\n" 
  "--after SLOUPEC �ET�ZEC netiskne ��dky, kde slovo nen� abecedn� p�ed �ET�ZCEM\n"
  "--print SLOUPEC vytiskne slova ve SLOUPCI\n"
  "--sort v�pis bude se�azen� podle abecedy\n"
  "--qsort v�pis bude se�azen� podle abecedy\n"
  "SLOUPEC identifikuje pozici slova na ��dku\n";
  
/**
 * Struktura obsahuj�c� hodnoty parametr� p��kazov� ��dky.
 */
typedef struct params
{
  char * fileIn;  /**< N�zev vsupniho souboru. */
  char * fileOut; /**< N�zev vystupniho souboru. */
  char * print;   /**< N�zev sloupce kter� se m� vytisknout. */ 
  char * select;  /**< Sloupec s kriteriem. */
  char * string;  /**< Kriterium podle ktereho se vybyra radek pro tisk. */
  int ecode;      /**< Chybov� k�d programu, odpov�d� v��tu tecodes. */
  int state;      /**< Stavov� k�d programu, odpov�d� v��tu tstates. */
  bool quick;
} TParams;

/////////////////////////////////////////////////////////////////
void printECode(int ecode);
TParams getParams(int argc, char *argv[]);
int loadList(TList *list,const TParams *params);
int loadHead(FILE *file,const TParams *params, int *printPos, int *selectPos, int *numCols);
/////////////////////////////////////////////////////////////////


/**
 * Vytiskne hl�en� odpov�daj�c� chybov�mu k�du.
 * @param ecode k�d chyby programu
 */
void printECode(int ecode)
{
  if (ecode < EOK || ecode > EUNKNOWN)
  { ecode = EUNKNOWN; }

  fprintf(stderr, "%s", ECODEMSG[ecode]);
}

/**
 * Zpracuje argumenty p��kazov�ho ��dku a vr�t� je ve struktu�e TParams.
 * Pokud je form�t argument� chybn�, ukon�� program s chybov�m k�dem.
 * @param argc Po�et argument�.
 * @param argv Pole textov�ch �et�zc� s argumenty.
 * @return Vrac� analyzovan� argumenty p��kazov�ho ��dku.
 */
TParams getParams(int argc, char *argv[])
{
  TParams result =
  { // inicializace struktury
    .fileIn = NULL,
    .fileOut = NULL,
    .print = NULL,
    .select = NULL,
    .string = NULL,
    .ecode = EOK,
    .state = CHELP,
    .quick = false,
  };

  if (argc == 2 && strcmp("-h", argv[1]) == 0)
  { // tisk n�pov�dy
    result.state = CHELP;
  }
  else if (argc == 5 && strcmp("--print", argv[1]) == 0)
  { // pouze tisk 
    result.state = CPRINT;
    result.print = argv[2];
    result.fileIn = argv[3];
    result.fileOut = argv[4];
  }
  else if (argc == 6)
  { // tisk a razeni
    if(strcmp("--print", argv[1]) == 0)
    {
      result.state = CSORT;
      result.print = argv[2];
      result.fileIn = argv[4];
      result.fileOut = argv[5];
      if(strcmp("--qsort", argv[3])==0) result.quick=true;
      if(strcmp("--sort", argv[3])!=0&&strcmp("--qsort", argv[3])!=0) result.ecode =ECLWRONG;
    }
    else result.ecode = ECLWRONG;
  }
  else if (argc == 8)
  { // sedm parametru
    if(strcmp("--after", argv[1])==0) result.state = CAFTER;
    else if (strcmp("--before", argv[1])==0) result.state = CBEFORE;
    else
    {
      result.ecode = ECLWRONG;
      return result;
    }
    result.select = argv[2];
    result.string = argv[3];
    result.print = argv[5];
    result.fileIn = argv[6];
    result.fileOut = argv[7];
    if(strcmp("--print", argv[4])!=0) result.ecode =ECLWRONG;
  }  
  else if (argc == 9)
  { // osm
    if(strcmp("--after", argv[1])==0) result.state = CAFTER_SORT;
    else if (strcmp("--before", argv[1])==0) result.state = CBEFORE_SORT;
    else
    {
      result.ecode = ECLWRONG;
      return result;
    }
    result.select = argv[2];
    result.string = argv[3];
    result.print = argv[5];
    result.fileIn = argv[7];
    result.fileOut = argv[8];
    if(strcmp("--qsort", argv[6])==0) result.quick=true;
    if(strcmp("--print", argv[4])!=0) result.ecode =ECLWRONG;
    if(strcmp("--sort", argv[6])!=0&&strcmp("--qsort", argv[6])!=0) result.ecode =ECLWRONG;
  }
  else
  { // p��li� mnoho parametr�
    result.ecode = ECLWRONG;
  }

  return result;
}

/**
 * Na�te seznam �et�zc� k vytisknut� podle zadan�ch parametr�.
 * @param list Seznam.
 * @param params Parametry.
 * @return Vrac� k�d chyby.
 */
int loadList(TList *list,const TParams *params)
{
  char *line;
  char *column;
  int numCols=0;
  int printPos=0, selectPos=0;
  int i=0;
  int errcode;
 
  FILE * file;
  if ((file = fopen(params->fileIn, "r")) == NULL)
  {  //chyba pri otevirani souboru
    return EFOPEN;
  }
  
  if((errcode=loadHead(file, params, &printPos, &selectPos, &numCols))!=EOK)
    return errcode;
  
  while(true)
  {  
  	char * print;
  	char * select;
  	int lenght;
  	char *data;
  	TItem *item;
  	
  	if((i=readLine(&line, file))==EALLOC)
  	{ // chyba alokace
  	  while(list->head!=NULL) deleteFirst(list,free);
  	  return EALLOC;
  	}
    if(line[0]=='\0')
    { // prazdny radek nebo konec souboru
    	free(line);
    	if(!feof(file)) continue; 
    	fclose(file);
    	return EOK;
    } 
  	i=0;
  	column = strtok (line," ");
    while (column != NULL)
    {
      i++;
      if(i==printPos) print=column;
      if(i==selectPos) select=column; 
      column = strtok (NULL, " ");
    }
    if(i!=numCols)
    { // sloupce neodpovidaji hlavicce
      while(list->head!=NULL) deleteFirst(list,free);
      free(line);
      fclose(file);
      return EBADVAL;
    }

    if(params->state==CAFTER||params->state==CAFTER_SORT)
    {   
      if(strCmpCZ(select,params->string)<=0)
      { // neni abecedne za 
        free(line);
        continue;
      }
    }
    else if(params->state==CBEFORE||params->state==CBEFORE_SORT)
    {  
      if(strCmpCZ(select,params->string)>=0)
      { // neni abecedne pred
        free(line);
        continue;
      }
    }
    
    // pridani retezce do seznamu k vytisknuti    
    lenght=strlen(print)+1;
    if((data = malloc((lenght+1)*sizeof(char)))==NULL)
    { // chyba alokace
      while(list->head!=NULL) deleteFirst(list,free);
      free(line);
      fclose(file);
      return EALLOC;
    }
    memcpy(data,print,lenght*sizeof(char));
    if((item = newItem(data))==NULL)
    { // chyba alokace
      while(list->head!=NULL) deleteFirst(list,free);
      free(line);
      fclose(file);
      free(data);
      return EALLOC;
    }
    insertLast(list,item);
    free(line);
  }
  fclose(file);
  return EOK;	
}

/**
 * Zjisti tvar hlavi�ky souboru.
 * @param file Soubor.
 * @param params Parametry.
 * @param printPos Pozice sloupce pro tisk.
 * @param selectPos Pozice sloupce pro vyber.
 * @param numCols Po�et sloupc�.
 * @return Vrac� k�d chyby.
 */
int loadHead(FILE *file,const TParams *params, int *printPos, int *selectPos, int *numCols)
{
  char *line;
  char *column;
  bool select=(params->state>=CAFTER && params->state<=CBEFORE_SORT);
  
  *printPos=0; 
  *selectPos=0;
  *numCols=0;
  
  if(readLine(&line, file)==EALLOC)
  { //chyba alokace
    fclose(file);
    return EALLOC;
  }
  column = strtok (line," ");
  while (column != NULL)
  {
    (*numCols)++;
    if(strCmpCZ(params->print,column)==0)
    { // nalezen sloupec pro tisk
      if(!(*printPos)) *printPos=*numCols;
      else
      { // duplicitni sloupec pro tisk
        free(line);
        fclose(file);
        return EBADHEAD;
      } 
    }
    if(select && strCmpCZ(params->select,column)==0) 
    { // nalezen sloupec pro vyber
      if(!(*selectPos)) *selectPos=*numCols;
      else
      { // duplicitni sloupec pro vyber
        free(line);
        fclose(file);
        return EBADHEAD;
      }
    }
    column = strtok (NULL, " ");
  }
  free(line);
  if(*printPos==0) return EBADHEAD;
  if(select&&(*selectPos)==0) return EBADHEAD;
  return EOK;
}

/////////////////////////////////////////////////////////////////
/**
 * Hlavn� program.
 */
int main(int argc, char *argv[])
{
  TList list;
  int errcode = EOK;
  TParams params = getParams(argc, argv);
  if (params.ecode != EOK)
  { // n�co nestandardn�ho
    printECode(params.ecode);
    return EXIT_FAILURE;
  }

  if (params.state == CHELP)
  {  // napoveda
    printf("%s", HELPMSG);
    return EXIT_SUCCESS;
  }

  listInit(&list);
    
  errcode = loadList(&list,&params);
  if (errcode != EOK)
  { // n�co nestandardn�ho
    printECode(errcode);
    return EXIT_FAILURE;
  }
  
  if(params.state<=CSORT && params.state>=CAFTER_SORT)
  { // razeni seznamu
    if(params.quick) quickSort(&list,cmpString);
    else sortList(&list,cmpString);
  }
  
  FILE * file;
  if ((file = fopen(params.fileOut, "w")) == NULL)
  {  //chyba pri vytvoreni noveho souboru
    while(list.head!=NULL) deleteFirst(&list,free);
    printECode(EFCREATE);
    return EXIT_FAILURE;
  }
  
  errcode = printList(&list,file,printString);
  if (errcode != EOK)
  { // n�co nestandardn�ho
    fclose(file);
    while(list.head!=NULL) deleteFirst(&list,free);
    printECode(errcode);
    return EXIT_FAILURE;
  }
  
  fclose(file);
  while(list.head!=NULL) deleteFirst(&list,free);
  
  return EXIT_SUCCESS;
}
/* konec proj4.c */
