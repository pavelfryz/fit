/*
 * Soubor:  proj3.c
 * Datum:   2010/11/29
 * Autor:   Pavel Frýz, xfryzp00@fit.vutbr.cz
 * Projekt: Maticové operacace, projekt è. 3 pro pøedmìt IZP
 * Popis:   Program naèítá vektory, matice nebo vektory matic
 * ze souboru a provádí vybrané maticové operace:
 * Vektorový souèet dvou vektorù (a+b)
 * Skalární souèin dvou vektorù (a*b)
 * Souèin matic (A*B)
 * Maticový výraz (A*B*A)
 * Poèet bublin(seskupení nul) v matici, vektoru matic
 * Osmismìrka, hledá vektor v matici
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

#define TYPE_VECTOR   1
#define TYPE_MATRIX   2
#define TYPE_3DMATRIX 3

#define getAxis(matrix,dim,row,col) ((dim)*(matrix).rows*(matrix).cols+(row)*(matrix).cols+(col))
#define getAxisP(matrix,dim,row,col) ((dim)*(matrix)->rows*(matrix)->cols+(row)*(matrix)->cols+(col))

/** Kódy chyb programu */
enum tecodes
{
  EOK = 0,     /**< Bez chyby */
  ECLWRONG,    /**< Chybný pøíkazový øádek. */
  EFOPEN,      /**< Chyba, soubor nenalezen. */
  EBADVAL,     /**< Chyba, spatny format dat. */
  EALLOC,      /**< Chyba pamìti */
  EPARAM,      /**< Chybné parametry */
  EFALSE,      /**< Operace nema platny vysledek */
  EUNKNOWN,    /**< Neznámá chyba */
};

/** Kódy smìrù */
enum tdirection
{
  DS = 0, /**< Sever */
  DSV,    /**< Severo-vychod */
  DV,     /**< Východ */
  DJV,    /**< Jiho-východ */
  DJ,     /**< Jih */
  DJZ,    /**< Jiho-západ */
  DZ,     /**< Západ */
  DSZ,    /**< Severo-západ */
};

/** Stavové kódy programu */
enum tstates
{
  CHELP,        /**< Nápovìda */
  CTEST,        /**< Testovací výstup */
  CVADD,        /**< Vektorový souèet */
  CVSCAL,       /**< Skalární souèin vektorù */
  CMMULT,       /**< Souèin dvou matic */
  CMEXPR,       /**< Maticový výraz A*B*A */
  CEIGHT,       /**< Osmismìrka */
  CBUBBLES,     /**< Bubliny */
  CEXTBUBBLES,  /**< Bubliny - roz¹íøení do 3D */
};

/** Chybová hlá¹ení odpovídající chybovým kódùm. */
const char *ECODEMSG[] =
{
  /* EOK */
  "V¹e v poøádku.\n",
  /* ECLWRONG */
  "Chybné parametry pøíkazového øádku!\n",
  /* EFOPEN */
  "Chyba pøi otevírání souboru! Soubor nenalezen\n",
  /* EBADVAL */
  "Chybný formát vstupního souboru!\n",
  /* EALLOC */
  "Nedostatek pamìti\n",
  /* EPARAM */
  "Chybné operandy! Operaci nelze provést.\n",

  /* EUNKNOWN */
  "Nastala nepøedvídaná chyba! Vypnìte poèítaè a rychle uteète.\n",
};

const char *HELPMSG =
  "Program Maticové operace.\n"
  "Autor: Pavel Frýz (c) 2010\n"
  "Program naèítá vektory, matice nebo vektory matic\n"
  "ze souboru a provádí vybrané maticové operace:\n"
  "Vektorový souèet dvou vektorù (a+b)\n"
  "Skalární souèin dvou vektorù (a*b)\n"
  "Souèin matic (A*B)\n"
  "Maticový výraz (A*B*A)\n"
  "Poèet bublin(seskupení nul) v matici, vektoru matic\n"
  "Osmismìrka, hledá vektor v matici\n"
  "Pou¾ití: proj3 -h\n"
  "         proj3 -c N\n"
  "         proj3 --test data.txt\n"
  "         proj3 --vadd a.txt b.txt\n"
  "         proj3 --vscal a.txt b.txt\n"
  "         proj3 --mmult A.txt B.txt\n"
  "         proj3 --mexpr A.txt B.txt\n"
  "         proj3 --eight v.txt M.txt\n"
  "         proj3 --bubbles M.txt\n" 
  "         proj3 --extbubbles 3DM.txt\n"
  "Popis parametrù:\n"
  "-h Vytiskne nápovìdu k pou¾ití programu.\n" 	 
  "--test data.txt Zkontroluje vstupní data, vytiskne je na standardní výstup.\n"
  "--vadd a.txt b.txt Seète dva vektory (a+b).\n"
  "--vscal a.txt b.txt Vypoète skalární souèin (a*b) dvou vektorù.\n"
  "--mmult A.txt B.txt Vypoète souèin dvou matic v daném poøadí (A*B).\n" 
  "--mexpr A.txt B.txt Vypoète maticový výraz (A*B*A).\n"
  "--eight v.txt M.txt Vyhledá v matici M vektor v.\n"
  "--bubbles M.txt Spoèítá poèet bublin v matici M.\n"
  "--extbubbles 3DM.txt Spoèítá poèet bublin v prostoru 3DM.\n";

/**
 * Struktura obsahující hodnoty parametrù pøíkazové øádky.
 */
typedef struct params
{
  char * filename1; /**< Název prvního souboru. */
  char * filename2; /**< Název druhého souboru. */
  int ecode;        /**< Chybový kód programu, odpovídá výètu tecodes. */
  int state;        /**< Stavový kód programu, odpovídá výètu tstates. */
} TParams;


/**
 * Struktura pro práci s maticemi.
 */
typedef struct tmatrix
{
  unsigned int rows;     /**< Poèet øádkù. */
  unsigned int cols;     /**< Poèet sloupcù. */
  unsigned int nums;     /**< Poèet matic. */
  int type;              /**< Typ matice. */
  int *matrix;           /**< Matice. */
} TMatrix;

/////////////////////////////////////////////////////////////////
void printECode(int ecode);
TParams getParams(int argc, char *argv[]);

int  loadMat (TMatrix * matrix, const char *filename);
int  allocMat(TMatrix * matrix);
void freeMat (TMatrix * matrix);
void printMat(TMatrix * matrix);
int getValMat(const TMatrix * matrix,unsigned int num,unsigned int row, unsigned int col);
void setValMat(TMatrix * matrix,unsigned int num,unsigned int row, unsigned int col,int value);
void addValMat(TMatrix * matrix,unsigned int num,unsigned int row, unsigned int col,int value);
int cpyMat(TMatrix * dest,const TMatrix * src);

int vadd(TMatrix *result,const TMatrix *vect1,const TMatrix *vect2);
int vscal(int *result,const TMatrix *vect1,const TMatrix *vect2);
int mmult(TMatrix *result,const TMatrix *matrix1,const TMatrix *matrix2);
int mexpr(TMatrix *result,const TMatrix *matrix1,const TMatrix *matrix2);
int eight(bool *result,const TMatrix *matrix,const TMatrix *vect);
bool testWord(const TMatrix *matrix,const TMatrix *vect, unsigned int row,unsigned int col, int direction);
int bubbles(int *result,const TMatrix *matrix,int type);
void fillBubble(TMatrix *matrix,unsigned int num,unsigned int row,unsigned int col);
/////////////////////////////////////////////////////////////////

/**
 * Funkce alokuje místo v pamìti pro matici.
 * Rozmìry matice musí být uvedeny ve struktuøe matrix.
 * @param matrix Struktura pro matici .
 * @return Vrací kód chyby.
 */
int allocMat(TMatrix * matrix)
{
  if((matrix->matrix=(int*)malloc(matrix->rows * matrix->cols * matrix->nums *sizeof(int)))==NULL)
  {  // chyba alokace
    return EALLOC;
  }
  return EOK;
}

/**
 * Funkce uvolní pamì» døíve alokované matice.
 * @param matrix Uvolòovaná matice.
 * @return Vrací kód chyby.
 */
void freeMat(TMatrix * matrix)
{
  free(matrix->matrix);
  matrix->matrix=NULL;
  matrix->rows=0;
  matrix->cols=0;
  matrix->nums=0;
}

/**
 * Funkce naète matici ze souboru.
 * @param matrix Matice.
 * @param filename Název souboru.
 * @return Vrací kód chyby.
 */
int loadMat(TMatrix * matrix, const char *filename)
{
  matrix->cols=1;
  matrix->rows=1;
  matrix->nums=1;
  matrix->matrix=NULL;
  if(filename==NULL) return EOK;

  //otevøení souboru
  FILE * file;
  if ((file = fopen(filename, "r")) == NULL)
  {  //chyba pri otevirani souboru
    return EFOPEN;
  }
  if(fscanf(file, "%d", &matrix->type)!=1)
  { // chyba nacteni ze souboru
    fclose(file);
    return EBADVAL;
  }
  if(matrix->type==TYPE_VECTOR)
  { // vector
    if(fscanf(file, "%u", &matrix->cols)!=1)
    {
      fclose(file);
      return EBADVAL;
    }
  }
  else if(matrix->type==TYPE_MATRIX)
  { // matice
    if(fscanf(file, "%u %u", &matrix->rows, &matrix->cols)!=2)
    {
      fclose(file);
      return EBADVAL;
    }
  }
  else if(matrix->type==TYPE_3DMATRIX)
  { // vector matic
    errno=0;
    if(fscanf(file, "%u %u %u", &matrix->nums, &matrix->rows, &matrix->cols)!=3)
    {
      fclose(file);
      return EBADVAL;
    }
  }
  else
  { // neznamy typ
    fclose(file);
    return EBADVAL;
  }
  if(matrix->nums==0||matrix->rows==0||matrix->cols==0)
  {
    fclose(file);
    return EBADVAL;
  }
  if(allocMat(matrix)==EALLOC)
  { // chyba alokace
    fclose(file);
    return EALLOC;
  }
  unsigned long items=matrix->rows*matrix->cols*matrix->nums;
  for(unsigned long i=0;i<items;i++)
  { // nacitani hodnot do matice
    if(fscanf(file, "%d",&matrix->matrix[i])!=1)
    { // chyba pri cteni ze souboru
      freeMat(matrix);
      fclose(file);
      return EBADVAL;
    }
  }
  fscanf(file,"%*d");
  if(feof(file))
  { // vse v poradku, zadna dalsi data v souboru
	fclose(file);
	return EOK;
  }
  freeMat(matrix);
  fclose(file);
  return EBADVAL;
}

/**
 * Funkce vytiskne matici na standartní výstup.
 * @param matrix Matice.
 * @param filename Název souboru.
 * @return Vrací kód chyby.
 */
void printMat(TMatrix * matrix)
{
  printf("%d\n",matrix->type);
  if(matrix->type==TYPE_VECTOR)
    printf("%u\n",matrix->cols);
  else if(matrix->type==TYPE_MATRIX)
    printf("%u %u\n",matrix->rows,matrix->cols);
  else if(matrix->type==TYPE_3DMATRIX)
    printf("%u %u %u\n",matrix->nums,matrix->rows,matrix->cols);

  for(unsigned int nums=0;nums<matrix->nums;nums++)
  {
    for(unsigned int rows=0;rows<matrix->rows;rows++)
    {
      for(unsigned int cols=0;cols<matrix->cols;cols++)
      {
        printf("%d ",matrix->matrix[getAxisP(matrix,nums,rows,cols)]);
      }
      printf("\n");
    }
    if((nums+1)!=matrix->nums)
      printf("\n");
  }
}

/**
 * Ziska hodnotu matice na souradnicich [col][row][col].
 * @param dest Zdrojova matice 
 * @param src Cilova matice
 * @return Vrací kod chyby.
 */
int cpyMat(TMatrix * dest,const TMatrix * src)
{
  dest->nums = src->nums;
  dest->rows = src->rows;
  dest->cols = src->cols;
  dest->type = src->type;
  dest->matrix=NULL;	
  if(allocMat(dest)==EALLOC)
  { // chyba alokace
    return EALLOC;
  }
  for(unsigned int nums=0;nums<src->nums;nums++)
  {
    for(unsigned int rows=0;rows<src->rows;rows++)
    {
      for(unsigned int cols=0;cols<src->cols;cols++)
      {
        int value = getValMat(src,nums,rows,cols);
        setValMat(dest,nums,rows,cols,value);
      }
    }
  }
  return EOK;
}

/**
 * Ziska hodnotu matice na souradnicich [col][row][col].
 * @param matrix Matice
 * @param num Souradnice matice ve vektoru matic
 * @param row Souradnice radku
 * @param col Souradnice sloupce
 * @return Vrací hodnotu na souradnici [num][row][col].
 */
int getValMat(const TMatrix * matrix,unsigned int num,unsigned int row, unsigned int col)
{
  return matrix->matrix[getAxisP(matrix,num,row,col)];	
}

/**
 * Nastavi hodnotu matice na souradnicich [col][row][col]
 * na urcenou hodnotu.
 * @param matrix Matice
 * @param num Souradnice matice ve vektoru matic
 * @param row Souradnice radku
 * @param col Souradnice sloupce
 * @param value Hodnota
 */
void setValMat(TMatrix * matrix,unsigned int num,unsigned int row, unsigned int col,int value)
{
  matrix->matrix[getAxisP(matrix,num,row,col)]=value;	
}

/**
 * Pricte k matici na souradnich [num][row][col]
 * urcenou hodnotu.
 * @param matrix Matice
 * @param num Souradnice matice ve vektoru matic
 * @param row Souradnice radku
 * @param col Souradnice sloupce
 * @param value Hodnota
 */
void addValMat(TMatrix * matrix,unsigned int num,unsigned int row, unsigned int col,int value)
{
  matrix->matrix[getAxisP(matrix,num,row,col)]+=value;	
}

/**
 * Vytiskne hlá¹ení odpovídající chybovému kódu.
 * @param ecode kód chyby programu
 */
void printECode(int ecode)
{
  if (ecode < EOK || ecode > EUNKNOWN)
  { ecode = EUNKNOWN; }

  fprintf(stderr, "%s", ECODEMSG[ecode]);
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
    .filename1 = NULL,
    .filename2 = NULL,
    .ecode = EOK,
    .state = CHELP,
  };
  if (argc == 2 && strcmp("-h", argv[1]) == 0)
  { // tisk nápovìdy
    result.state = CHELP;
  }
  else if (argc == 3)
  { // dva parametry
    result.filename1 = argv[2];
    if(strcmp("--test", argv[1]) == 0)
    { // testovaci vystup
      result.state = CTEST;
    }
    else if(strcmp("--bubbles", argv[1]) == 0)
    { // bubliny
      result.state = CBUBBLES;
    }
    else if(strcmp("--extbubbles", argv[1]) == 0)
    { // bubliny - rozsireni do 3D
      result.state = CEXTBUBBLES;
    }
    else
    { // chybne parametry
      result.ecode = ECLWRONG;
    }
  }
  else if(argc==4)
  { // tøi parametry
    result.filename1 = argv[2];
    result.filename2 = argv[3];
    if(strcmp("--vadd", argv[1]) == 0)
    { // vektorovy soucet
      result.state = CVADD;
    }
    else if(strcmp("--vscal", argv[1]) == 0)
    { // skalarni soucin vektoru
      result.state = CVSCAL;
    }
    else if(strcmp("--mmult", argv[1]) == 0)
    { // soucin dvou matic
      result.state = CMMULT;
    }
    else if(strcmp("--mexpr", argv[1]) == 0)
    { // maticovy vyraz A*B*A
      result.state = CMEXPR;
    }
    else if(strcmp("--eight", argv[1]) == 0)
    { // osmismerka
      result.state = CEIGHT;
    }
    else
    { // chybne parametry
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
 * Funkce seète dva vektory.
 * @param vect1 První vektor.
 * @param vect2 Druhý vektor.
 * @param result Výsledek operace.
 * @return Vrací kód chyby.
 */
int vadd(TMatrix *result,const TMatrix *vect1,const TMatrix *vect2)
{
  result->nums=1;
  result->rows=1;
  result->cols=vect1->cols;
  result->type=TYPE_VECTOR;
  result->matrix=NULL;

  if(vect1->type!=TYPE_VECTOR||vect2->type!=TYPE_VECTOR)
  { // neni vector
    return EPARAM;
  }
  if(vect1->cols!=vect2->cols)
  { // nejsou stejne velky
    return EFALSE;
  }
  if(allocMat(result)==EALLOC)
  { // chyba alokace
    return EALLOC;
  }

  for(unsigned int i=0;i<result->cols;i++)
  {
    result->matrix[i]=vect1->matrix[i]+vect2->matrix[i];
  }
  return EOK;
}


/**
 * Funkce vypoète skalární souèin dvou vektorù.
 * @param vect1 První vektor.
 * @param vect2 Druhý vektor.
 * @param result Výsledek operace.
 * @return Vrací kód chyby.
 */
int vscal(int *result,const TMatrix *vect1,const TMatrix *vect2)
{
  *result=0;
  if(vect1->type!=TYPE_VECTOR||vect2->type!=TYPE_VECTOR)
  { // neni vector
    return EPARAM;
  }
  if(vect1->cols!=vect2->cols)
  { // nejsou stejne velky
    return EFALSE;
  }

  for(unsigned int i=0;i<vect1->cols;i++)
  {
    *result+=vect1->matrix[i]*vect2->matrix[i];
  }
  return EOK;
}


/**
 * Funkce vynásobí dvì matice.
 * @param matrix1 První matice.
 * @param matrix2 Druhá matice.
 * @param result Výsledek operace.
 * @return Vrací kód chyby.
 */
int mmult(TMatrix *result,const TMatrix *matrix1,const TMatrix *matrix2)
{
  result->nums=1;
  result->rows=matrix1->rows;
  result->cols=matrix2->cols;
  result->type=TYPE_MATRIX;
  result->matrix=NULL;

  if(matrix1->type!=TYPE_MATRIX||matrix2->type!=TYPE_MATRIX)
  { // neni matice
    return EPARAM;
  }
  if(matrix1->cols!=matrix2->rows)
  { // nelze provest soucin matic
    return EFALSE;
  }
  if(allocMat(result)==EALLOC)
  { // chyba alokace
    return EALLOC;
  }

  for(unsigned int row=0;row<result->rows;row++)
  {
    for(unsigned int col=0;col<result->cols;col++)
    {
      result->matrix[getAxisP(result,0,row,col)]=0;
      for(unsigned int i=0;i<matrix1->cols;i++)
      {
        int value=getValMat(matrix1,0,row,i)*getValMat(matrix2,0,i,col);
        addValMat(result,0,row,col,value);
      }
    }
  }
  return EOK;
}


/**
 * Funkce vypoète maticový výraz A*B*A.
 * @param matrix1 Matice A.
 * @param matrix2 Matice B.
 * @param result Výsledek operace.
 * @return Vrací kód chyby.
 */
int mexpr(TMatrix *result,const TMatrix *matrix1,const TMatrix *matrix2)
{
  result->nums=1;
  result->rows=matrix1->rows;
  result->cols=matrix2->cols;
  result->type=TYPE_MATRIX;
  result->matrix=NULL;
  TMatrix temp;
  int errcode=EOK;
  if((errcode=mmult(&temp,matrix1,matrix2)))
  {
    return errcode;
  }

  if((errcode=mmult(result,&temp,matrix1)))
  {
    freeMat(&temp);
    return errcode;
  }

  freeMat(&temp);
  return EOK;
}


/**
 * Funkce vyhledá vektor v matici. Pokud vektor najde
 * nastaví výsledek na hodnotu true, jinak na hodnotu false.
 * @param matrix Matice.
 * @param vect Hledaný vektor.
 * @param result Výsledek funkce.
 * @return Vrací kód chyby.
 */
int eight(bool *result, const TMatrix *matrix,const TMatrix *vect)
{
  if(matrix->type!=TYPE_MATRIX||vect->type!=TYPE_VECTOR)
  { // matice neni matice nebo vektor neni vektor
    return EPARAM;
  }

  // prochazeni matice radek po radku
  for(unsigned int row=0;row<matrix->rows;row++)
  {
    for(unsigned int col=0;col<matrix->cols;col++)
    {
      if(matrix->matrix[getAxisP(matrix,0,row,col)]==vect->matrix[getAxisP(vect,0,0,0)])
      { // shoda prvniho znaku vektoru se znakem v matici na souradnicich [row][col]
        for(int i=0;i<=DSZ;i++)
        { // test vsech osmi smeru na vyskyt vektoru v matici
          if(testWord(matrix,vect,row,col,i))
          {
			*result=true;
			return EOK;
	      }
	    }
      }
    }
  }
  *result=false;
  return EOK;
}

/**
 * Funkce zji¹»uje zda se vector 
 * nachází v matici v uvedeném smìru.
 * @param matrix Matice.
 * @param vect Hledaný vektor.
 * @param row Souøadnice øádku.
 * @param row Souøadnice sloupce.
 * @param direction Smìr hledání.
 * @return Vrací výsledek hledání, true=nalezeno, false=nenalezeno.
 */
bool testWord(const TMatrix *matrix,const TMatrix *vect, unsigned int row, unsigned int col, int direction)
{
  const char dir[][2]={
	  {-1,0},
	  {-1,1},
	  {0,1},
	  {1,1},
	  {1,0},
	  {1,-1},
	  {0,-1},
	  {-1,-1},
  };
  char dirRow = dir[direction][0];
  char dirCol = dir[direction][1];

  for(unsigned int i=0;i<vect->cols;i++)
  {
    if(row==UINT_MAX||row==matrix->rows) return false;
    if(col==UINT_MAX||col==matrix->cols) return false;
    if(matrix->matrix[getAxisP(matrix,0,row,col)]!=vect->matrix[i]) return false;
    row+=dirRow;
    col+=dirCol;
  }
  return true;
}


/**
 * Funkce spoèítá poèet bublin v matici, pøípadnì
 * vekroru matic.
 * @param matrix Matice.
 * @param type Typ matice, buï TYPE_MATRIX, nebo TYPE_3DMATRIX.
 * @param result Poèet bublin.
 * @return Vrací kód chyby.
 */
int bubbles(int *result,const TMatrix *matrix,int type)
{
  int errcode;
  TMatrix temp;
  *result=0;
  if(matrix->type!=type)
  {
    return EPARAM;
  }
  if((errcode=cpyMat(&temp,matrix)))
  {
    return errcode;
  }	 
  // pruchod vsemi prvky vektoru matic
  for(unsigned int num=0;num<matrix->nums;num++)
  {
    for(unsigned int row=0;row<matrix->rows;row++)
    {
      for(unsigned int col=0;col<matrix->cols;col++)
      {
        if(temp.matrix[getAxis(temp,num,row,col)]==0)
        { //vyplneni bubliny
          *result=*result+1;
          fillBubble(&temp,num,row,col);
        }
      }
    }
  }
  freeMat(&temp);
  return EOK;
}


/**
 * Funkce vyplní bublinu jednièkami.
 * @param matrix Matice.
 * @param num Souøadnice matice ve vektoru matic.
 * @param row Souøadnice øádku.
 * @param col Souøadnice sloupce.
 */
void fillBubble(TMatrix *matrix,unsigned int num,unsigned int row,unsigned int col)
{
  setValMat(matrix,num,row,col,1);
  if(num!=0 && getValMat(matrix,num-1,row,col)==0)
    fillBubble(matrix,num-1,row,col);
  if(row!=0 && getValMat(matrix,num,row-1,col)==0)
    fillBubble(matrix,num,row-1,col);
  if(col!=0 && getValMat(matrix,num,row,col-1)==0)
    fillBubble(matrix,num,row,col-1);
  if(num+1!=matrix->nums && getValMat(matrix,num+1,row,col)==0)
    fillBubble(matrix,num+1,row,col);
  if(row+1!=matrix->rows && getValMat(matrix,num,row+1,col)==0)
    fillBubble(matrix,num,row+1,col);
  if(col+1!=matrix->cols && getValMat(matrix,num,row,col+1)==0)
    fillBubble(matrix,num,row,col+1);
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
  else
  {
    TMatrix matice1;
    TMatrix matice2;
    TMatrix result;
    result.matrix=matice1.matrix=matice2.matrix=NULL;
    int count=0;
    bool res;
    if(params.state!=CTEST)
    {
      if((errcode=loadMat(&matice1,params.filename1)))
      {
        printECode(errcode);
        return EXIT_FAILURE;
      }
      if((errcode=loadMat(&matice2,params.filename2)))
      {
        freeMat(&matice1);
        printECode(errcode);
        return EXIT_FAILURE;
      }
    }
    switch(params.state)
    {
      case CTEST:      // Test matice
        if(!(errcode=loadMat(&matice1,params.filename1)))
        {
          printMat(&matice1);
          freeMat(&matice1);
        }
        break;
      case CVADD:      // Vektorový souèet
        if(!(errcode=vadd(&result,&matice1,&matice2))||errcode==EFALSE)
        {
          if(errcode)printf("false\n");
          else printMat(&result);
        }
        break;
      case CVSCAL:     // Skalární souèin vektorù
        if(!(errcode=vscal(&count,&matice1,&matice2))||errcode==EFALSE)
        {
          if(errcode)printf("false\n");
          else printf("%d\n",count);
        }
        break;
      case CMMULT:     // Souèin dvou matic
        if(!(errcode=mmult(&result,&matice1, &matice2))||errcode==EFALSE)
        {
          if(errcode)printf("false\n");
          else printMat(&result);
        }
        break;
      case CMEXPR:     // Maticový výraz A*B*A
        if(!(errcode=mexpr(&result,&matice1, &matice2))||errcode==EFALSE)
        {
          if(errcode)printf("false\n");
          else printMat(&result);
        }
        break;
      case CEIGHT:     // Osmismìrka
        if(!(errcode=eight(&res,&matice2,&matice1)))
        {
          if(res) printf("true\n");
          else    printf("false\n");
        }
        break;
      case CBUBBLES:   // Bubliny
        if(!(errcode=bubbles(&count,&matice1,TYPE_MATRIX)))
        {
          printf("%d\n",count);
        }
        break;
      case CEXTBUBBLES:// Bubliny - roz¹íøení do 3D
        if(!(errcode=bubbles(&count,&matice1,TYPE_3DMATRIX)))
        {
          printf("%d\n",count);
        }
        break;
    }

    if(matice1.matrix!=NULL)freeMat(&matice1);
    if(matice2.matrix!=NULL)freeMat(&matice2);
    if(result.matrix!=NULL)freeMat(&result);
  }

  if (errcode != EOK && errcode != EFALSE)
  { // nìco nestandardního
    printECode(errcode);
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}

/* konec proj3.c */
