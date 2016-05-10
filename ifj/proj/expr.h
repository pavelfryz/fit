/*
 * Soubor:   expr.h
 * Resitele: xbartl03, xfryzp00, xhorak34, xstane31
 * Projekt:  P�eklada� jazyka IFJ11
 * Popis:    Hlavi�ka pro zpracov�n� v�raz� precenden�n� syntaktickou
 *           anal�zou a vol�n� funkc�.
 */

/**
 * V��et zpracov�van�ch symbol�
 */
typedef enum PSymbol {
  PPOW,                         /**< Oper�tor '^' */
  PMUL,                         /**< Oper�tor '*' */
  PDIV,                         /**< Oper�tor '/' */
  PADD,                         /**< Oper�tor '+' */
  PSUB,                         /**< Oper�tor '-' */
  PKONKATENACE,                 /**< Oper�tor '..' */
  PEQ,                          /**< Oper�tor '==' */
  PNEQ,                         /**< Oper�tor '~=' */
  PLESS,                        /**< Oper�tor '<' */
  PGREATER,                     /**< Oper�tor '>' */
  PLESS_EQ,                     /**< Oper�tor '<=' */
  PGREATER_EQ,                  /**< Oper�tor '>=' */
  PLEFT_BRACKET,                /**< Lev� z�vorka '(' */
  PRIGHT_BRACKET,               /**< Prav� z�vorka')' */
  POPERAND,                     /**< Identifik�tor 'i' */
  PDOLLAR,                      /**< Dolar '$' */
  CLT,                          /**< Ridici '<' */
  CGT,                          /**< Ridici '>' */
  CEQ,                          /**< Ridici '=' */
  CER,                          /**< Prazdna bunka (chyba) */
  NONTERM,                      /**< Netermin�l */
} PSymbol;

/**
 * Zpracov�n� v�raz� precenden�n� syntaktickou anal�zou.
 * @return Chybov� k�d
 */
int expr();
