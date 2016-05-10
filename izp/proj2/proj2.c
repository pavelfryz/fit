/*
 * Soubor:  proj2.c
 * Datum:   2010/11/11
 * Autor:   Pavel Fr�z, xfryzp00@fit.vutbr.cz
 * Projekt: Itera�n� v�po�ty, projekt �. 2 pro p�edm�t IZP
 * Popis:   Program na��t� hodnoty ze standartn�ho vstupu
            a po��t� vybran� matematick� funkce. V�sledek
            pr�b�n� tiskne na standartn� v�stup.
            Jedn� se o funkce logaritmus x o z�kladu a,
            p�irozen� logaritmus, hyperbolick� tangens a
            mocninu e. D�le po��t� v�en� aritmetick� a
            kvadratick� pr�m�r.
 */

// pr�ce se vstupem/v�stupem
#include <stdio.h>
// obecn� funkce jazyka C
#include <stdlib.h>
// kv�li funkci strcmp
#include <string.h>
// testov�n� znak� - isalpha, isdigit, atd.
#include <ctype.h>
// typ bool, konstanty true, false
#include <stdbool.h>
// matematick� funkce
#include <math.h>
// prom�nn� errno
#include <errno.h>
// limity ��seln�ch typ�
#include <limits.h>
// limity desetinn�ch typ�
#include <float.h>

#define REZERVA 5
const double IZP_LN2 = 0.693147180559945309417232121458;
const double IZP_E = 2.718281828459045235360287471352;

/** K�dy chyb programu */
enum tecodes
{
  EOK = 0,     /**< Bez chyby */
  ECLWRONG,    /**< Chybn� p��kazov� ��dek. */
  ESIGDIG1,    /**< Chybn� form�t ��sla. */
  ESIGDIG2,    /**< Mimo rozsah unsigned nebo 0. */
  ESIGDIG3,    /**< Double nema takovou presnost */
  EADOUBLE,    /**< Chybn� parametr A */
  EFORMAT,     /**< Chybna vstupni data */
  EWEIGHT,     /**< Zaporna vaha */
  EUNKNOWN,    /**< Nezn�m� chyba */
};

/** Stavov� k�dy programu */
enum tstates
{
  CHELP,       /**< N�pov�da */
  CTANH,       /**< Hyperbolick� tangens */
  CLOGAX,      /**< Logaritmus x o z�kladu a */
  CLN,         /**< P�irozen� logaritmus */
  CEX,         /**< E^x */
  CWAM,        /**< V�en� aritmetick� pr�m�r */
  CWQM,        /**< V�en� kvadratick� pr�m�r */
};

/** Chybov� hl�en� odpov�daj�c� chybov�m k�d�m. */
const char *ECODEMSG[] =
{
  /* EOK */
  "V�e v po��dku.\n",
  /* ECLWRONG */
  "Chybn� parametry p��kazov�ho ��dku!\n",
  /* ESIGDIG1 */
  "Chyba! Parametr SIGDIG nen� ��slo.\n",
  /* ESIGDIG2 */
  "Chyba! Parametr SIGDIG je mimo rozsah unsigned int.\n",
  /* ESIGDIG3 */
  "Varov�n�: Po�adovan� p�esnost nelze zaru�it.\n",
  /* EADOUBLE */
  "Chyba! Parametr base m� �patn� form�t.\n",
  /* EFORMAT */
  "Varov�n�: Vstup obsahoval nezn�m� data.\n",
  /* EWEIGHT */
  "Varov�n�: Prvek m�l z�pornou v�hu.\n",

  "Nastala nep�edv�dan� chyba! Vypn�te po��ta� a rychle ute�te.\n",
};

const char *HELPMSG =
  "Program Itera�n� v�po�ty.\n"
  "Autor: Pavel Fr�z (c) 2010\n"
  "Program na��t� ��sla ze standartn�ho vstupu a prov�d�\n"
  "nad nimi vybranou matematickou funkci. V�sledky pr�b�n�\n"
  "tiskne na standartn� v�stup. V p��pad� v�en�ho aritmetick�ho\n"
  "a kvadratick0ho pr�m�ru na��t� dv� ��sla, prvn� ��slo je hodnota\n"
  "a druh� ��slo je jeho v�ha. Funkce po��t� na zadanou p�esnost, kter�\n"
  "je vyj�d�ena jako po�et platn�ch cifer(0=MAX PRESNOST).\n"
  "Pou�it�: proj2 -h\n"
  "         proj2 -wam\n"
  "         proj2 -wqm\n"
  "         proj2 --tanh sigdig\n"
  "         proj2 --ln sigdig\n"
  "         proj2 --ex sigdig\n"
  "         proj2 --logax sigdig base\n"
  "Popis parametr�:\n"
  "-h Vytiskne n�pov�du\n"
  "-wam Po��t� v�en� aritmetick� pr�m�r\n"
  "-wqm Po��t� v�en� kvadratick� pr�m�r\n"
  "--tanh Po��t� hyperbolick� tangens\n"
  "--ln Po��t� p�irozen� logaritmus\n"
  "--ex Po��t� mocninu e\n"
  "--logax Po��t� logaritmus\n"
  "sigdig Po�et platn�ch cifer\n"
  "base Z�klad logaritmu\n";

/**
 * Struktura obsahuj�c� hodnoty parametr� p��kazov� ��dky.
 */
typedef struct params
{
  double epsilon;      /**< P�esnost vypo��tan� z parametru sigdig */
  double A;            /**< Z�klad logaritmu. */
  int ecode;           /**< Chybov� k�d programu, odpov�d� v��tu tecodes. */
  int state;           /**< Stavov� k�d programu, odpov�d� v��tu tstates. */
} TParams;

/**
 * Struktura pro v�po�et v�en�ho pr�m�ru.
 */
typedef struct average
{
  double sumValue;      /**< Sou�et hodnot */
  double sumWeight;     /**< Sou�et vah. */
  double value;         /**< Aktu�ln� hodnota. */
  double weight;        /**< Aktu�ln� v�ha. */
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
 *  P�evede �et�zec na presnost.
 *  @param str �et�zec.
 *  @param epsilon Presnost.
 *  @return Vrac� cislo chyby.
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
    .epsilon = 0.,
    .A = 0.,
    .ecode = EOK,
    .state = CHELP,
  };

  if (argc == 2)
  { // jeden parametr
    if(strcmp("-h", argv[1]) == 0)
    { // tisk n�pov�dy
      result.state = CHELP;
    }
    else if(strcmp("--wam", argv[1]) == 0)
    { // aritmetick� pr�m�r
      result.state = CWAM;
    }
    else if(strcmp("--wqm", argv[1]) == 0)
    { // kvadratick� pr�m�r
      result.state = CWQM;
    }
    else
    { // chybn� parametr
      result.ecode = ECLWRONG;
    }
  }
  else if (argc == 3)
  { // dva parametry
    if(strcmp("--tanh", argv[1]) == 0)
    { //  hyperbolick� tangens
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
    { // chybn� parametr
      result.ecode = ECLWRONG;
    }
  }
  else if (argc == 4)
  { // t�i parametry
    if(strcmp("--logax", argv[1]) == 0)
    { // logaritmus x o z�kladu a
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
    { // chybn� parametr
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
 * Vypo��t� e^x se zadanou relativn� p�esnost� epsilon
 * @param x Argument x.
 * @param epsilon Relativn� p�esnost.
 * @return Vrac� e^x.
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
 * Vypo��t� hyperbolicky tangens x se zadanou relativn� p�esnost� epsilon
 * @param x Argument tanh.
 * @param epsilon Relativn� p�esnost.
 * @return Vrac� tanh x.
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
 * Vypo��t� p�irozen� logaritmus x se zadanou relativn� p�esnost� epsilon
 * @param x Argument logaritmu.
 * @param epsilon Relativn� p�esnost.
 * @return Vrac� p�irozen� logaritmus x.
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
 * Vypo��t� logaritmus x o z�kladu a se zadanou relativn� p�esnost� epsilon
 * @param x Argument logaritmu.
 * @param a Z�klad logaritmu.
 * @param epsilon Relativn� p�esnost.
 * @return Vrac� logaritmus x o z�kladu a.
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
 * Vypo��t� aritmeticky prumer hodnoty hod o vaze vah
 * @param average Struktura s hodnotami pro vypocet.
 * @return Vrac� aritmeticky prumer.
 */
double wam(TAverage *average)
{
  average->sumValue+=average->value*average->weight;
  average->sumWeight+=average->weight;
  return average->sumValue/average->sumWeight;
}

/**
 * Vypo��t� kvadraticky prumer hodnoty hod o vaze vah
 * @param average Struktura s hodnotami pro vypocet.
 * @return Vrac� kvadraticky prumer.
 */
double wqm(TAverage *average)
{
  average->sumValue+=average->value*average->value*average->weight;
  average->sumWeight+=average->weight;
  return sqrt((average->sumValue)/(average->sumWeight));
}

/**
 * Na��t� data ze standartn�ho vstupu a podle parametr�
 * po��t� vybranou matematickou funkci.
 * @param params Struktura s parametry.
 * @return Vrac� cislo chyby.
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
 * @return Vrac� cislo chyby.
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
 * Hlavn� program.
 */
int main(int argc, char *argv[])
{
  TParams params = getParams(argc, argv);
  if (params.ecode != EOK && params.ecode != ESIGDIG3)
  { // n�co nestandardn�ho
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
  { // n�co nestandardn�ho
    printECode(errcode);
    return EXIT_FAILURE;
  }
  return EXIT_SUCCESS;
}
/* konec proj2.c */
