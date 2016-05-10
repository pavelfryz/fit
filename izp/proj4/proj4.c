/*
 * Soubor:  proj4.c
 * Datum:   12.12.2010
 * Autor:   Pavel Frýz, xfryzp00@stud.fit.vutbr.cz
 * Projekt: Èeské øazení, projekt è. 4 pro pøedmìt IZP
 * Popis:   Program vybírá sloupec z tabulky, který tiskne
 *          do souboru. Mù¾e být pou¾ít jednoduchý filtr
 *          a data mohou být seøazena.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "proj4list.h"
#include "proj4string.h"


/** Stavové kódy programu */
enum tstates
{
  CHELP,       /**< Nápovìda */
  CPRINT,      /**< Tisknout. */
  CAFTER,      /**< Vybrat abecedne za, vytisknou. */
  CBEFORE,     /**< Vybrat abecedne pred, vytisknou. */
  CAFTER_SORT, /**< Vybrat, seradit vytisknou. */
  CBEFORE_SORT,/**< Vybrat, seradit vytisknou. */
  CSORT,       /**< Radit a tisknout. */
};

/** Chybová hlá¹ení odpovídající chybovým kódùm. */
const char *ECODEMSG[] =
{
  /* EOK */
  "V¹e v poøádku.\n",
  /* ECLWRONG */
  "Chybné parametry pøíkazového øádku!\n",
  /* EFOPEN */
  "Chyba pøi otevírání souboru!\n",
  /* EFCREATE */  
  "Chyba! Nepodaøilo se vytvoøit výstupní soubor.\n",
  /* EBADHEAD */
  "Chyba! ©patná hlavièka souboru.\n",
  /* EBADVAL */
  "Chyba! ©patný formát vstupních dat.\n",
  /* EALLOC */
  "Chyba! Nedostatek pamìti\n",
  /* EPRINT */
  "Chyba pøi zápisu!\n",
  
  
  "Nastala nepøedvídaná chyba! Vypnìte poèítaè a rychle uteète.\n",
};

const char *HELPMSG =
  "Program Èeské øazení.\n"
  "Autor: Pavel Frýz (c) 2010\n"
  "Program vypisuje slova v urèeném sloupci tabulky.\n"
  "Pøi zadání pøepínaèù after/before vybírá pouze nìkteré øádky.\n"
  "Pøi zadání pøepínaèù sort/qsort seøadí výpis podle abecedy.\n"
  "Pou¾ití: proj4 -h\n"
  "         proj4 [--after SLOUPEC ØETÌZEC]  --print SLOUPEC [--sort]\n"
  "         proj4 [--before SLOUPEC ØETÌZEC] --print SLOUPEC [--sort]\n"
  "         proj4 [--after SLOUPEC ØETÌZEC]  --print SLOUPEC [--qsort]\n"
  "         proj4 [--before SLOUPEC ØETÌZEC] --print SLOUPEC [--qsort]\n"
  "Popis parametrù:\n"
  "-h Vytiskne nápovìdu k pou¾ití programu.\n"
  "--before SLOUPEC ØETÌZEC netiskne øádky, kde slovo není abecednì pøed ØETÌZCEM\n" 
  "--after SLOUPEC ØETÌZEC netiskne øádky, kde slovo není abecednì pøed ØETÌZCEM\n"
  "--print SLOUPEC vytiskne slova ve SLOUPCI\n"
  "--sort výpis bude seøazený podle abecedy\n"
  "--qsort výpis bude seøazený podle abecedy\n"
  "SLOUPEC identifikuje pozici slova na øádku\n";
  
/**
 * Struktura obsahující hodnoty parametrù pøíkazové øádky.
 */
typedef struct params
{
  char * fileIn;  /**< Název vsupniho souboru. */
  char * fileOut; /**< Název vystupniho souboru. */
  char * print;   /**< Název sloupce který se má vytisknout. */ 
  char * select;  /**< Sloupec s kriteriem. */
  char * string;  /**< Kriterium podle ktereho se vybyra radek pro tisk. */
  int ecode;      /**< Chybový kód programu, odpovídá výètu tecodes. */
  int state;      /**< Stavový kód programu, odpovídá výètu tstates. */
  bool quick;
} TParams;

/////////////////////////////////////////////////////////////////
void printECode(int ecode);
TParams getParams(int argc, char *argv[]);
int loadList(TList *list,const TParams *params);
int loadHead(FILE *file,const TParams *params, int *printPos, int *selectPos, int *numCols);
/////////////////////////////////////////////////////////////////


/**
 * Vytiskne hlá¹ení odpovídající chybovému kódu.
 * @param ecode kód chyby programu
 */
void printECode(int ecode)
{
  if (ecode < EOK || ecode > EUNKNOWN)
  { ecode = EUNKNOWN; }

  fprintf(stderr, "%s", ECODEMSG[ecode]);
}

/**
 * Zpracuje argumenty pøíkazového øádku a vrátí je ve struktuøe TParams.
 * Pokud je formát argumentù chybný, ukonèí program s chybovým kódem.
 * @param argc Poèet argumentù.
 * @param argv Pole textových øetìzcù s argumenty.
 * @return Vrací analyzované argumenty pøíkazového øádku.
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
  { // tisk nápovìdy
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
  { // pøíli¹ mnoho parametrù
    result.ecode = ECLWRONG;
  }

  return result;
}

/**
 * Naète seznam øetìzcù k vytisknutí podle zadaných parametrù.
 * @param list Seznam.
 * @param params Parametry.
 * @return Vrací kód chyby.
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
 * Zjisti tvar hlavièky souboru.
 * @param file Soubor.
 * @param params Parametry.
 * @param printPos Pozice sloupce pro tisk.
 * @param selectPos Pozice sloupce pro vyber.
 * @param numCols Poèet sloupcù.
 * @return Vrací kód chyby.
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
 * Hlavní program.
 */
int main(int argc, char *argv[])
{
  TList list;
  int errcode = EOK;
  TParams params = getParams(argc, argv);
  if (params.ecode != EOK)
  { // nìco nestandardního
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
  { // nìco nestandardního
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
  { // nìco nestandardního
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
