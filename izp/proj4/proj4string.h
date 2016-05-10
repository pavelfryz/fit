/*
 * Soubor:  proj4string.h
 * Datum:   12.12.2010
 * Autor:   Pavel Frýz, xfryzp00@stud.fit.vutbr.cz
 * Projekt: Èeské øazení, projekt è. 4 pro pøedmìt IZP
 * Popis:   Hlavièkovy soubor k souboru proj4string.c
 */

/** Kódy chyb */
enum tecodes
{
  EOK = 0,     /**< Bez chyby */
  ECLWRONG,    /**< Chybný pøíkazový øádek. */
  EFOPEN,      /**< Chyba, soubor nenalezen. */
  EFCREATE,    /**< Chyba, vytvareni souboru */
  EBADHEAD,    /**< Chyba, hlavicka souboru */  
  EBADVAL,     /**< Chyba, spatny format dat. */
  EALLOC,      /**< Chyba pamìti */
  EPRINT,      /**< Chyba pri zapisu */
  EUNKNOWN,    /**< Neznámá chyba */
};

/**
 * Pøeète øádek souboru do bufferu.
 * @param buffer Buffer.
 * @param stream Soubor.
 * @return Kód chyby nebo EOF.
 */
int readLine(char **buffer, FILE *stream);

/**
 * Porovná dva øetìzce.
 * @param str1 Prvni øetìzec.
 * @param str2 Druhý øetìzec.
 * @return Èíslo indikující vztah mezi øetìzci.
 */
int strCmpCZ(const char *str1,const char *str2);

/**
 * Zjistí jestli dva znaky øetìzce jsou CH.
 * @param str Øetìzec.
 * @return Pravdivostní hodnotu.
 */
bool isCH(const char *str);

/**
 * Vytiskne øetìzec do souboru.
 * @param str Øetìzec
 * @param stream Soubor.
 * @return Kód chyby.
 */
int printString(void * str,FILE *stream);

/**
 * Porovná dva øetìzce.
 * @param str1 Prvni øetìzec.
 * @param str2 Druhý øetìzec.
 * @return Èíslo indikující vztah mezi øetìzci.
 */
int cmpString(const void *str1,const void*str2);


static const unsigned char cmpTablePrim[256]=
{
    [0x00]=0, [0x20]=0, [0xA0]=0, /*'\0',mezera,pevna mezera*/
    [0x61]=1, /*a*/ [0x41]=1, /*A*/ [0xE1]=1,/*á*/ [0xC1]=1,/*Á*/
    [0x62]=3, /*b*/ [0x42]=3, /*B*/
    [0x63]=4, /*c*/ [0x43]=4, /*C*/ [0xE8]=5,/*è*/[0xC8]=5,/*È*/  
    [0x64]=6, /*d*/ [0x44]=6, /*D*/ [0xEF]=6,/*ï*/[0xCF]=6,/*Ï*/
    [0x65]=8, /*e*/ [0x45]=8, /*E*/ [0xE9]=8,/*é*/[0xC9]=8,/*É*/[0xEC]=8,/*ì*/[0xCC]=8,/*Ì*/
    [0x66]=11,/*f*/ [0x46]=11,/*F*/ 
    [0x67]=12,/*g*/ [0x47]=12,/*G*/
    [0x68]=13,/*h*/ [0x48]=13,/*H*/
    /* 14 vynechano pro znak CH */
    [0x69]=15,/*i*/ [0x49]=15,/*I*/ [0xED]=15,/*í*/[0xCD]=15,/*Í*/
    [0x6A]=17,/*j*/ [0x4A]=17,/*J*/
    [0x6B]=18,/*k*/ [0x4B]=18,/*K*/
    [0x6C]=19,/*l*/ [0x4C]=19,/*L*/
    [0x6D]=20,/*m*/ [0x4D]=20,/*M*/
    [0x6E]=21,/*n*/ [0x4E]=21,/*N*/ [0xF2]=21,/*ò*/[0xD2]=21,/*Ò*/
    [0x6F]=23,/*o*/ [0x4F]=23,/*O*/ [0xF3]=23,/*ó*/[0xD3]=23,/*Ó*/
    [0x70]=25,/*p*/ [0x50]=25,/*P*/
    [0x71]=26,/*q*/ [0x51]=26,/*Q*/
    [0x72]=27,/*r*/ [0x52]=27,/*R*/ [0xF8]=28,/*ø*/[0xD8]=28,/*Ø*/
    [0x73]=29,/*s*/ [0x53]=29,/*S*/ [0xB9]=30,/*¹*/[0xA9]=30,/*©*/
    [0x74]=31,/*t*/ [0x54]=31,/*T*/ [0xBB]=31,/*»*/[0xAB]=31,/*«*/
    [0x75]=33,/*u*/ [0x55]=33,/*U*/ [0xFA]=33,/*ú*/[0xDA]=33,/*Ú*/[0xF9]=33,/*ù*/[0xD9]=33,/*Ù*/
    [0x76]=36,/*v*/ [0x56]=36,/*V*/ 
    [0x77]=37,/*w*/ [0x57]=37,/*W*/
    [0x78]=38,/*x*/ [0x58]=38,/*X*/
    [0x79]=39,/*y*/ [0x59]=39,/*Y*/ [0xFD]=39,/*ý*/[0xDD]=39,/*Ý*/
    [0x7A]=41,/*z*/ [0x5A]=41,/*Z*/ [0xBE]=42,/*¾*/[0xAE]=42,/*®*/

    [0x30]=43,/*0*/ [0x31]=44,/*1*/ [0x32]=45,/*2*/
    [0x33]=46,/*3*/ [0x34]=47,/*4*/ [0x35]=48,/*5*/
    [0x36]=49,/*6*/ [0x37]=50,/*7*/ [0x38]=51,/*8*/
    [0x39]=52,/*9*/

    [0x2E]=53,/*.*/ [0x2C]=54,/*,*/ [0x3B]=55,/*;*/
    [0x3F]=56,/*?*/ [0x21]=57,/*!*/ [0x3A]=58,/*:*/
    [0x22]=59,/*"*/ [0x2D]=60,/*-*/ [0x7C]=61,/*|*/
    [0x2F]=62,/*/*/ [0x5C]=63,/*\*/ [0x28]=64,/*(*/
    [0x29]=65,/*)*/ [0x5B]=66,/*[*/ [0x5D]=67,/*]*/
    [0x3C]=68,/*<*/ [0x3E]=69,/*>*/ [0x7B]=70,/*{*/
    [0x7D]=71,/*}*/ [0x26]=72,/*&*/ [0x25]=73,/*%*/
    [0x24]=74,/*$*/ [0x3D]=75,/*=*/ [0x2B]=76,/*+*/
    [0x2A]=77,/***/ [0x23]=78,/*#*/ [0x7E]=79,/*~*/
};
static const unsigned char cmpTableSec[256]=
{
    [0x00]=0, [0x20]=0, [0xA0]=0, /*'\0',mezera,pevna mezera*/
    [0x61]=1, /*a*/ [0x41]=1, /*A*/ [0xE1]=2,/*á*/ [0xC1]=2,/*Á*/
    [0x62]=3, /*b*/ [0x42]=3, /*B*/
    [0x63]=4, /*c*/ [0x43]=4, /*C*/ [0xE8]=5,/*è*/[0xC8]=5,/*È*/  
    [0x64]=6, /*d*/ [0x44]=6, /*D*/ [0xEF]=7,/*ï*/[0xCF]=7,/*Ï*/
    [0x65]=8, /*e*/ [0x45]=8, /*E*/ [0xE9]=9,/*é*/[0xC9]=9,/*É*/[0xEC]=10,/*ì*/[0xCC]=10,/*Ì*/
    [0x66]=11,/*f*/ [0x46]=11,/*F*/ 
    [0x67]=12,/*g*/ [0x47]=12,/*G*/
    [0x68]=13,/*h*/ [0x48]=13,/*H*/
    /* 14 vynechano pro znak CH */
    [0x69]=15,/*i*/ [0x49]=15,/*I*/ [0xED]=16,/*í*/[0xCD]=16,/*Í*/
    [0x6A]=17,/*j*/ [0x4A]=17,/*J*/
    [0x6B]=18,/*k*/ [0x4B]=18,/*K*/
    [0x6C]=19,/*l*/ [0x4C]=19,/*L*/
    [0x6D]=20,/*m*/ [0x4D]=20,/*M*/
    [0x6E]=21,/*n*/ [0x4E]=21,/*N*/ [0xF2]=22,/*ò*/[0xD2]=22,/*Ò*/
    [0x6F]=23,/*o*/ [0x4F]=23,/*O*/ [0xF3]=24,/*ó*/[0xD3]=24,/*Ó*/
    [0x70]=25,/*p*/ [0x50]=25,/*P*/
    [0x71]=26,/*q*/ [0x51]=26,/*Q*/
    [0x72]=27,/*r*/ [0x52]=27,/*R*/ [0xF8]=28,/*ø*/[0xD8]=28,/*Ø*/
    [0x73]=29,/*s*/ [0x53]=29,/*S*/ [0xB9]=30,/*¹*/[0xA9]=30,/*©*/
    [0x74]=31,/*t*/ [0x54]=31,/*T*/ [0xBB]=32,/*»*/[0xAB]=32,/*«*/
    [0x75]=33,/*u*/ [0x55]=33,/*U*/ [0xFA]=34,/*ú*/[0xDA]=34,/*Ú*/[0xF9]=35,/*ù*/[0xD9]=35,/*Ù*/
    [0x76]=36,/*v*/ [0x56]=36,/*V*/ 
    [0x77]=37,/*w*/ [0x57]=37,/*W*/
    [0x78]=38,/*x*/ [0x58]=38,/*X*/
    [0x79]=39,/*y*/ [0x59]=39,/*Y*/ [0xFD]=40,/*ý*/[0xDD]=40,/*Ý*/
    [0x7A]=41,/*z*/ [0x5A]=41,/*Z*/ [0xBE]=42,/*¾*/[0xAE]=42,/*®*/

    [0x30]=43,/*0*/ [0x31]=44,/*1*/ [0x32]=45,/*2*/
    [0x33]=46,/*3*/ [0x34]=47,/*4*/ [0x35]=48,/*5*/
    [0x36]=49,/*6*/ [0x37]=50,/*7*/ [0x38]=51,/*8*/
    [0x39]=52,/*9*/

    [0x2E]=53,/*.*/ [0x2C]=54,/*,*/ [0x3B]=55,/*;*/
    [0x3F]=56,/*?*/ [0x21]=57,/*!*/ [0x3A]=58,/*:*/
    [0x22]=59,/*"*/ [0x2D]=60,/*-*/ [0x7C]=61,/*|*/
    [0x2F]=62,/*/*/ [0x5C]=63,/*\*/ [0x28]=64,/*(*/
    [0x29]=65,/*)*/ [0x5B]=66,/*[*/ [0x5D]=67,/*]*/
    [0x3C]=68,/*<*/ [0x3E]=69,/*>*/ [0x7B]=70,/*{*/
    [0x7D]=71,/*}*/ [0x26]=72,/*&*/ [0x25]=73,/*%*/
    [0x24]=74,/*$*/ [0x3D]=75,/*=*/ [0x2B]=76,/*+*/
    [0x2A]=77,/***/ [0x23]=78,/*#*/ [0x7E]=79,/*~*/
};

/* konec proj4string.h */
