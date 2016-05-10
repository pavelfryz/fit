/*
 * Soubor:   instruction.h
 * Resitele: xbartl03, xfryzp00, xhorak34, xstane31
 * Projekt:  Pøekladaè jazyka IFJ11
 * Popis:    Deklarace funkcí pro práci s instrukcemi pou¾itými v seznamu instrukcí
 *           + potøebné struktury
 */

/**
 * Výèet typu instrukcí
 */
typedef enum InstrType {        //POZOR, pou¾ito pøi indexování
  IPUSH,                        /**< vlo¾í operand instrukce na zásobník */
  IPOP,                         /**< ulo¾í hodnotu ze zásobníku do operandu instrukce(promìnné) */
  IREAD,                        /**< pøeète hodnotu ze vstupu a ulo¾í na zásobník */
  IWRITE,                       /**< vypí¹e hodnotu na zásobníku */
  IMARK,                        /**< vlo¾í znaèku na vrchol zásobníku, kam se ulo¾í adresa instrukce ICALL */
  ICALL,                        /**< volá funkci dle operandu instrukce, volá se po funkci IMARK */
  IRET,                         /**< návrat z funkce po pøedchozím volání instrukce ICALL */
  IJMPZ,                        /**< podmínìný skok, pokud je hodnota na zásobníku false nebo nil IJMP */
  IJMP,                         /**< nepodmínìný skok, skok na instrukci v operandu instrukce */
  IHALT,                        /**< instrukce pro konec programu */
  IADD,                         /**< operátor + */
  ISUB,                         /**< operátor - */
  IMUL,                         /**< operátor * */
  IDIV,                         /**< operátor / */
  IPOW,                         /**< operátor ^ */
  ICMP_EQ,                      /**< operátor == */
  ICMP_NEQ,                     /**< operátor ~= */
  ICMP_GREATER,                 /**< operátor > */
  ICMP_LESS,                    /**< operátor < */
  ICMP_GREATER_EQ,              /**< operátor >= */
  ICMP_LESS_EQ,                 /**< operátor <= */
  IAPPEND,                      /**< operátor .. */
  ITYPE,                        /**< vestavìná funkce type() */
  ISUBSTR,                      /**< vestavìná funkce substr() */
  IFIND,                        /**< vestavìná funkce find() */
  ISORT                         /**< vestavìná funkce sort() */
} InstrType;

/**
 * Struktura pro instrukci
 */
typedef struct Instr {
  InstrType type;               /**< typ instrukce */
  void *op;                     /**< operand */
} Instr;

/**
 * Vytvoøí novì alokovaný objekt typu Instr.
 * V pøípadì chyby alokace vrací NULL.
 * @param t Typ instrukce
 * @param op Operand instrukce
 * @return Novì vytvoøený objekt
 */
Instr *newInstr(InstrType t, void *op);

/**
 * Vytvoøí kopii instrukce.
 * V pøípadì chyby alokace vrací NULL.
 * @param i Instrukce
 * @return Ukazatel na zkopírovanou instrukci
 */
Instr *copyInstr(Instr * i);

/**
 * Sma¾e instrukci.
 * @param i Instrukce
 */
void freeInstr(Instr * i);
