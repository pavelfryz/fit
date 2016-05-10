/*
 * Soubor:  proj4string.c
 * Datum:   12.12.2010
 * Autor:   Pavel Fr�z, xfryzp00@stud.fit.vutbr.cz
 * Projekt: �esk� �azen�, projekt �. 4 pro p�edm�t IZP
 * Popis:   Funkce pro pr�ci s �et�zci
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include "proj4string.h"
#define CHVALUE 14
#define SIZE 256
#define INCREMENT 16

/**
 * P�e�te ��dek souboru do bufferu.
 * @param buffer Buffer.
 * @param stream Soubor.
 * @return K�d chyby nebo EOF.
 */
int readLine(char **buffer, FILE *stream)
{
  int c, i=0, size=SIZE;
  
  if((*buffer=malloc(size*sizeof(char)))==NULL)
    return EALLOC;
  while((c=getc(stream))!=EOF && c!='\n')
  {
    (*buffer)[i]=c;
    i++;
    if(i==size-1)
    {
      char *buf;
      size+=INCREMENT;
      buf=realloc(*buffer,size*sizeof(char));
      if(buf==NULL)
      {
        free(*buffer);
        return EALLOC;
      }
      else *buffer=buf;  
    }
  }
  (*buffer)[i]='\0';
  if(c==EOF) return EOF;
  return EOK;
}

/**
 * Porovn� dva �et�zce.
 * @param str1 Prvni �et�zec.
 * @param str2 Druh� �et�zec.
 * @return ��slo indikuj�c� vztah mezi �et�zci.
 */
int strCmpCZ(const char *str1,const char *str2)
{
  static const unsigned char *cmpTable[]={ cmpTablePrim,cmpTableSec };
  unsigned char value1, value2;
  int count=strlen(str1);
  for(int table=0;table<2;table++)
  {// porovnavani podle 1. tabulky, v pripade shody podle 2. tabulky  
    for(int i=0;i<=count;i++)
    {// porovnava retezec znak po znaku
      if(isCH(&(str1[i]))) value1=CHVALUE;
      else value1 = cmpTable[table][(unsigned char)str1[i]];
      if(isCH(&(str2[i]))) value2=CHVALUE;
      else value2 = cmpTable[table][(unsigned char)str2[i]];  	
      
      if(value1<value2) return -1;
      if(value1>value2) return 1;
    }	
  }
  return 0;
}

/**
 * Zjist� jestli dva znaky �et�zce jsou CH.
 * @param str �et�zec.
 * @return Pravdivostn� hodnotu.
 */
bool isCH(const char *str)
{
  return ((str[0]=='c'||str[0]=='C')&&(str[1]=='h'||str[1]=='H'));	
}

/**
 * Vytiskne �et�zec do souboru.
 * @param str �et�zec
 * @param stream Soubor.
 * @return K�d chyby.
 */
int printString(void * str,FILE *stream)
{
  if(fprintf(stream,"%s\n",(char*)str)<0) return EPRINT;
  return EOK;	
}

/**
 * Porovn� dva �et�zce.
 * @param str1 Prvni �et�zec.
 * @param str2 Druh� �et�zec.
 * @return ��slo indikuj�c� vztah mezi �et�zci.
 */
int cmpString(const void *str1,const void*str2)
{
  return strCmpCZ((char*)str1,(char *)str2);
}
/* konec proj4string.c */
