/*
 * Soubor:   expr.h
 * Resitele: xbartl03, xfryzp00, xhorak34, xstane31
 * Projekt:  Pøekladaè jazyka IFJ11
 * Popis:    Hlavièka pro zpracování výrazù precendenèní syntaktickou
 *           analýzou a volání funkcí.
 */

/**
 * Výèet zpracovávaných symbolù
 */
typedef enum PSymbol {
  PPOW,                         /**< Operátor '^' */
  PMUL,                         /**< Operátor '*' */
  PDIV,                         /**< Operátor '/' */
  PADD,                         /**< Operátor '+' */
  PSUB,                         /**< Operátor '-' */
  PKONKATENACE,                 /**< Operátor '..' */
  PEQ,                          /**< Operátor '==' */
  PNEQ,                         /**< Operátor '~=' */
  PLESS,                        /**< Operátor '<' */
  PGREATER,                     /**< Operátor '>' */
  PLESS_EQ,                     /**< Operátor '<=' */
  PGREATER_EQ,                  /**< Operátor '>=' */
  PLEFT_BRACKET,                /**< Levá závorka '(' */
  PRIGHT_BRACKET,               /**< Pravá závorka')' */
  POPERAND,                     /**< Identifikátor 'i' */
  PDOLLAR,                      /**< Dolar '$' */
  CLT,                          /**< Ridici '<' */
  CGT,                          /**< Ridici '>' */
  CEQ,                          /**< Ridici '=' */
  CER,                          /**< Prazdna bunka (chyba) */
  NONTERM,                      /**< Neterminál */
} PSymbol;

/**
 * Zpracování výrazù precendenèní syntaktickou analýzou.
 * @return Chybový kód
 */
int expr();
