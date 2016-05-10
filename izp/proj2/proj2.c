/*
 * Soubor:  proj2.c
 * Datum:   2010/11/11
 * Autor:   Pavel Frýz, xfryzp00@fit.vutbr.cz
 * Projekt: Iteraèní výpoèty, projekt è. 2 pro pøedmìt IZP
 * Popis:   Program naèítá hodnoty ze standartního vstupu
            a poèítá vybrané matematické funkce. Výsledek
            prùbì¾nì tiskne na standartní výstup.
            Jedná se o funkce logaritmus x o základu a,
            pøirozený logaritmus, hyperbolický tangens a
            mocninu e. Dále poèítá vá¾ený aritmetický a
            kvadratický prùmìr.
 */

// práce se vstupem/výstupem
#include <stdio.h>
// obecné funkce jazyka C
#include <stdlib.h>
// kvùli funkci strcmp
#include <string.h>
// testování znakù - isalpha, isdigit, atd.
#include <ctype.h>
// typ bool, konstanty true, false
#include <stdbool.h>
// matematické funkce
#include <math.h>
// promìnná errno
#include <errno.h>
// limity èíselných typù
#include <limits.h>
// limity desetinných typù
#include <float.h>

#define REZERVA 5
const double IZP_LN2 = 0.693147180559945309417232121458;
const double IZP_E = 2.718281828459045235360287471352;

/** Kódy chyb programu */
enum tecodes
{
  EOK = 0,     /**< Bez chyby */
  ECLWRONG,    /**< Chybný pøíkazový øádek. */
  ESIGDIG1,    /**< Chybný formát èísla. */
  ESIGDIG2,    /**< Mimo rozsah unsigned nebo 0. */
  ESIGDIG3,    /**< Double nema takovou presnost */
  EADOUBLE,    /**< Chybný parametr A */
  EFORMAT,     /**< Chybna vstupni data */
  EWEIGHT,     /**< Zaporna vaha */
  EUNKNOWN,    /**< Neznámá chyba */
};

/** Stavové kódy programu */
enum tstates
{
  CHELP,       /**< Nápovìda */
  CTANH,       /**< Hyperbolický tangens */
  CLOGAX,      /**< Logaritmus x o základu a */
  CLN,         /**< Pøirozený logaritmus */
  CEX,         /**< E^x */
  CWAM,        /**< Vá¾ený aritmetický prùmìr */
  CWQM,        /**< Vá¾ený kvadratický prùmìr */
};

/** Chybová hlá¹ení odpovídající chybovým kódùm. */
const char *ECODEMSG[] =
{
  /* EOK */
  "V¹e v poøádku.\n",
  /* ECLWRONG */
  "Chybné parametry pøíkazového øádku!\n",
  /* ESIGDIG1 */
  "Chyba! Parametr SIGDIG není èíslo.\n",
  /* ESIGDIG2 */
  "Chyba! Parametr SIGDIG je mimo rozsah unsigned int.\n",
  /* ESIGDIG3 */
  "Varování: Po¾adovaná pøesnost nelze zaruèit.\n",
  /* EADOUBLE */
  "Chyba! Parametr base má ¹patný formát.\n",
  /* EFORMAT */
  "Varování: Vstup obsahoval neznámá data.\n",
  /* EWEIGHT */
  "Varování: Prvek mìl zápornou váhu.\n",

  "Nastala nepøedvídaná chyba! Vypnìte poèítaè a rychle uteète.\n",
};

const char *HELPMSG =
  "Program Iteraèní výpoèty.\n"
  "Autor: Pavel Frýz (c) 2010\n"
  "Program naèítá èísla ze standartního vstupu a provádí\n"
  "nad nimi vybranou matematickou funkci. Výsledky prùbì¾nì\n"
  "tiskne na standartní výstup. V pøípadì vá¾eného aritmetického\n"
  "a kvadratick0ho prùmìru naèítá dvì èísla, první èíslo je hodnota\n"
  "a druhé èíslo je jeho váha. Funkce poèítá na zadanou pøesnost, která\n"
  "je vyjádøena jako poèet platných cifer(0=MAX PRESNOST).\n"
  "Pou¾ití: proj2 -h\n"
  "         proj2 -wam\n"
  "         proj2 -wqm\n"
  "         proj2 --tanh sigdig\n"
  "         proj2 --ln sigdig\n"
  "         proj2 --ex sigdig\n"
  "         proj2 --logax sigdig base\n"
  "Popis parametrù:\n"
  "-h Vytiskne nápovìdu\n"
  "-wam Poèítá vá¾ený aritmetický prùmìr\n"
  "-wqm Poèítá vá¾ený kvadratický prùmìr\n"
  "--tanh Poèítá hyperbolický tangens\n"
  "--ln Poèítá pøirozený logaritmus\n"
  "--ex Poèítá mocninu e\n"
  "--logax Poèítá logaritmus\n"
  "sigdig Poèet platných cifer\n"
  "base Základ logaritmu\n";

/**
 * Struktura obsahující hodnoty parametrù pøíkazové øádky.
 */
typedef struct params
{
  double epsilon;      /**< Pøesnost vypoèítaná z parametru sigdig */
  double A;            /**< Základ logaritmu. */
  int ecode;           /**< Chybový kód programu, odpovídá výètu tecodes. */
  int state;           /**< Stavový kód programu, odpovídá výètu tstates. */
} TParams;

/**
 * Struktura pro výpoèet vá¾eného prùmìru.
 */
typedef struct average
{
  double sumValue;      /**< Souèet hodnot */
  double sumWeight;     /**< Souèet vah. */
  double value;         /**< Aktuální hodnota. */
  double weight;        /**< Aktuální váha. */
} TAverage;

/////////////////////////////////////////////////////////////////
void printECode(int ecode);
TParams getParams(int argc, char *argv[]);
unsigned int atou(const char * str);
int getEpsilon(char *str,double *epsilon);
int testInput(int count,double *inDouble);
int calculate(TParams params);
double ln(double x,double epsilon);
double logax(double x, double a, double epsilon);
double enax(double x,double epsilon);
double mytanh(double x,double epsilon);
double wam(TAverage *average);
double wqm(TAverage *average);
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
 *  Pøevede øetìzec na presnost.
 *  @param str Øetìzec.
 *  @param epsilon Presnost.
 *  @return Vrací cislo chyby.
 */
int getEpsilon(char *str,double *epsilon)
{
  unsigned int sigdig;
  errno = 0;
  sigdig = atou(str);
  if (errno==EINVAL)
  { // chyba SIGDIG spatny format
    return ESIGDIG1;
  }
  else if(errno)
  { // chyba SIGDIG je mimo rozsah (unsigned int)
    return ESIGDIG2;
  }
  else if(sigdig==0) *epsilon=0;
  else
  {
    *epsilon=pow(0.1,sigdig);
    if(sigdig>DBL_DIG) return ESIGDIG3;
  }
  return EOK;
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
    .epsilon = 0.,
    .A = 0.,
    .ecode = EOK,
    .state = CHELP,
  };

  if (argc == 2)
  { // jeden parametr
    if(strcmp("-h", argv[1]) == 0)
    { // tisk nápovìdy
      result.state = CHELP;
    }
    else if(strcmp("--wam", argv[1]) == 0)
    { // aritmetický prùmìr
      result.state = CWAM;
    }
    else if(strcmp("--wqm", argv[1]) == 0)
    { // kvadratický prùmìr
      result.state = CWQM;
    }
    else
    { // chybný parametr
      result.ecode = ECLWRONG;
    }
  }
  else if (argc == 3)
  { // dva parametry
    if(strcmp("--tanh", argv[1]) == 0)
    { //  hyperbolický tangens
      result.state = CTANH;
      result.ecode = getEpsilon(argv[2],&(result.epsilon));
    }
    else if(strcmp("--ln", argv[1]) == 0)
    { // prirozeny logaritmus
      result.state = CLN;
      result.ecode = getEpsilon(argv[2],&(result.epsilon));
    }
    else if(strcmp("--ex", argv[1]) == 0)
    { // e^x
      result.state = CEX;
      result.ecode = getEpsilon(argv[2],&(result.epsilon));
    }
    else
    { // chybný parametr
      result.ecode = ECLWRONG;
    }
  }
  else if (argc == 4)
  { // tøi parametry
    if(strcmp("--logax", argv[1]) == 0)
    { // logaritmus x o základu a
      result.state = CLOGAX;
      result.ecode = getEpsilon(argv[2],&(result.epsilon));
      errno = 0;
      char *ptr=NULL;
      result.A = strtod(argv[3],&ptr);
      if(*ptr!='\0'||(errno)||(result.A<=0.0)||result.A==1)
      { // chyba A spatny format nebo mimo R+-{1}
        result.ecode = EADOUBLE;
      }
    }
    else
    { // chybný parametr
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
 * Vypoèítá e^x se zadanou relativní pøesností epsilon
 * @param x Argument x.
 * @param epsilon Relativní pøesnost.
 * @return Vrací e^x.
 */
double enax(double x, double epsilon)
{
  //kontrola parametru
  if(isnan(x))
  {
    errno=EDOM;
    return NAN;
  }
  if(x==HUGE_VAL)
  {
    return HUGE_VAL;
  }
  if(x==-HUGE_VAL)
  {
    return 0;
  }
  if(epsilon<0)
  {
    errno=EINVAL;
    epsilon=0;
  }
  // rozdeleni x na celou cast a zbytek
  double intpart;    //cela cast x
  double fraction;   //desetina cast x
  fraction=modf(x,&intpart);
  // vypocet enax cele casti
  double enaint=IZP_E; //e^celou cast x
  if(intpart>0)
  {
    for(double i=1; i<intpart; i+=1)
    {
      enaint*=IZP_E;
      if(enaint==HUGE_VAL) return enaint;
    }
  }
  else
  {
    for(double i=0; i<=-intpart; i+=1)
    {
      enaint/=IZP_E;
      if(enaint==0) return enaint;
    }
  }
  // inicializace pred iteraci
  double sum=1;           //aktualni soucet rady
  double factN=1;         //jmenovatel, n!
  double xPowN=fraction;  //citatel, x^n
  double step=xPowN/factN;//aktualni clen rady
  long int n=2;           //n
  // iterace
  while(fabs(step) > epsilon*fabs(sum*enaint))
  {
    sum+=step;
    xPowN*=fraction;
    factN*=n;
    n++;
    step=xPowN/factN;
  }

  // e^(c+d)=e^c*e^d
  return (sum+step)*enaint;
}

/**
 * Vypoèítá hyperbolicky tangens x se zadanou relativní pøesností epsilon
 * @param x Argument tanh.
 * @param epsilon Relativní pøesnost.
 * @return Vrací tanh x.
 */
double mytanh(double x,double epsilon)
{
  //kontrola parametru
  if(isnan(x))
  {
    errno=EDOM;
    return NAN;
  }
  if(x==HUGE_VAL)
  {
    return 1;
  }
  if(x==-HUGE_VAL)
  {
    return -1;
  }
  if(epsilon<0)
  {
    errno=EINVAL;
    epsilon=0;
  }
  if(fabs(x)>DBL_DIG+REZERVA)
  {
    if(x>0)return 1;
    else return -1;
  }
  //vypocet tanh
  double sin=x,cos=1,tanOld,tanNew=x,step=x;
  unsigned long n=2;
  do
  {
    tanOld=tanNew;
    step*=(x/n);
    n++;
    cos+=step;
    step*=(x/n);
    n++;
    sin+=step;
    tanNew=sin/cos;
  } while(fabs(tanOld-tanNew) > epsilon*fabs(tanNew));
  return tanNew;
}

/**
 * Vypoèítá pøirozený logaritmus x se zadanou relativní pøesností epsilon
 * @param x Argument logaritmu.
 * @param epsilon Relativní pøesnost.
 * @return Vrací pøirozený logaritmus x.
 */
double ln(double x,double epsilon)
{
  //kontrola parametru
  if(x<0||isnan(x))
  {
    errno=EDOM;
    return NAN;
  }
  if(x==0)
  {
    errno=ERANGE;
    return -HUGE_VAL;
  }
  if(isinf(x))
  {
    return HUGE_VAL;
  }
  if(epsilon<0)
  {
    errno=EINVAL;
    epsilon=0;
  }
  if(x==1)
  {
    return 0;
  }
  // prevod x do intervalu [0.5,1)
  // nove x = x*2^k
  int k=0;
  x=frexp(x,&k);
  /* funkce frexp lze nahradit nasledujicim komentarem
  while(x>1)
  {
    x/=2;
    k++;
  }
  while(x<0.5)
  {
    x*=2;
    k--;
  }
  */

  double sum;          // aktualni soucet rady, ln x, x=[0.5,1)
  double step;         // aktualni clen rady
  double xsub1;        // citatel, (x-1)^count
  double xadd1;        // 1.cast jmenovatele, (x+1)^count
  int count;           // 2.cast jmenovatele, count=2n-1
  // jmenovatel = xadd1*count
  // step = 2*(citatel/jmenovatel)
  long double kmulln2; // k*ln 2
  // ln(a*b)=ln a + ln b  ln(a^n)=n*ln a
  // ln = ln x + k*ln 2 = sum+kmulln2
  // inicializace promenych pred iteraci
  kmulln2=k*IZP_LN2;
  sum=0;
  count=1;
  xsub1=x-1.,xadd1=x+1.;
  // iterace
  do
  {
    step=2*((xsub1)/(xadd1*count));
    sum+=step;
    count+=2;
    xsub1*=(x*x-2*x+1);
    xadd1*=(x*x+2*x+1);
  } while(fabs(step) > epsilon*fabs(sum+kmulln2));
  return sum+kmulln2;
}

/**
 * Vypoèítá logaritmus x o základu a se zadanou relativní pøesností epsilon
 * @param x Argument logaritmu.
 * @param a Základ logaritmu.
 * @param epsilon Relativní pøesnost.
 * @return Vrací logaritmus x o základu a.
 */
double logax(double x, double a, double epsilon)
{
  // kontrola parametru
  if(a<=0||a==1||isnan(a)||isinf(a)||x<0||isnan(x))
  {
    errno=EDOM;
    return NAN;
  }
  if(x==0)
  {
    errno=ERANGE;
    return -HUGE_VAL;
  }
  if(isinf(x))
  {
    return HUGE_VAL;
  }
  if(epsilon<0)
  {
    errno=EINVAL;
    epsilon=0;
  }
  // vypocet logaritmu
  double logax;
  logax=ln(x,epsilon)/ln(a,epsilon);
  return logax;
}

/**
 * Vypoèítá aritmeticky prumer hodnoty hod o vaze vah
 * @param average Struktura s hodnotami pro vypocet.
 * @return Vrací aritmeticky prumer.
 */
double wam(TAverage *average)
{
  average->sumValue+=average->value*average->weight;
  average->sumWeight+=average->weight;
  return average->sumValue/average->sumWeight;
}

/**
 * Vypoèítá kvadraticky prumer hodnoty hod o vaze vah
 * @param average Struktura s hodnotami pro vypocet.
 * @return Vrací kvadraticky prumer.
 */
double wqm(TAverage *average)
{
  average->sumValue+=average->value*average->value*average->weight;
  average->sumWeight+=average->weight;
  return sqrt((average->sumValue)/(average->sumWeight));
}

/**
 * Naèítá data ze standartního vstupu a podle parametrù
 * poèítá vybranou matematickou funkci.
 * @param params Struktura s parametry.
 * @return Vrací cislo chyby.
 */
int calculate(TParams params)
{
  double value,weight;
  int count;
  int errcode=EOK;
  TAverage average=
  { //inicializece struktury
    .sumValue=0,
    .sumWeight=0,
    .value=0,
    .weight=0,
  };
  while((count=scanf("%lf",&value))!=EOF)
  {
    int err=EOK;
    err=testInput(count,&value);
    if(err)errcode=err;
    switch(params.state)
    {
    case CWAM:// vazeny aritmeticky prumer
      if((count=scanf("%lf",&weight))!=EOF)
      {
        err=testInput(count,&weight);
        if(err)errcode=err;
        if(!errcode&&weight<0) errcode=EWEIGHT;
        average.value=value;
        average.weight=weight;
        printf("%.10e\n",wam(&average));
      }
      else printf("%.10e\n",NAN);
      break;
    case CWQM:// vazeny kvadraticky prumer
      if((count=scanf("%lf",&weight))!=EOF)
      {
        err=testInput(count,&weight);
        if(err)errcode=err;
        if(!errcode&&weight<0) errcode=EWEIGHT;
        average.value=value;
        average.weight=weight;
        printf("%.10e\n",wqm(&average));
      }
      else printf("%.10e\n",NAN);
      break;
    case CTANH:// hyperbolicky tangens
      printf("%.10e\n",mytanh(value,params.epsilon));
      break;
    case CLOGAX:// logaritmus x o zakladu a
      printf("%.10e\n",logax(value,params.A,params.epsilon));
      break;
    case CLN:// prirozeny logaritmus
      printf("%.10e\n",ln(value,params.epsilon));
      break;
    case CEX:// e^x
      printf("%.10e\n",enax(value,params.epsilon));
      break;
    default:
      return EUNKNOWN;
    }
  }
  return errcode;
}

/**
 * Testuje vysledek posledniho scanf o jedne hodnote
 * @param count Pocet nactenych hodnot.
 * @param double nactene cislo.
 * @return Vrací cislo chyby.
 */
int testInput(int count,double *inDouble)
{
  int c;
  if(count!=1)
  {
    *inDouble=NAN;
    scanf("%*s");
    return EFORMAT;
  }
  else if((c=getchar())!=EOF&&!isspace(c))
  {
    *inDouble=NAN;
    ungetc(c,stdin);
    scanf("%*s");
    return EFORMAT;
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
  if (params.ecode != EOK && params.ecode != ESIGDIG3)
  { // nìco nestandardního
    printECode(params.ecode);
    return EXIT_FAILURE;
  }
  else if(params.ecode == ESIGDIG3)
  { // velka presnost
    printECode(params.ecode);
  }
  if (params.state == CHELP)
  { // tisk napovedy
    printf("%s", HELPMSG);
    return EXIT_SUCCESS;
  }

  int errcode=EOK;
  errcode=calculate(params);

  if (errcode != EOK)
  { // nìco nestandardního
    printECode(errcode);
    return EXIT_FAILURE;
  }
  return EXIT_SUCCESS;
}
/* konec proj2.c */
