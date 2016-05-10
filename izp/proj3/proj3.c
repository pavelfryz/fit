/*
 * Soubor:  proj3.c
 * Datum:   2010/11/29
 * Autor:   Pavel Fr�z, xfryzp00@fit.vutbr.cz
 * Projekt: Maticov� operacace, projekt �. 3 pro p�edm�t IZP
 * Popis:   Program na��t� vektory, matice nebo vektory matic
 * ze souboru a prov�d� vybran� maticov� operace:
 * Vektorov� sou�et dvou vektor� (a+b)
 * Skal�rn� sou�in dvou vektor� (a*b)
 * Sou�in matic (A*B)
 * Maticov� v�raz (A*B*A)
 * Po�et bublin(seskupen� nul) v matici, vektoru matic
 * Osmism�rka, hled� vektor v matici
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

#define TYPE_VECTOR   1
#define TYPE_MATRIX   2
#define TYPE_3DMATRIX 3

#define getAxis(matrix,dim,row,col) ((dim)*(matrix).rows*(matrix).cols+(row)*(matrix).cols+(col))
#define getAxisP(matrix,dim,row,col) ((dim)*(matrix)->rows*(matrix)->cols+(row)*(matrix)->cols+(col))

/** K�dy chyb programu */
enum tecodes
{
  EOK = 0,     /**< Bez chyby */
  ECLWRONG,    /**< Chybn� p��kazov� ��dek. */
  EFOPEN,      /**< Chyba, soubor nenalezen. */
  EBADVAL,     /**< Chyba, spatny format dat. */
  EALLOC,      /**< Chyba pam�ti */
  EPARAM,      /**< Chybn� parametry */
  EFALSE,      /**< Operace nema platny vysledek */
  EUNKNOWN,    /**< Nezn�m� chyba */
};

/** K�dy sm�r� */
enum tdirection
{
  DS = 0, /**< Sever */
  DSV,    /**< Severo-vychod */
  DV,     /**< V�chod */
  DJV,    /**< Jiho-v�chod */
  DJ,     /**< Jih */
  DJZ,    /**< Jiho-z�pad */
  DZ,     /**< Z�pad */
  DSZ,    /**< Severo-z�pad */
};

/** Stavov� k�dy programu */
enum tstates
{
  CHELP,        /**< N�pov�da */
  CTEST,        /**< Testovac� v�stup */
  CVADD,        /**< Vektorov� sou�et */
  CVSCAL,       /**< Skal�rn� sou�in vektor� */
  CMMULT,       /**< Sou�in dvou matic */
  CMEXPR,       /**< Maticov� v�raz A*B*A */
  CEIGHT,       /**< Osmism�rka */
  CBUBBLES,     /**< Bubliny */
  CEXTBUBBLES,  /**< Bubliny - roz���en� do 3D */
};

/** Chybov� hl�en� odpov�daj�c� chybov�m k�d�m. */
const char *ECODEMSG[] =
{
  /* EOK */
  "V�e v po��dku.\n",
  /* ECLWRONG */
  "Chybn� parametry p��kazov�ho ��dku!\n",
  /* EFOPEN */
  "Chyba p�i otev�r�n� souboru! Soubor nenalezen\n",
  /* EBADVAL */
  "Chybn� form�t vstupn�ho souboru!\n",
  /* EALLOC */
  "Nedostatek pam�ti\n",
  /* EPARAM */
  "Chybn� operandy! Operaci nelze prov�st.\n",

  /* EUNKNOWN */
  "Nastala nep�edv�dan� chyba! Vypn�te po��ta� a rychle ute�te.\n",
};

const char *HELPMSG =
  "Program Maticov� operace.\n"
  "Autor: Pavel Fr�z (c) 2010\n"
  "Program na��t� vektory, matice nebo vektory matic\n"
  "ze souboru a prov�d� vybran� maticov� operace:\n"
  "Vektorov� sou�et dvou vektor� (a+b)\n"
  "Skal�rn� sou�in dvou vektor� (a*b)\n"
  "Sou�in matic (A*B)\n"
  "Maticov� v�raz (A*B*A)\n"
  "Po�et bublin(seskupen� nul) v matici, vektoru matic\n"
  "Osmism�rka, hled� vektor v matici\n"
  "Pou�it�: proj3 -h\n"
  "         proj3 -c N\n"
  "         proj3 --test data.txt\n"
  "         proj3 --vadd a.txt b.txt\n"
  "         proj3 --vscal a.txt b.txt\n"
  "         proj3 --mmult A.txt B.txt\n"
  "         proj3 --mexpr A.txt B.txt\n"
  "         proj3 --eight v.txt M.txt\n"
  "         proj3 --bubbles M.txt\n" 
  "         proj3 --extbubbles 3DM.txt\n"
  "Popis parametr�:\n"
  "-h Vytiskne n�pov�du k pou�it� programu.\n" 	 
  "--test data.txt Zkontroluje vstupn� data, vytiskne je na standardn� v�stup.\n"
  "--vadd a.txt b.txt Se�te dva vektory (a+b).\n"
  "--vscal a.txt b.txt Vypo�te skal�rn� sou�in (a*b) dvou vektor�.\n"
  "--mmult A.txt B.txt Vypo�te sou�in dvou matic v dan�m po�ad� (A*B).\n" 
  "--mexpr A.txt B.txt Vypo�te maticov� v�raz (A*B*A).\n"
  "--eight v.txt M.txt Vyhled� v matici M vektor v.\n"
  "--bubbles M.txt Spo��t� po�et bublin v matici M.\n"
  "--extbubbles 3DM.txt Spo��t� po�et bublin v prostoru 3DM.\n";

/**
 * Struktura obsahuj�c� hodnoty parametr� p��kazov� ��dky.
 */
typedef struct params
{
  char * filename1; /**< N�zev prvn�ho souboru. */
  char * filename2; /**< N�zev druh�ho souboru. */
  int ecode;        /**< Chybov� k�d programu, odpov�d� v��tu tecodes. */
  int state;        /**< Stavov� k�d programu, odpov�d� v��tu tstates. */
} TParams;


/**
 * Struktura pro pr�ci s maticemi.
 */
typedef struct tmatrix
{
  unsigned int rows;     /**< Po�et ��dk�. */
  unsigned int cols;     /**< Po�et sloupc�. */
  unsigned int nums;     /**< Po�et matic. */
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
 * Funkce alokuje m�sto v pam�ti pro matici.
 * Rozm�ry matice mus� b�t uvedeny ve struktu�e matrix.
 * @param matrix Struktura pro matici .
 * @return Vrac� k�d chyby.
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
 * Funkce uvoln� pam� d��ve alokovan� matice.
 * @param matrix Uvol�ovan� matice.
 * @return Vrac� k�d chyby.
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
 * Funkce na�te matici ze souboru.
 * @param matrix Matice.
 * @param filename N�zev souboru.
 * @return Vrac� k�d chyby.
 */
int loadMat(TMatrix * matrix, const char *filename)
{
  matrix->cols=1;
  matrix->rows=1;
  matrix->nums=1;
  matrix->matrix=NULL;
  if(filename==NULL) return EOK;

  //otev�en� souboru
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
 * Funkce vytiskne matici na standartn� v�stup.
 * @param matrix Matice.
 * @param filename N�zev souboru.
 * @return Vrac� k�d chyby.
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
 * @return Vrac� kod chyby.
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
 * @return Vrac� hodnotu na souradnici [num][row][col].
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
 * Vytiskne hl�en� odpov�daj�c� chybov�mu k�du.
 * @param ecode k�d chyby programu
 */
void printECode(int ecode)
{
  if (ecode < EOK || ecode > EUNKNOWN)
  { ecode = EUNKNOWN; }

  fprintf(stderr, "%s", ECODEMSG[ecode]);
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
    .filename1 = NULL,
    .filename2 = NULL,
    .ecode = EOK,
    .state = CHELP,
  };
  if (argc == 2 && strcmp("-h", argv[1]) == 0)
  { // tisk n�pov�dy
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
  { // t�i parametry
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
  { // p��li� mnoho parametr�
    result.ecode = ECLWRONG;
  }

  return result;
}

/**
 * Funkce se�te dva vektory.
 * @param vect1 Prvn� vektor.
 * @param vect2 Druh� vektor.
 * @param result V�sledek operace.
 * @return Vrac� k�d chyby.
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
 * Funkce vypo�te skal�rn� sou�in dvou vektor�.
 * @param vect1 Prvn� vektor.
 * @param vect2 Druh� vektor.
 * @param result V�sledek operace.
 * @return Vrac� k�d chyby.
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
 * Funkce vyn�sob� dv� matice.
 * @param matrix1 Prvn� matice.
 * @param matrix2 Druh� matice.
 * @param result V�sledek operace.
 * @return Vrac� k�d chyby.
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
 * Funkce vypo�te maticov� v�raz A*B*A.
 * @param matrix1 Matice A.
 * @param matrix2 Matice B.
 * @param result V�sledek operace.
 * @return Vrac� k�d chyby.
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
 * Funkce vyhled� vektor v matici. Pokud vektor najde
 * nastav� v�sledek na hodnotu true, jinak na hodnotu false.
 * @param matrix Matice.
 * @param vect Hledan� vektor.
 * @param result V�sledek funkce.
 * @return Vrac� k�d chyby.
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
 * Funkce zji��uje zda se vector 
 * nach�z� v matici v uveden�m sm�ru.
 * @param matrix Matice.
 * @param vect Hledan� vektor.
 * @param row Sou�adnice ��dku.
 * @param row Sou�adnice sloupce.
 * @param direction Sm�r hled�n�.
 * @return Vrac� v�sledek hled�n�, true=nalezeno, false=nenalezeno.
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
 * Funkce spo��t� po�et bublin v matici, p��padn�
 * vekroru matic.
 * @param matrix Matice.
 * @param type Typ matice, bu� TYPE_MATRIX, nebo TYPE_3DMATRIX.
 * @param result Po�et bublin.
 * @return Vrac� k�d chyby.
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
 * Funkce vypln� bublinu jedni�kami.
 * @param matrix Matice.
 * @param num Sou�adnice matice ve vektoru matic.
 * @param row Sou�adnice ��dku.
 * @param col Sou�adnice sloupce.
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
      case CVADD:      // Vektorov� sou�et
        if(!(errcode=vadd(&result,&matice1,&matice2))||errcode==EFALSE)
        {
          if(errcode)printf("false\n");
          else printMat(&result);
        }
        break;
      case CVSCAL:     // Skal�rn� sou�in vektor�
        if(!(errcode=vscal(&count,&matice1,&matice2))||errcode==EFALSE)
        {
          if(errcode)printf("false\n");
          else printf("%d\n",count);
        }
        break;
      case CMMULT:     // Sou�in dvou matic
        if(!(errcode=mmult(&result,&matice1, &matice2))||errcode==EFALSE)
        {
          if(errcode)printf("false\n");
          else printMat(&result);
        }
        break;
      case CMEXPR:     // Maticov� v�raz A*B*A
        if(!(errcode=mexpr(&result,&matice1, &matice2))||errcode==EFALSE)
        {
          if(errcode)printf("false\n");
          else printMat(&result);
        }
        break;
      case CEIGHT:     // Osmism�rka
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
      case CEXTBUBBLES:// Bubliny - roz���en� do 3D
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
  { // n�co nestandardn�ho
    printECode(errcode);
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}

/* konec proj3.c */
