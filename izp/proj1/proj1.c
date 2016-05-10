/*
 * Soubor:  proj1.c
 * Datum:   2010/10/24
 * Autor:   Pavel Frýz, xfryzp00@fit.vutbr.cz
 * Projekt: Jednoduchá komprese textu, projekt è. 1 pro pøedmìt IZP
 * Popis:   Program provádí komprimaci a dekomprimaci vstupního textu.
 *          Pøi komprimaci hledá opakující se bloky o N znacích, které
 *          pak napí¹e ve tvaru: èíslo vyjadøující poèet blokù následované
 *          tímto blokem. Bez parametru --extra je toto èíslo jednociferné.
 *          Pøi dekomprimaci vrací text do pùvodní podoby.
 *          Komprimovaný text mù¾e obsahovat tisknutelné a interpunkèní znaky
 *          bez diakritiky kromì èísel.
 */

// práce se vstupem/výstupem
#include <stdio.h>
// obecné funkce jazyka C
#include <stdlib.h>
// kvùli funkci strcmp
#include <string.h>
// testování znakù - isalpha, isdigit, atd.
#include <ctype.h>
// limity èíselných typù
#include <limits.h>
// typ bool, konstanty true, false
#include <stdbool.h>
// promìnná errno
#include <errno.h>

#define MAX_CIFRA 9

/** Kódy chyb programu */
enum tecodes
{
  EOK = 0,     /**< Bez chyby */
  ECLWRONG,    /**< Chybný pøíkazový øádek. */
  EINMSG,      /**< Chybný vstupní øetìzec. */
  EOUT,        /**< Chyba výstupu. */
  EDIGIT1,     /**< Chybný formát èísla. */
  EDIGIT2,     /**< Mimo rozsah unsigned nebo 0. */
  EALLOC,      /**< Chyba pamìti */

  EUNKNOWN,    /**< Neznámá chyba */
};

/** Stavové kódy programu */
enum tstates
{
  CHELP,        /**< Nápovìda */
  CCOMP,        /**< Komprimovat. */
  CDECOMP,      /**< Dekomprimovat. */
  CECOMP,       /**< Extra - Komprimovat. */
  CEDECOMP,     /**< Extra - Dekomprimovat. */
};

/** Chybová hlá¹ení odpovídající chybovým kódùm. */
const char *ECODEMSG[] =
{
  /* EOK */
  "V¹e v poøádku.\n",
  /* ECLWRONG */
  "Chybné parametry pøíkazového øádku!\n",
  /* EINMSG */
  "Chybný vstupní øetìzec!\n",
  /* EOUT */
  "Chyba zápisu! Nepodaøilo se zapsat v¹echny znaky.\n",
  /* EDIGIT1 */
  "Chyba! Parametr N není èíslo.\n",
  /* EDIGIT2 */
  "Chyba! Parametr N je mimo rozsah (unsigned int \\ 0).\n",
  /* EALLOC */
  "Chyba alokace! Nedostatek pamìti.\n",

  /* EUNKNOWN */
  "Nastala nepøedvídaná chyba! Vypnìte poèítaè a rychle uteète.\n",
};

const char *HELPMSG =
  "Program Jednoduchá komprese.\n"
  "Autor: Pavel Frýz (c) 2010\n"
  "Program provádí komprimaci a dekomprimaci vstupního textu.\n"
  "Pøi komprimaci hledá opakující se bloky o N znacích, které\n"
  "pak napí¹e ve tvaru: èíslo vyjadøující poèet blokù následované\n"
  "tímto blokem. Bez parametru --extra je toto èíslo jednociferné.\n"
  "Pøi dekomprimaci vrací text do pùvodní podoby.\n"
  "Komprimovaný text mù¾e obsahovat tisknutelné a interpunkèní znaky\n"
  "bez diakritiky kromì èísel.\n"
  "Pou¾ití: proj1 -h\n"
  "         proj1 -c N\n"
  "         proj1 -d N\n"
  "         proj1 --extra -c N\n"
  "         proj1 --extra -d N\n"
  "Popis parametrù:\n"
  "-h Vytiskne nápovìdu\n"
  "-c Komprimace textu\n"
  "-d Dekomprimace textu\n"
  "N Poèet znakù bloku\n"
  "--extra Poèet blokù nemusí být jednociferný\n";

/**
 * Struktura obsahující hodnoty parametrù pøíkazové øádky.
 */
typedef struct params
{
  unsigned int N;   /**< Hodnota N z pøíkazové øádky. */
  int ecode;        /**< Chybový kód programu, odpovídá výètu tecodes. */
  int state;        /**< Stavový kód programu, odpovídá výètu tstates. */
} TParams;

/**
* Struktura obsahující právì zpracovávané znaky
*/
typedef struct buffer
{
  char *field;              /**< Pole znakù. */
  unsigned int pointer;     /**< Index prvního prvku pole. */
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
 * Vytiskne hlá¹ení odpovídající chybovému kódu.
 * @param ecode kód chyby programu
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
 *  Vytiskne neznaménkové èíslo.
 *  @param i Neznaménkové èíslo.
 *  @return Vrací èíslo chyby.
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
 *  Pøevede øetìzec na neznaménkové èíslo.
 *  @param str Øetìzec.
 *  @return Vrací pøevedené èíslo.
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
 * Inicializuje buffer a vrátí ho ve struktuøe TBuffer.
 * Pokud se nepodaøí alokovat pamì», ukonèí program s chybovým kódem.
 * @param size Poèet znakù, velikost bufferu.
 * @return Vrací inicializovaný buffer.
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
 * Naète N znakù ze standartního vstupu do cílového bufferu.
 * @param destination Cílový buffer.
 * @param N Poèet znakù.
 * @return Vrací poèet naètených znakù.
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
 * Vypí¹e N znakù na standartní výstup ze zdrojového buffer od urèené pozice.
 * @param source Zdrojový buffer.
 * @param position Pozice.
 * @param N Poèet znakù.
 * @return Vrací poèet vypsaných znakù.
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
 * Zkopíruje N znakù ze zdrojového bufferu
 * do nového bufferu, který vrátí ve struktuøe TBuffer.
 * @param source Zdrojový buffer.
 * @param N Poèet znakù
 * @return Vrací naplnìný buffer o N znacích.
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
 * Zpracuje argumenty pøíkazového øádku a vrátí je ve struktuøe TParams.
 * Pokud je formát argumentù chybný, ukonèí program s chybovým kódem.
 * @param argc Poèet argumentù.
 * @param argv Pole textových øetìzcù s argumenty.
 * @return Vrací analyzované argumenty pøíkazového øádku.
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
  { // tisk nápovìdy
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
    { // neznámý parametr
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
  { // tøi parametry
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
      { // neznámý parametr
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
    { // neznámý parametr
      result.ecode = ECLWRONG;
    }
  }
  else
  { // pøíli¹ mnoho parametrù
    result.ecode = ECLWRONG;
  }

  return result;
}

/**
 * Komprimuje vstupní text.
 * Hledá opakující se bloky znakù délky N.
 * Dále program pøi nalezení minimálnì 2 a maximálnì 9
 * (více pøi mo¾nosti extra)
 * shodných blokù za sebou zapí¹e tuto posloupnost
 * ve tvaru èíslo udávající poèet opakování + tento blok.
 * @param N Délka bloku znakù.
 * @param extra Mo¾nost extra.
 * @return Vrací èíslo chyby.
 */
int comp(unsigned int N,bool extra)
{
  TBuffer buffer = initBuf(N+1);
  unsigned int count;
  errno=EOK;
  if((count=getNChar(&buffer,N))!=N)
  { //ménì znakù ne¾ délka opakujícího se øetìzce
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
  { //zpracovávání znakù a¾ po konec vstupu
    if(errno==EINMSG)
    { //chyba vstupu
      deinitBuf(&buffer);
      return EINMSG;
    }
    if(buffer.field[buffer.pointer]==buffer.field[(buffer.size+buffer.pointer-1)%buffer.size])
    { //shoda znakù vzdálených N znakù
      if(print>0)
        print--;
      else
        cequal++;
    }
    else
    { //výpis znakù
      int errcode=EOK;
      if((errcode=checkPrintBuffer(buffer,&cequal,&print,extra))!=EOK)
      { //chyba pri vypisu znaku
        deinitBuf(&buffer);
        return errcode;
      }
    }
    errno=EOK;
  }
  //vypsaní posledních znakù v buffer
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
 * Funkce vyhodnotí argumenty a podle toho
 * vytiskne znaky ze zdrojového bufferu.
 * @param buffer Zdrojový buffer.
 * @param cequal Poèet shodných znakù
 * @param print Poèet znakù, které se namají tisknout
 * @param extra Mo¾nost extra.
 * @return Vrací èíslo chyby.
 */
int checkPrintBuffer(TBuffer buffer,int *cequal,int *print,bool extra)
{
  if(*print>0)
  {
    *print-=1;;
    return EOK;
  }

  if(*cequal==0)
  { //¾ádná shoda, vytiskne nejstar¹í znak
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
  { //shoda, vytiskne èíslo a blok znakù
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
 * Dekomprimuje vstupní text.
 * Opí¹e vstupní znaky na výstup. Pokud narazí na èíslici
 * (na èíslo pøi extra nastavení),
 * zopakuje následujících N znakù tolikrát, kolik udává èíslice.
 * @param N Délka bloku znakù.
 * @param extra Mo¾nost extra.
 * @return Vrací èíslo chyby.
 */
int decomp(unsigned int N,bool extra)
{
  int c;
  while((c=getchar())!=EOF)
  {
    if(isdigit(c))
    { //èíslice
      TBuffer buffer = initBuf(N);
      int count=c-'0';  // pøevod èíslice na èíslo
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
 * Hlavní program.
 */
int main(int argc, char *argv[])
{
  TParams params = getParams(argc, argv);
  if (params.ecode != EOK)
  { // nìco nestandardního
    printECode(params.ecode);
    return EXIT_FAILURE;
  }

  int errcode=EOK;
  if (params.state == CHELP)
  { // tisk nápovìdy
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
  { // nìco nestandardního
    printECode(errcode);
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}

/* konec proj1.c */
