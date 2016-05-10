/*
 * Soubor:   error.h
 * Resitele: xbartl03, xfryzp00, xhorak34, xstane31
 * Projekt:  P�eklada� jazyka IFJ11
 * Popis:    Deklarace v��tu chyb.
 */

/**
 * V��et chyb
 */
typedef enum ErrorType {
  EOK,                          /**< Bez chyby */
  LEX_ERR = -1,                 /**< Lexik�ln� chyba */
  SYNT_ERR = -2,                /**< Syntakrick� chyba */
  SEM_ERR = -3,                 /**< S�mantick� chyba */
  RUN_ERR = -4,                 /**< Chyba interpretace */
  INTER_ERR = -5,               /**< Vnit�n� chyba */
  EALLOC = -6
} ErrorType;
