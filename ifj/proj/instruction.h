/*
 * Soubor:   instruction.h
 * Resitele: xbartl03, xfryzp00, xhorak34, xstane31
 * Projekt:  P�eklada� jazyka IFJ11
 * Popis:    Deklarace funkc� pro pr�ci s instrukcemi pou�it�mi v seznamu instrukc�
 *           + pot�ebn� struktury
 */

/**
 * V��et typu instrukc�
 */
typedef enum InstrType {        //POZOR, pou�ito p�i indexov�n�
  IPUSH,                        /**< vlo�� operand instrukce na z�sobn�k */
  IPOP,                         /**< ulo�� hodnotu ze z�sobn�ku do operandu instrukce(prom�nn�) */
  IREAD,                        /**< p�e�te hodnotu ze vstupu a ulo�� na z�sobn�k */
  IWRITE,                       /**< vyp�e hodnotu na z�sobn�ku */
  IMARK,                        /**< vlo�� zna�ku na vrchol z�sobn�ku, kam se ulo�� adresa instrukce ICALL */
  ICALL,                        /**< vol� funkci dle operandu instrukce, vol� se po funkci IMARK */
  IRET,                         /**< n�vrat z funkce po p�edchoz�m vol�n� instrukce ICALL */
  IJMPZ,                        /**< podm�n�n� skok, pokud je hodnota na z�sobn�ku false nebo nil IJMP */
  IJMP,                         /**< nepodm�n�n� skok, skok na instrukci v operandu instrukce */
  IHALT,                        /**< instrukce pro konec programu */
  IADD,                         /**< oper�tor + */
  ISUB,                         /**< oper�tor - */
  IMUL,                         /**< oper�tor * */
  IDIV,                         /**< oper�tor / */
  IPOW,                         /**< oper�tor ^ */
  ICMP_EQ,                      /**< oper�tor == */
  ICMP_NEQ,                     /**< oper�tor ~= */
  ICMP_GREATER,                 /**< oper�tor > */
  ICMP_LESS,                    /**< oper�tor < */
  ICMP_GREATER_EQ,              /**< oper�tor >= */
  ICMP_LESS_EQ,                 /**< oper�tor <= */
  IAPPEND,                      /**< oper�tor .. */
  ITYPE,                        /**< vestav�n� funkce type() */
  ISUBSTR,                      /**< vestav�n� funkce substr() */
  IFIND,                        /**< vestav�n� funkce find() */
  ISORT                         /**< vestav�n� funkce sort() */
} InstrType;

/**
 * Struktura pro instrukci
 */
typedef struct Instr {
  InstrType type;               /**< typ instrukce */
  void *op;                     /**< operand */
} Instr;

/**
 * Vytvo�� nov� alokovan� objekt typu Instr.
 * V p��pad� chyby alokace vrac� NULL.
 * @param t Typ instrukce
 * @param op Operand instrukce
 * @return Nov� vytvo�en� objekt
 */
Instr *newInstr(InstrType t, void *op);

/**
 * Vytvo�� kopii instrukce.
 * V p��pad� chyby alokace vrac� NULL.
 * @param i Instrukce
 * @return Ukazatel na zkop�rovanou instrukci
 */
Instr *copyInstr(Instr * i);

/**
 * Sma�e instrukci.
 * @param i Instrukce
 */
void freeInstr(Instr * i);
