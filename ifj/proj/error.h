/*
 * Soubor:   error.h
 * Resitele: xbartl03, xfryzp00, xhorak34, xstane31
 * Projekt:  Pøekladaè jazyka IFJ11
 * Popis:    Deklarace výètu chyb.
 */

/**
 * Výèet chyb
 */
typedef enum ErrorType {
  EOK,                          /**< Bez chyby */
  LEX_ERR = -1,                 /**< Lexikální chyba */
  SYNT_ERR = -2,                /**< Syntakrická chyba */
  SEM_ERR = -3,                 /**< Sémantická chyba */
  RUN_ERR = -4,                 /**< Chyba interpretace */
  INTER_ERR = -5,               /**< Vnitøní chyba */
  EALLOC = -6
} ErrorType;
