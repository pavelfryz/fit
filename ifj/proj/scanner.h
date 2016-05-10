/*
 * Soubor:   scanner.h
 * Resitele: xbartl03, xfryzp00, xhorak34, xstane31
 * Projekt:  P�eklada� jazyka IFJ11
 * Popis:    Hlavi�kov� soubor scanneru
 */

#include <stdio.h>

/**
 * V��et typu token�
 */
typedef enum TokenType {
  ID,                           /**< identifik�tor */
  STRING,                       /**< �et�zcov� liter�l */
  NUMBER,                       /**< ��seln� liter�l */
  IF,                           /**< Kl��ov� slovo if */
  THEN,                         /**< Kl��ov� slovo then */
  ELSE,                         /**< Kl��ov� slovo else */
  TRUE,                         /**< Kl��ov� slovo true */
  FALSE,                        /**< Kl��ov� slovo false */
  NIL,                          /**< Kl��ov� slovo nil */
  WHILE,                        /**< Kl��ov� slovo while */
  DO,                           /**< Kl��ov� slovo do */
  END,                          /**< Kl��ov� slovo end */
  FUNCTION,                     /**< Kl��ov� slovo function */
  LOCAL,                        /**< Kl��ov� slovo local */
  RETURN,                       /**< Kl��ov� slovo return */
  READ,                         /**< Kl��ov� slovo read */
  WRITE,                        /**< Kl��ov� slovo write */
  AND,                          /**< Rezervovan� slovo and */
  OR,                           /**< Rezervovan� slovo or */
  NOT,                          /**< Rezervovan� slovo not */
  BREAK,                        /**< Rezervovan� slovo break */
  ELSEIF,                       /**< Rezervovan� slovo elseif */
  FOR,                          /**< Rezervovan� slovo for */
  IN,                           /**< Rezervovan� slovo in */
  REPEAT,                       /**< Rezervovan� slovo repeat */
  UNTIL,                        /**< Rezervovan� slovo until */
  ASSIGN,                       /**< Oper�tor '=' */
  ADD,                          /**< Oper�tor '+' */
  SUB,                          /**< Oper�tor '-' */
  MUL,                          /**< Oper�tor '*' */
  DIV,                          /**< Oper�tor '/' */
  POW,                          /**< Oper�tor '^' */
  KONKATENACE,                  /**< Oper�tor '..' */
  EQ,                           /**< Oper�tor '==' */
  NEQ,                          /**< Oper�tor '~=' */
  LESS,                         /**< Oper�tor '<' */
  GREATER,                      /**< Oper�tor '>' */
  LESS_EQ,                      /**< Oper�tor '<=' */
  GREATER_EQ,                   /**< Oper�tor '>=' */
  LEFT_BRACKET,                 /**< Lev� z�vorka '(' */
  RIGHT_BRACKET,                /**< Prav� z�vorka')' */
  SEMICOLON,                    /**< St�edn�k ';' */
  COMMA,                        /**< ��rka',' */
  END_OF_FILE                   /**< Konec souboru */
} TokenType;

struct tString;

/**
 * Vr�t� token ze zdrojov�ho souboru.
 * @param s �et�zec
 * @param f Zdrojov� soubor
 * @return Typ tokenu
 */
int getToken(struct tString *s, FILE * f);
