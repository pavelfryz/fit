/*
 * Soubor:  proj1.c
 * Datum:   2010/10/24
 * Autor:   Pavel Fr�z, xfryzp00@fit.vutbr.cz
 * Projekt: Jednoduch� komprese textu, projekt �. 1 pro p�edm�t IZP
 * Popis:   Program prov�d� komprimaci a dekomprimaci vstupn�ho textu.
 *          P�i komprimaci hled� opakuj�c� se bloky o N znac�ch, kter�
 *          pak nap�e ve tvaru: ��slo vyjad�uj�c� po�et blok� n�sledovan�
 *          t�mto blokem. Bez parametru --extra je toto ��slo jednocifern�.
 *          P�i dekomprimaci vrac� text do p�vodn� podoby.
 *          Komprimovan� text m��e obsahovat tisknuteln� a interpunk�n� znaky
 *          bez diakritiky krom� ��sel.
 */

// pr�ce se vstupem/v�stupem
#include <stdio.h>
// obecn� funkce jazyka C
#include <stdlib.h>
// kv�li funkci strcmp
#include <string.h>
// testov�n� znak� - isalpha, isdigit, atd.
#include <ctype.h>
// limity ��seln�ch typ�
#include <limits.h>
// typ bool, konstanty true, false
#include <stdbool.h>
// prom�nn� errno
#include <errno.h>

#define MAX_CIFRA 9

/** K�dy chyb programu */
enum tecodes
{
  EOK = 0,     /**< Bez chyby */
  ECLWRONG,    /**< Chybn� p��kazov� ��dek. */
  EINMSG,      /**< Chybn� vstupn� �et�zec. */
  EOUT,        /**< Chyba v�stupu. */
  EDIGIT1,     /**< Chybn� form�t ��sla. */
  EDIGIT2,     /**< Mimo rozsah unsigned nebo 0. */
  EALLOC,      /**< Chyba pam�ti */

  EUNKNOWN,    /**< Nezn�m� chyba */
};

/** Stavov� k�dy programu */
enum tstates
{
  CHELP,        /**< N�pov�da */
  CCOMP,        /**< Komprimovat. */
  CDECOMP,      /**< Dekomprimovat. */
  CECOMP,       /**< Extra - Komprimovat. */
  CEDECOMP,     /**< Extra - Dekomprimovat. */
};

/** Chybov� hl�en� odpov�daj�c� chybov�m k�d�m. */
const char *ECODEMSG[] =
{
  /* EOK */
  "V�e v po��dku.\n",
  /* ECLWRONG */
  "Chybn� parametry p��kazov�ho ��dku!\n",
  /* EINMSG */
  "Chybn� vstupn� �et�zec!\n",
  /* EOUT */
  "Chyba z�pisu! Nepoda�ilo se zapsat v�echny znaky.\n",
  /* EDIGIT1 */
  "Chyba! Parametr N nen� ��slo.\n",
  /* EDIGIT2 */
  "Chyba! Parametr N je mimo rozsah (unsigned int \\ 0).\n",
  /* EALLOC */
  "Chyba alokace! Nedostatek pam�ti.\n",

  /* EUNKNOWN */
  "Nastala nep�edv�dan� chyba! Vypn�te po��ta� a rychle ute�te.\n",
};

const char *HELPMSG =
  "Program Jednoduch� komprese.\n"
  "Autor: Pavel Fr�z (c) 2010\n"
  "Program prov�d� komprimaci a dekomprimaci vstupn�ho textu.\n"
  "P�i komprimaci hled� opakuj�c� se bloky o N znac�ch, kter�\n"
  "pak nap�e ve tvaru: ��slo vyjad�uj�c� po�et blok� n�sledovan�\n"
  "t�mto blokem. Bez parametru --extra je toto ��slo jednocifern�.\n"
  "P�i dekomprimaci vrac� text do p�vodn� podoby.\n"
  "Komprimovan� text m��e obsahovat tisknuteln� a interpunk�n� znaky\n"
  "bez diakritiky krom� ��sel.\n"
  "Pou�it�: proj1 -h\n"
  "         proj1 -c N\n"
  "         proj1 -d N\n"
  "         proj1 --extra -c N\n"
  "         proj1 --extra -d N\n"
  "Popis parametr�:\n"
  "-h Vytiskne n�pov�du\n"
  "-c Komprimace textu\n"
  "-d Dekomprimace textu\n"
  "N Po�et znak� bloku\n"
  "--extra Po�et blok� nemus� b�t jednocifern�\n";

/**
 * Struktura obsahuj�c� hodnoty parametr� p��kazov� ��dky.
 */
typedef struct params
{
  unsigned int N;   /**< Hodnota N z p��kazov� ��dky. */
  int ecode;        /**< Chybov� k�d programu, odpov�d� v��tu tecodes. */
  int state;        /**< Stavov� k�d programu, odpov�d� v��tu tstates. */
} TParams;

/**
* Struktura obsahuj�c� pr�v� zpracov�van� znaky
*/
typedef struct buffer
{
  char *field;              /**< Pole znak�. */
  unsigned int pointer;     /**< Index prvn�ho prvku pole. */
  unsigned int size;        /**< Velikost pole. */
} TBuffer;

/////////////////////////////////////////////////////////////////
void printECode(int ecode);
TParams getParams(int argc, char *argv[]);

unsigned int atou(const char * str);
int putInt(unsigned int i);

TBuffer initBuf(unsigned int size);
void deinitBuf(TBuffer * buffer);
unsigned int getNChar(TBuffer * destination,unsigned int N);
unsigned int putNChar(TBuffer source,unsigned int position,unsigned int N);
TBuffer bufNCpy(TBuffer source,unsigned int N);

int comp(unsigned int N,bool extra);
int checkPrintBuffer(TBuffer buffer,int *cequal,int *print,bool extra);

int decomp(unsigned int N,bool extra);
/////////////////////////////////////////////////////////////////

/**
 * Vytiskne hl�en� odpov�daj�c� chybov�mu k�du.
 * @param ecode k�d chyby programu
 */
void printECode(int ecode)
{
  if (ecode < EOK || ecode > EUNKNOWN)
  {
    ecode = EUNKNOWN;
  }

  fprintf(stderr, "%s", ECODEMSG[ecode]);
}

/**
 *  Vytiskne neznam�nkov� ��slo.
 *  @param i Neznam�nkov� ��slo.
 *  @return Vrac� ��slo chyby.
 */
int putInt(unsigned int i)
{
  if (i / 10 != 0)
    if(putInt(i/10)==EOUT)
      return EOUT;
  if(putchar('0'+i%10)==EOF)
    return EOUT;
  return EOK;
}

/**
 *  P�evede �et�zec na neznam�nkov� ��slo.
 *  @param str �et�zec.
 *  @return Vrac� p�eveden� ��slo.
 */
unsigned int atou(const char * str)
{
  if(str[0]=='-'&&isdigit(str[1]))
  {
    errno = ERANGE;
    return 0;
  }
  else if(!isdigit(str[0]))
  {
    errno = EINVAL;
    return 0;
  }
  char *ptr=NULL;
  unsigned long int ul = strtoul(str,&ptr,10);
  if(*ptr!='\0')
  {
    errno = EINVAL;
    return 0;
  }	  
  if(ul>UINT_MAX)
  {
    errno = ERANGE;
    return UINT_MAX;
  }
  return ul;
}

/**
 * Inicializuje buffer a vr�t� ho ve struktu�e TBuffer.
 * Pokud se nepoda�� alokovat pam�, ukon�� program s chybov�m k�dem.
 * @param size Po�et znak�, velikost bufferu.
 * @return Vrac� inicializovan� buffer.
 */
TBuffer initBuf(unsigned int size)
{
  TBuffer buf =
  { // inicializace struktury
    .field = (char *) malloc(sizeof(char)*size),
    .pointer = 0,
    .size = size,
  };
  if(buf.field==NULL||size==0)
  { // chyba alokace
    printECode(EALLOC);
    exit(EXIT_FAILURE);
  }
  return buf;
}

/**
 * Deinicializuje buffer.
 * @param buffer Buffer.
 */
void deinitBuf(TBuffer * buffer)
{
  free(buffer->field);
  buffer->pointer=0;
  buffer->size=0;
}

/**
 * Na�te N znak� ze standartn�ho vstupu do c�lov�ho bufferu.
 * @param destination C�lov� buffer.
 * @param N Po�et znak�.
 * @return Vrac� po�et na�ten�ch znak�.
 */
unsigned int getNChar(TBuffer * destination,unsigned int N)
{
  unsigned int count;
  for(count=0; count<N; count++)
  {
    int c=getchar();
    if(c==EOF)
      return count;
    if(!(isprint(c)||isspace(c))||isdigit(c))
      errno=EINMSG;
    destination->field[destination->pointer]=c;
    destination->pointer = (destination->pointer+1)%destination->size;
  }
  return count;
}

/**
 * Vyp�e N znak� na standartn� v�stup ze zdrojov�ho buffer od ur�en� pozice.
 * @param source Zdrojov� buffer.
 * @param position Pozice.
 * @param N Po�et znak�.
 * @return Vrac� po�et vypsan�ch znak�.
 */
unsigned int putNChar(TBuffer source,unsigned int position,unsigned int N)
{
  unsigned int count;
  for(count=0; count<N; count++)
  {
    if(putchar(source.field[(position+count)%source.size])==EOF)
      return count;
  }
  return count;
}

/**
 * Zkop�ruje N znak� ze zdrojov�ho bufferu
 * do nov�ho bufferu, kter� vr�t� ve struktu�e TBuffer.
 * @param source Zdrojov� buffer.
 * @param N Po�et znak�
 * @return Vrac� napln�n� buffer o N znac�ch.
 */
TBuffer bufNCpy(TBuffer source,unsigned int N)
{
  TBuffer buffer=initBuf(N);
  unsigned int count;
  for(count=0; count<N; count++)
  {
    buffer.field[count]=source.field[(source.pointer+count)%source.size];
  }
  return buffer;
}

/**
 * Zpracuje argumenty p��kazov�ho ��dku a vr�t� je ve struktu�e TParams.
 * Pokud je form�t argument� chybn�, ukon�� program s chybov�m k�dem.
 * @param argc Po�et argument�.
 * @param argv Pole textov�ch �et�zc� s argumenty.
 * @return Vrac� analyzovan� argumenty p��kazov�ho ��dku.
 */
TParams getParams(int argc, char *argv[])
{
  TParams result =
  { // inicializace struktury
    .N = 0,
    .ecode = EOK,
    .state = CCOMP,
  };
  if (argc == 2 && strcmp("-h", argv[1]) == 0)
  { // tisk n�pov�dy
    result.state = CHELP;
  }
  else if (argc == 3)
  { // dva parametry
    if (strcmp("-c",argv[1]) == 0)
    { // koprese
      result.state = CCOMP;
    }
    else if (strcmp("-d",argv[1]) == 0)
    { // dekomprese
      result.state = CDECOMP;
    }
    else
    { // nezn�m� parametr
      result.ecode = ECLWRONG;
      return result;
    }
    errno = 0;
    result.N = atou(argv[2]);
    if (errno==EINVAL)
    { // chyba N spatny format
      result.ecode = EDIGIT1;
    }
    else if(result.N==0||errno)
    { // chyba N je mimo rozsah (unsigned int - 0)
      result.ecode = EDIGIT2;
    }
  }
  else if(argc==4)
  { // t�i parametry
    if (strcmp("--extra",argv[1]) == 0)
    { // extra
      if (strcmp("-c",argv[2]) == 0)
      { // extra koprese
        result.state = CECOMP;
      }
      else if (strcmp("-d",argv[2]) == 0)
      { // extra dekomprese
        result.state = CEDECOMP;
      }
      else
      { // nezn�m� parametr
        result.ecode = ECLWRONG;
        return result;
      }
      errno = 0;
      result.N = atou(argv[3]);
      if (errno==EINVAL)
      { // chyba N spatny format
        result.ecode = EDIGIT1;
      }
      else if(result.N==0||errno)
      { // chyba N je mimo rozsah (unsigned int - 0)
        result.ecode = EDIGIT2;
      }
    }
    else
    { // nezn�m� parametr
      result.ecode = ECLWRONG;
    }
  }
  else
  { // p��li� mnoho parametr�
    result.ecode = ECLWRONG;
  }

  return result;
}

/**
 * Komprimuje vstupn� text.
 * Hled� opakuj�c� se bloky znak� d�lky N.
 * D�le program p�i nalezen� minim�ln� 2 a maxim�ln� 9
 * (v�ce p�i mo�nosti extra)
 * shodn�ch blok� za sebou zap�e tuto posloupnost
 * ve tvaru ��slo ud�vaj�c� po�et opakov�n� + tento blok.
 * @param N D�lka bloku znak�.
 * @param extra Mo�nost extra.
 * @return Vrac� ��slo chyby.
 */
int comp(unsigned int N,bool extra)
{
  TBuffer buffer = initBuf(N+1);
  unsigned int count;
  errno=EOK;
  if((count=getNChar(&buffer,N))!=N)
  { //m�n� znak� ne� d�lka opakuj�c�ho se �et�zce
    if(errno==EINMSG)
    { //chyba vstupu
      deinitBuf(&buffer);
      return EINMSG;
    }
    if(putNChar(buffer,0,count)!=count)
    { //chyba vystupu
      deinitBuf(&buffer);
      return EOUT;
    }
    deinitBuf(&buffer);
    return EOK;
  }
  if(errno==EINMSG)
  { //chyba vstupu
    deinitBuf(&buffer);
    return EINMSG;
  }
  int cequal=0;
  int print=0;
  errno=EOK;
  while((getNChar(&buffer,1))==1)
  { //zpracov�v�n� znak� a� po konec vstupu
    if(errno==EINMSG)
    { //chyba vstupu
      deinitBuf(&buffer);
      return EINMSG;
    }
    if(buffer.field[buffer.pointer]==buffer.field[(buffer.size+buffer.pointer-1)%buffer.size])
    { //shoda znak� vzd�len�ch N znak�
      if(print>0)
        print--;
      else
        cequal++;
    }
    else
    { //v�pis znak�
      int errcode=EOK;
      if((errcode=checkPrintBuffer(buffer,&cequal,&print,extra))!=EOK)
      { //chyba pri vypisu znaku
        deinitBuf(&buffer);
        return errcode;
      }
    }
    errno=EOK;
  }
  //vypsan� posledn�ch znak� v buffer
  int errcode=EOK;
  if(cequal)
  {
    buffer.pointer=(buffer.pointer+1)%buffer.size;
    if((errcode=checkPrintBuffer(buffer,&cequal,&print,extra))!=EOK)
    { //chyba pri vypisu znaku
      deinitBuf(&buffer);
      return errcode;
    }
  }
  else
  {
    unsigned int pointer=(buffer.pointer+print+1)%buffer.size;
    unsigned int size=buffer.size-(print+1);
    if(putNChar(buffer,pointer,size)!=size)
    { //chyba vystupu
      deinitBuf(&buffer);
      return EOUT;
    }
  }
  deinitBuf(&buffer);
  return EOK;
}

/**
 * Funkce vyhodnot� argumenty a podle toho
 * vytiskne znaky ze zdrojov�ho bufferu.
 * @param buffer Zdrojov� buffer.
 * @param cequal Po�et shodn�ch znak�
 * @param print Po�et znak�, kter� se namaj� tisknout
 * @param extra Mo�nost extra.
 * @return Vrac� ��slo chyby.
 */
int checkPrintBuffer(TBuffer buffer,int *cequal,int *print,bool extra)
{
  if(*print>0)
  {
    *print-=1;;
    return EOK;
  }

  if(*cequal==0)
  { //��dn� shoda, vytiskne nejstar�� znak
    if(putNChar(buffer,buffer.pointer,1)!=1)
      return EOUT;
    return EOK;
  }

  TBuffer cpy=bufNCpy(buffer,buffer.size-1);
  int move=*cequal%cpy.size;
  int count=*cequal/cpy.size;
  if(move)
  { //nastaveni ukazatele na spravne misto
    cpy.pointer=(cpy.pointer+cpy.size-move)%cpy.size;
  }
  if(count)
  { //shoda, vytiskne ��slo a blok znak�
    *print=cpy.size;
    if(extra)
    {
      if(putInt(count+1)==EOUT)
      { //chyba vystupu
        deinitBuf(&cpy);
        return EOUT;
      }
      if(putNChar(cpy,cpy.pointer,cpy.size)!=cpy.size)
      { //chyba vystupu
        deinitBuf(&cpy);
        return EOUT;
      }
    }
    else do {
        if(count/MAX_CIFRA)
        {
          if(putchar('0'+MAX_CIFRA)==EOF)
          { //chyba vystupu
            deinitBuf(&cpy);
            return EOUT;
          }
          if(putNChar(cpy,cpy.pointer,cpy.size)!=cpy.size)
          { //chyba vystupu
            deinitBuf(&cpy);
            return EOUT;
          }
        }
        else if (count==0)
        {
          if(putNChar(cpy,cpy.pointer,cpy.size)!=cpy.size)
          { //chyba vystupu
            deinitBuf(&cpy);
            return EOUT;
          }
        }
        else
        {
          if(putchar('0'+count%MAX_CIFRA+1)==EOF)
          { //chyba vystupu
            deinitBuf(&cpy);
            return EOUT;
          }
          if(putNChar(cpy,cpy.pointer,cpy.size)!=cpy.size)
          { //chyba vystupu
            deinitBuf(&cpy);
            return EOUT;
          }
        }
        count-=MAX_CIFRA;
      } while (count>=0);
  }
  if(move>0)
  {
    if(putNChar(cpy,cpy.pointer,move)!=(unsigned)move)
    { //chyba vystupu
      deinitBuf(&cpy);
      return EOUT;
    }
  }
  if(*print>0)
  {
    *print-=1;;
  }
  else
  {
    if(putNChar(buffer,buffer.pointer,1)!=1)
    { //chyba vystupu
      deinitBuf(&cpy);
      return EOUT;
    }
  }
  *cequal=0;
  deinitBuf(&cpy);
  return EOK;
}


/**
 * Dekomprimuje vstupn� text.
 * Op�e vstupn� znaky na v�stup. Pokud naraz� na ��slici
 * (na ��slo p�i extra nastaven�),
 * zopakuje n�sleduj�c�ch N znak� tolikr�t, kolik ud�v� ��slice.
 * @param N D�lka bloku znak�.
 * @param extra Mo�nost extra.
 * @return Vrac� ��slo chyby.
 */
int decomp(unsigned int N,bool extra)
{
  int c;
  while((c=getchar())!=EOF)
  {
    if(isdigit(c))
    { //��slice
      TBuffer buffer = initBuf(N);
      int count=c-'0';  // p�evod ��slice na ��slo
      unsigned int n=N;
      if(extra)
      {
        while(isdigit(c=getchar()))
        {
          count=count*10+c-'0';
        }
        if(c==EOF||!(isprint(c)||isspace(c)))
        { //chyba vstupu
          deinitBuf(&buffer);
          return EINMSG;
        }
        buffer.field[buffer.pointer]=c;
        buffer.pointer=(buffer.pointer+1)%buffer.size;
        n--;
      }
      errno=EOK;
      if((getNChar(&buffer,n))!=n||errno==EINMSG)
      { //chyba vstupu
        deinitBuf(&buffer);
        return EINMSG;
      }
      if(putNChar(buffer,0,count*buffer.size)!=count*buffer.size)
      { //chyba vystupu
        deinitBuf(&buffer);
        return EOUT;
      }
      deinitBuf(&buffer);
    }
    else if(isprint(c)||isspace(c))
    { //tisknutelny znak
      if(putchar(c)==EOF)
        return EOUT;
    }
    else
    { //chyba vstupu
      return EINMSG;
    }
  }
  return EOK;
}


/////////////////////////////////////////////////////////////////
/**
 * Hlavn� program.
 */
int main(int argc, char *argv[])
{
  TParams params = getParams(argc, argv);
  if (params.ecode != EOK)
  { // n�co nestandardn�ho
    printECode(params.ecode);
    return EXIT_FAILURE;
  }

  int errcode=EOK;
  if (params.state == CHELP)
  { // tisk n�pov�dy
    printf("%s", HELPMSG);
    return EXIT_SUCCESS;
  }
  else if (params.state == CDECOMP)
  { // dekomprese
    errcode = decomp(params.N,false);
  }
  else if (params.state == CCOMP)
  { // komprese
    errcode = comp(params.N, false);
  }
  else if (params.state == CEDECOMP)
  { // extra dekomprese
    errcode = decomp(params.N, true);
  }
  else if (params.state == CECOMP)
  { // extra komprese
    errcode = comp(params.N, true);
  }

  if (errcode != EOK)
  { // n�co nestandardn�ho
    printECode(errcode);
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}

/* konec proj1.c */
