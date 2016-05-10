/*
 * Soubor:   scanner.h
 * Resitele: xbartl03, xfryzp00, xhorak34, xstane31
 * Projekt:  Pøekladaè jazyka IFJ11
 * Popis:    Hlavièkový soubor scanneru
 */

#include <stdio.h>

/**
 * Výèet typu tokenù
 */
typedef enum TokenType {
  ID,                           /**< identifikátor */
  STRING,                       /**< øetìzcový literál */
  NUMBER,                       /**< èíselný literál */
  IF,                           /**< Klíèové slovo if */
  THEN,                         /**< Klíèové slovo then */
  ELSE,                         /**< Klíèové slovo else */
  TRUE,                         /**< Klíèové slovo true */
  FALSE,                        /**< Klíèové slovo false */
  NIL,                          /**< Klíèové slovo nil */
  WHILE,                        /**< Klíèové slovo while */
  DO,                           /**< Klíèové slovo do */
  END,                          /**< Klíèové slovo end */
  FUNCTION,                     /**< Klíèové slovo function */
  LOCAL,                        /**< Klíèové slovo local */
  RETURN,                       /**< Klíèové slovo return */
  READ,                         /**< Klíèové slovo read */
  WRITE,                        /**< Klíèové slovo write */
  AND,                          /**< Rezervované slovo and */
  OR,                           /**< Rezervované slovo or */
  NOT,                          /**< Rezervované slovo not */
  BREAK,                        /**< Rezervované slovo break */
  ELSEIF,                       /**< Rezervované slovo elseif */
  FOR,                          /**< Rezervované slovo for */
  IN,                           /**< Rezervované slovo in */
  REPEAT,                       /**< Rezervované slovo repeat */
  UNTIL,                        /**< Rezervované slovo until */
  ASSIGN,                       /**< Operátor '=' */
  ADD,                          /**< Operátor '+' */
  SUB,                          /**< Operátor '-' */
  MUL,                          /**< Operátor '*' */
  DIV,                          /**< Operátor '/' */
  POW,                          /**< Operátor '^' */
  KONKATENACE,                  /**< Operátor '..' */
  EQ,                           /**< Operátor '==' */
  NEQ,                          /**< Operátor '~=' */
  LESS,                         /**< Operátor '<' */
  GREATER,                      /**< Operátor '>' */
  LESS_EQ,                      /**< Operátor '<=' */
  GREATER_EQ,                   /**< Operátor '>=' */
  LEFT_BRACKET,                 /**< Levá závorka '(' */
  RIGHT_BRACKET,                /**< Pravá závorka')' */
  SEMICOLON,                    /**< Støedník ';' */
  COMMA,                        /**< Èárka',' */
  END_OF_FILE                   /**< Konec souboru */
} TokenType;

struct tString;

/**
 * Vrátí token ze zdrojového souboru.
 * @param s Øetìzec
 * @param f Zdrojový soubor
 * @return Typ tokenu
 */
int getToken(struct tString *s, FILE * f);
