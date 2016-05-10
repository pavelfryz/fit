/*
 * Soubor:  proj4string.h
 * Datum:   12.12.2010
 * Autor:   Pavel Fr�z, xfryzp00@stud.fit.vutbr.cz
 * Projekt: �esk� �azen�, projekt �. 4 pro p�edm�t IZP
 * Popis:   Hlavi�kovy soubor k souboru proj4string.c
 */

/** K�dy chyb */
enum tecodes
{
  EOK = 0,     /**< Bez chyby */
  ECLWRONG,    /**< Chybn� p��kazov� ��dek. */
  EFOPEN,      /**< Chyba, soubor nenalezen. */
  EFCREATE,    /**< Chyba, vytvareni souboru */
  EBADHEAD,    /**< Chyba, hlavicka souboru */  
  EBADVAL,     /**< Chyba, spatny format dat. */
  EALLOC,      /**< Chyba pam�ti */
  EPRINT,      /**< Chyba pri zapisu */
  EUNKNOWN,    /**< Nezn�m� chyba */
};

/**
 * P�e�te ��dek souboru do bufferu.
 * @param buffer Buffer.
 * @param stream Soubor.
 * @return K�d chyby nebo EOF.
 */
int readLine(char **buffer, FILE *stream);

/**
 * Porovn� dva �et�zce.
 * @param str1 Prvni �et�zec.
 * @param str2 Druh� �et�zec.
 * @return ��slo indikuj�c� vztah mezi �et�zci.
 */
int strCmpCZ(const char *str1,const char *str2);

/**
 * Zjist� jestli dva znaky �et�zce jsou CH.
 * @param str �et�zec.
 * @return Pravdivostn� hodnotu.
 */
bool isCH(const char *str);

/**
 * Vytiskne �et�zec do souboru.
 * @param str �et�zec
 * @param stream Soubor.
 * @return K�d chyby.
 */
int printString(void * str,FILE *stream);

/**
 * Porovn� dva �et�zce.
 * @param str1 Prvni �et�zec.
 * @param str2 Druh� �et�zec.
 * @return ��slo indikuj�c� vztah mezi �et�zci.
 */
int cmpString(const void *str1,const void*str2);


static const unsigned char cmpTablePrim[256]=
{
    [0x00]=0, [0x20]=0, [0xA0]=0, /*'\0',mezera,pevna mezera*/
    [0x61]=1, /*a*/ [0x41]=1, /*A*/ [0xE1]=1,/*�*/ [0xC1]=1,/*�*/
    [0x62]=3, /*b*/ [0x42]=3, /*B*/
    [0x63]=4, /*c*/ [0x43]=4, /*C*/ [0xE8]=5,/*�*/[0xC8]=5,/*�*/  
    [0x64]=6, /*d*/ [0x44]=6, /*D*/ [0xEF]=6,/*�*/[0xCF]=6,/*�*/
    [0x65]=8, /*e*/ [0x45]=8, /*E*/ [0xE9]=8,/*�*/[0xC9]=8,/*�*/[0xEC]=8,/*�*/[0xCC]=8,/*�*/
    [0x66]=11,/*f*/ [0x46]=11,/*F*/ 
    [0x67]=12,/*g*/ [0x47]=12,/*G*/
    [0x68]=13,/*h*/ [0x48]=13,/*H*/
    /* 14 vynechano pro znak CH */
    [0x69]=15,/*i*/ [0x49]=15,/*I*/ [0xED]=15,/*�*/[0xCD]=15,/*�*/
    [0x6A]=17,/*j*/ [0x4A]=17,/*J*/
    [0x6B]=18,/*k*/ [0x4B]=18,/*K*/
    [0x6C]=19,/*l*/ [0x4C]=19,/*L*/
    [0x6D]=20,/*m*/ [0x4D]=20,/*M*/
    [0x6E]=21,/*n*/ [0x4E]=21,/*N*/ [0xF2]=21,/*�*/[0xD2]=21,/*�*/
    [0x6F]=23,/*o*/ [0x4F]=23,/*O*/ [0xF3]=23,/*�*/[0xD3]=23,/*�*/
    [0x70]=25,/*p*/ [0x50]=25,/*P*/
    [0x71]=26,/*q*/ [0x51]=26,/*Q*/
    [0x72]=27,/*r*/ [0x52]=27,/*R*/ [0xF8]=28,/*�*/[0xD8]=28,/*�*/
    [0x73]=29,/*s*/ [0x53]=29,/*S*/ [0xB9]=30,/*�*/[0xA9]=30,/*�*/
    [0x74]=31,/*t*/ [0x54]=31,/*T*/ [0xBB]=31,/*�*/[0xAB]=31,/*�*/
    [0x75]=33,/*u*/ [0x55]=33,/*U*/ [0xFA]=33,/*�*/[0xDA]=33,/*�*/[0xF9]=33,/*�*/[0xD9]=33,/*�*/
    [0x76]=36,/*v*/ [0x56]=36,/*V*/ 
    [0x77]=37,/*w*/ [0x57]=37,/*W*/
    [0x78]=38,/*x*/ [0x58]=38,/*X*/
    [0x79]=39,/*y*/ [0x59]=39,/*Y*/ [0xFD]=39,/*�*/[0xDD]=39,/*�*/
    [0x7A]=41,/*z*/ [0x5A]=41,/*Z*/ [0xBE]=42,/*�*/[0xAE]=42,/*�*/

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
    [0x61]=1, /*a*/ [0x41]=1, /*A*/ [0xE1]=2,/*�*/ [0xC1]=2,/*�*/
    [0x62]=3, /*b*/ [0x42]=3, /*B*/
    [0x63]=4, /*c*/ [0x43]=4, /*C*/ [0xE8]=5,/*�*/[0xC8]=5,/*�*/  
    [0x64]=6, /*d*/ [0x44]=6, /*D*/ [0xEF]=7,/*�*/[0xCF]=7,/*�*/
    [0x65]=8, /*e*/ [0x45]=8, /*E*/ [0xE9]=9,/*�*/[0xC9]=9,/*�*/[0xEC]=10,/*�*/[0xCC]=10,/*�*/
    [0x66]=11,/*f*/ [0x46]=11,/*F*/ 
    [0x67]=12,/*g*/ [0x47]=12,/*G*/
    [0x68]=13,/*h*/ [0x48]=13,/*H*/
    /* 14 vynechano pro znak CH */
    [0x69]=15,/*i*/ [0x49]=15,/*I*/ [0xED]=16,/*�*/[0xCD]=16,/*�*/
    [0x6A]=17,/*j*/ [0x4A]=17,/*J*/
    [0x6B]=18,/*k*/ [0x4B]=18,/*K*/
    [0x6C]=19,/*l*/ [0x4C]=19,/*L*/
    [0x6D]=20,/*m*/ [0x4D]=20,/*M*/
    [0x6E]=21,/*n*/ [0x4E]=21,/*N*/ [0xF2]=22,/*�*/[0xD2]=22,/*�*/
    [0x6F]=23,/*o*/ [0x4F]=23,/*O*/ [0xF3]=24,/*�*/[0xD3]=24,/*�*/
    [0x70]=25,/*p*/ [0x50]=25,/*P*/
    [0x71]=26,/*q*/ [0x51]=26,/*Q*/
    [0x72]=27,/*r*/ [0x52]=27,/*R*/ [0xF8]=28,/*�*/[0xD8]=28,/*�*/
    [0x73]=29,/*s*/ [0x53]=29,/*S*/ [0xB9]=30,/*�*/[0xA9]=30,/*�*/
    [0x74]=31,/*t*/ [0x54]=31,/*T*/ [0xBB]=32,/*�*/[0xAB]=32,/*�*/
    [0x75]=33,/*u*/ [0x55]=33,/*U*/ [0xFA]=34,/*�*/[0xDA]=34,/*�*/[0xF9]=35,/*�*/[0xD9]=35,/*�*/
    [0x76]=36,/*v*/ [0x56]=36,/*V*/ 
    [0x77]=37,/*w*/ [0x57]=37,/*W*/
    [0x78]=38,/*x*/ [0x58]=38,/*X*/
    [0x79]=39,/*y*/ [0x59]=39,/*Y*/ [0xFD]=40,/*�*/[0xDD]=40,/*�*/
    [0x7A]=41,/*z*/ [0x5A]=41,/*Z*/ [0xBE]=42,/*�*/[0xAE]=42,/*�*/

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
