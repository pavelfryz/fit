#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <stdarg.h>
#include <stdbool.h>
#include <errno.h>

#include <semaphore.h>
#include <sys/types.h>
#include <sys/mman.h>
#include <sys/wait.h>
#include <signal.h>
#include <fcntl.h>
#include <unistd.h>

#define ARGSPATH "/argspath"
#define SEMSPATH "/semspath"

enum tsemaphores
{
  OUTPUT = 0,
  BARBER,
  READY,
  QUEUE,
  SLEEP,
  ENTER,
  SERVED,
  SEMSCOUNT,
};

enum tecodes
{
  EOK = 0,     /**< Bez chyby */
  ECLWRONG,    /**< Chybny prikazovy radek. */
  ENUMBER,     /**< Chyba pri prevodu ciselnych parametru. */
  EFOPEN,      /**< Chyba pri otevirani souboru. */
  EUNKNOWN,    /**< Neznama chyba */
};

const char *ECODEMSG[] =
{
  /* EOK */
  "Vse v poradku.\n",
  /* ECLWRONG */
  "Chybne parametry prikazoveho radku!",
  /* ENUMBER */
  "Chybny format cisla prikazove radky.",
  /* EFOPEN */
  "Chyba pri otevirani vystupniho souboru.",
  /* EUNKNOWN */
  "Nastala nepredvidana chyba! Vypnete pocitac a rychle utecte.",
};

typedef unsigned long ulong;

typedef struct args_t
{
  ulong chairs;  /**< Pocet zidli. */
  ulong GenC;    /**< Rozsah pro generovani zakazniku [ms]. */
  ulong GenB;    /**< Rozsah pro generovani doby obsluhy [ms]. */
  ulong N;       /**< Pocet zakazniku celkem. */
  ulong toserve; /**< Pocet zakazniku jeste k obslouzeni. */
  ulong action;  /**< Poradove cislo akce. */
  ulong QueC;    /**< Pocet zakazniku ve fronte. */
  bool  sleep;   /**< Zda holic spi. */
  char *FPATH;   /**< Vystupni soubor. */
}args_t;

///////////////////////////////////////////////////////////////////////
args_t *args_init(void);
int args_get(args_t *args, int argc, char *argv[]);
void args_free(args_t *args);
int conversion(const char *s, ulong *l);

sem_t *semaphores_init(void);
void semaphores_clean(sem_t *sems);
void semaphores_free(sem_t *sems);

int barber(sem_t *semaphores, args_t *args);
int customer(ulong i,sem_t *semaphores, args_t *args);

FILE *open_out(const char *path,const char *mode);
void Error(const char *s, const char *fmt, ...);
void clean(void);
///////////////////////////////////////////////////////////////////////

FILE *outfile=NULL; // vystupni soubor
pid_t *childs=NULL; // pole s pid potomku
args_t *args=NULL;  // struktura s paramtery
sem_t *sems=NULL;   // pole se semafory
// ukazatel na fuknci pro uvolneni semaforu
void (*sems_free_p)(sem_t *sems)=semaphores_free;

/**
 * Hlavni program.
 */
int main(int argc, char *argv[])
{
  int ecode=EOK;
  pid_t pid;

  args=args_init(); // inicializace struktury pro parametry
  if(args==NULL)
    return 1;
  sems=semaphores_init(); // inicilalizace semaforu
  if(sems==NULL)
  {
    clean();
    return 1;
  }
  if((ecode=args_get(args,argc,argv))!=EOK) // ziskani paramateru
  {
    Error("main: ","%s",ECODEMSG[ecode]);
    clean();
    return 1;
  }
  if((outfile=open_out(args->FPATH,"a"))==NULL) // otevreni vystupniho souboru
  {
    Error("main: ","%s", strerror(errno));
    clean();
    return 1;
  }
  setbuf(outfile, NULL);

  childs=malloc(sizeof(pid_t)*(args->N+1)); // alokace pole pro pid potomku
  if(childs==NULL)
  {
    Error("main: ","Chyba pri alokaci pameti\n");
    clean();
    return 1;
  }

  pid=fork(); // vytvoreni procesu pro barbera
  if(pid==0)
  { // cast pouze pro potomka - barber
    sems_free_p=semaphores_clean;
    free(childs);
    childs=NULL;
    if(barber(sems,args)==1)
    {
      Error("barber: ","%s", strerror(errno));
      clean();
      return 1;
    }
    clean();
    return 0;
  }
  else
    if(pid==-1)
    { // nepodarilo se vytvorit potomek
      Error("main: ","%s", strerror(errno));
      clean();
      return 1;
    }
    else
      // pridani pid barbera nakonec pole
      childs[args->N]=pid;

  for(ulong i=0;i<args->N;i++)
  {
    pid=fork(); // vytvoreni procesu pro zakaznika
    if(pid==0)
    { // cast pouze pro potomka - zakaznik
      sems_free_p=semaphores_clean;
      free(childs);
      childs=NULL;
      if(customer(i+1,sems,args)==1)
      {
        Error("customer: ","%s", strerror(errno));
        clean();
        return 1;
      }
      clean();
      return 0;
    }
    else
      if(pid==-1)
      { // nepodarilo se vytvorit potomek
        Error("main: ","%s", strerror(errno));
        while(i>0){ kill(childs[i-1], SIGTERM); i--;}
        kill(childs[args->N], SIGTERM);
        clean();
        return 1;
      }
      else
        // pridani pid zakaznika do pole
        childs[i]=pid;
    usleep((rand()%(args->GenC+1))*1000);
  }

  //cekani hlavniho procesu na ukonceni potomku
  for(ulong i=0;i<=args->N;i++)
  {
    int status;
    if((waitpid(childs[i], &status, 0)) < 0 )
    {
      Error("main: ","%s", strerror(errno));
      for(ulong j=i;j<=args->N;j++) kill(childs[j], SIGTERM);
      clean();
      return 1;
    }
    if(WIFEXITED(status))
      if(WEXITSTATUS(status)!=0)
      {
        Error("main :","Za behu potomka doslo k chybe");
        for(ulong j=i;j<=args->N;j++) kill(childs[j], SIGTERM);
        clean();
        return 1;
      }
  }

  clean();
  return 0;
}

/**
 * Funkce inicializuje potrebne semafory.
 * @return Pole semaforu.
 */
sem_t *semaphores_init(void)
{
  int fd;
  sem_t *sems;
  // vytvoreni sdilene pameti pro semafory
  fd=shm_open(SEMSPATH, O_CREAT|O_RDWR, 0777);
  if(fd == -1)
  {
    Error("semaphores_init: ", "%s", strerror(errno));
    return NULL;
  }
  if(shm_unlink(SEMSPATH)==-1)
  {
    Error("semaphores_init: ", "%s", strerror(errno));
    close(fd);
    return NULL;
  }
  // nastaveni velikosti pameti
  if(ftruncate(fd, sizeof(sem_t)*SEMSCOUNT) == -1)
  {
    Error("semaphores_init: ", "%s", strerror(errno));
    close(fd);
    return NULL;
  }
  // namapovani sdilene pameti do pameti
  sems=mmap(NULL, sizeof(sem_t)*SEMSCOUNT, PROT_READ|PROT_WRITE, MAP_SHARED, fd, 0);
  if(sems == MAP_FAILED)
  {
    Error("semaphores_init: ", "%s", strerror(errno));
    close(fd);
    return NULL;
  }
  close(fd);

  for(int i=0;i<SEMSCOUNT;i++)
  {
    // inicializace semaforu
    if(sem_init(&sems[i], 1, 0) == -1)
    {
    Error("semaphores_init: ", "%s", strerror(errno));
    while(i>0){ sem_destroy(&sems[i-1]); i--;}
    munmap(sems,sizeof(sem_t)*SEMSCOUNT);
    return NULL;
  }
  }

  if(sem_post(&sems[OUTPUT])==-1) // otevreni semaforu pro vystup
  {
    semaphores_free(sems);
    Error("semaphores_init: ", "%s", strerror(errno));
    return NULL;
  }
  if(sem_post(&sems[ENTER])==-1) // otevreni semaforu pro vstup do èekárny
  {
    semaphores_free(sems);
    Error("semaphores_init: ", "%s", strerror(errno));
    return NULL;
  }

  return sems;
}

/**
 * Odstrani semafory z pameti.
 * @param sems Pole semaforu.
 */
void semaphores_clean(sem_t *sems)
{
  if(munmap(sems,sizeof(sem_t)*SEMSCOUNT)==-1)
    Error("semaphores_clean: ", "%s", strerror(errno));
}

/**
 * Odstrani semafory.
 * @param sems Pole semaforu.
 */
void semaphores_free(sem_t *sems)
{
  for(int i=0;i<SEMSCOUNT;i++)
    if(sem_destroy(&sems[i])==-1)
      Error("semaphores_free: ", "%s", strerror(errno));
  semaphores_clean(sems);
}

/**
 * Inicializuje strukturu pro parametry.
 * @return Ukazatel na strukturu pro parametry.
 */
args_t *args_init(void)
{
  int fd;
  args_t *args;
  // vytvoreni sdilene pameti pro argumenty
  fd=shm_open(ARGSPATH, O_CREAT|O_RDWR, 0777);
  if(fd == -1)
  {
    Error("args_init: ", "%s", strerror(errno));
    return NULL;
  }
  if(shm_unlink(ARGSPATH)==-1)
  {
    Error("args_init: ", "%s", strerror(errno));
    close(fd);
    return NULL;
  }
  // nastaveni velikosti pameti
  if(ftruncate(fd, sizeof(args_t)) == -1)
  {
    Error("args_init: ", "%s", strerror(errno));
    close(fd);
    return NULL;
  }
  // namapovani sdilene pameti do pameti
  args=mmap(NULL, sizeof(args_t), PROT_READ|PROT_WRITE, MAP_SHARED, fd, 0);
  if(args == MAP_FAILED)
  {
    Error("args_init: ", "%s", strerror(errno));
    close(fd);
    return NULL;
  }
  close(fd);
  args->action=1;
  args->QueC=0;
  args->sleep=false;
  return args;
}

/**
 * Uvolni strukturu s parametry.
 * @param args Ukazatel na strukturu pro parametry.
 */
void args_free(args_t *args)
{
  if(munmap(args,sizeof(args_t))==-1)
    Error("args_free: ", "%s", strerror(errno));
}

/**
 * Zpracuje paramtery z prokazove radky.
 * @param args Cilova struktura pro parametry.
 * @param argc Pocet parametru prikazove radky.
 * @param argv Pole s parametry prikazove radky.
 * @return Kod chyby.
 */
int args_get(args_t *args, int argc, char *argv[])
{
  FILE *f;
  int errcode=EOK;
  if(argc==6)
  {
    if((errcode=conversion(argv[1],&args->chairs   ))!=EOK) return errcode;
    if((errcode=conversion(argv[2],&args->GenC))!=EOK) return errcode;
    if((errcode=conversion(argv[3],&args->GenB))!=EOK) return errcode;
    if((errcode=conversion(argv[4],&args->N   ))!=EOK) return errcode;
    args->toserve=args->N;
    args->FPATH=argv[5];
    if((f=open_out(argv[5],"w"))==NULL)
    {
      Error("args_ger: ", "%s", strerror(errno));
      return EFOPEN;
    }
    if(f!=stdout) fclose(f);
  }
  else return ECLWRONG;
  return EOK;
}

/**
 * Otevre soubor daneho nazvu, v pripade
 * nazvu '-' vraci stdout.
 * @param path Cesta k souboru.
 * @param mode Mod otevirani souboru.
 * @return Ukazatel na otevreny soubor nebo NULL.
 */
FILE *open_out(const char *path,const char *mode)
{
  if(strcmp(path,"-")==0)
    return stdout;
  else
    return fopen(path,mode);
}

/**
 * Prevede retezec na cislo.
 * @param s Retezec.
 * @param l Prevedene cislo.
 * @return Kod chyby.
 */
int conversion(const char *s, ulong *l)
{
  unsigned i=0;
  char *ptr=NULL;
  while(isspace(s[i]))i++;
  if(s[i]=='+')i++;
  if(!isdigit(s[i])) return ENUMBER;
  errno=0;
  *l=strtoul(s+i, &ptr, 10);
  if(errno||*ptr!='\0') return ENUMBER;
  return EOK;
}

/**
 * Uvolnuje vsechny zdroje.
 */
void clean(void)
{
  if(outfile!=NULL&&outfile!=stdout)
  { // zavreni vystupniho souboru
    fclose(outfile);
    outfile=NULL;
  }
  if(childs!=NULL)
  { // uvolneni pameti s id potomku
    free(childs);
    childs=NULL;
  }
  if(args!=NULL)
  { // uvolneni sdilene pameti s argumenty
    args_free(args);
    args=NULL;
  }
  if(sems!=NULL)
  { // smazani semaforu
    sems_free_p(sems);
    sems=NULL;
  }
}

/**
 * Vypi¹e chybovou zpravu na chybovy vystup
 * Funkce ma stejne parametry jako funkce printf(),
 * az na prvni parametr, kterym je uvozovaci retezec.
 * @param s Uvozovaci retezec
 * @param fmt Formatovaci retezec.
 */
void Error(const char *s, const char *fmt, ...)
{
  va_list arg;
  va_start(arg,fmt);
  fprintf(stderr,"%s",s);
  vfprintf(stderr, fmt, arg);
  fprintf(stderr, "\n");
  va_end(arg);
}

/**
 * Funkce pro holice.
 * @param semaphores Pole se semafory.
 * @param args Struktura s parametry.
 * @return chybovy kod.
 */
int barber(sem_t *semaphores, args_t *args)
{
  FILE *f=outfile;
  if(sem_wait(&semaphores[ENTER])==-1) return 1; //cekani na vstup do cekarny
  while(args->toserve>0)
  {//dokud jsou zakaznici k obslouzeni
    if(sem_wait(&semaphores[OUTPUT])==-1) return 1;//cekani na vystup
    fprintf(f,"%lu: barber: checks\n",args->action++);
    if(sem_post(&semaphores[OUTPUT])==-1) return 1;//uvolneni vystupu
    if(args->chairs==0) break;//zadna zidle v cekarne, koncim
    if(args->QueC==0)
    {//nikdo neni v cekarne
      args->sleep=true;
      if(sem_post(&semaphores[ENTER])==-1) return 1;//uvolneni vstupu do cekarny
      if(sem_wait(&semaphores[SLEEP])==-1) return 1;//spani holice
    }
    else
    {//v cekarne nekdo je
      if(sem_post(&semaphores[ENTER])==-1) return 1;//uvoleni vstupu do cekarny
    }
    if(sem_post(&semaphores[QUEUE])==-1) return 1; //poslani dalsiho zakaznika
    if(sem_wait(&semaphores[BARBER])==-1) return 1;//cekani na zakaznika
    if(sem_wait(&semaphores[ENTER])==-1) return 1; //cekani na vstup do cekarny
    if(sem_wait(&semaphores[OUTPUT])==-1) return 1;//cekani na vystup
    fprintf(f,"%lu: barber: ready\n",args->action++);
    if(sem_post(&semaphores[OUTPUT])==-1) return 1;//uvolneni vystupu
    args->QueC--; //snizeni poctu zakazniku v cekarne
    if(sem_post(&semaphores[ENTER])==-1) return 1; //uvolneni vstupu do cekarny
    if(sem_post(&semaphores[READY])==-1) return 1; //informoani o pripravenosti
    if(sem_wait(&semaphores[BARBER])==-1) return 1;//cekani nez se zakaznik pripravi
    usleep((rand()%(args->GenB+1))*1000);          //simulace strihani
    if(sem_wait(&semaphores[OUTPUT])==-1) return 1;//cekani na vystup
    fprintf(f,"%lu: barber: finished\n",args->action++);
    if(sem_post(&semaphores[OUTPUT])==-1) return 1;//uvolneni vystupu
    if(sem_post(&semaphores[SERVED])==-1) return 1;//informovani o dokonceni strihani
    if(sem_wait(&semaphores[ENTER])==-1) return 1; //cekani na vstup do cekarny
    args->toserve--;//snizeni poctu zakazniku k obsluze
  }
  if(sem_post(&semaphores[ENTER])==-1) return 1;   //uvolneni vstupu do cekarny
  return EOK;
}

/**
 * Funkce pro zakaznika.
 * @param id Id zakaznika.
 * @param semaphores Pole se semafory.
 * @param args Struktura s parametry.
 * @return chybovy kod.
 */
int customer(ulong id, sem_t *semaphores, args_t *args)
{
  FILE *f=outfile;
  if(sem_wait(&semaphores[OUTPUT])==-1) return 1;//cekani na vystup
  fprintf(f,"%lu: customer %lu: created\n",args->action++,id);
  if(sem_post(&semaphores[OUTPUT])==-1) return 1;//cekani na vstup
  if(sem_wait(&semaphores[ENTER])==-1) return 1; //cekani na vstup do cekarny
  if(sem_wait(&semaphores[OUTPUT])==-1) return 1;//cekani na vystup
  fprintf(f,"%lu: customer %lu: enters\n",args->action++,id);
  if(sem_post(&semaphores[OUTPUT])==-1) return 1;//uvolneni vystupu
  if(args->QueC<args->chairs)
  { //dostatek zidli v cekarne
    args->QueC++;  //zvyseni poctu zakazniku v cekarne
    if(args->sleep)//zjisteni zda holic spi
    {
      if(sem_post(&semaphores[SLEEP])==-1) return 1;//probuzeni holice
      args->sleep=false;
    }
    if(sem_post(&semaphores[ENTER])==-1) return 1; //uvolneni vstupu do cekarny
    if(sem_wait(&semaphores[QUEUE])==-1) return 1; //cekani ve fronte
    if(sem_post(&semaphores[BARBER])==-1) return 1;//informovani holice ze jsem na rade
    if(sem_wait(&semaphores[READY])==-1) return 1; //cekani nez se holic pripravi
    if(sem_wait(&semaphores[OUTPUT])==-1) return 1;//cekani na vystup
    fprintf(f,"%lu: customer %lu: ready\n",args->action++,id);
    if(sem_post(&semaphores[OUTPUT])==-1) return 1;//uvolneni vystupu
    if(sem_post(&semaphores[BARBER])==-1) return 1;//informovani holice o pripravenosti
    if(sem_wait(&semaphores[SERVED])==-1) return 1;//cekani na dokonceni strihani
    if(sem_wait(&semaphores[OUTPUT])==-1) return 1;//cekani na vystup
    fprintf(f,"%lu: customer %lu: served\n",args->action++,id);
    if(sem_post(&semaphores[OUTPUT])==-1) return 1;//uvolneni vystupu
  }
  else
  { // malo mista v cekarne
    args->toserve--; // snizeni poctu zakazniku k obslouzeni
    if(sem_wait(&semaphores[OUTPUT])==-1) return 1;//cekani na vystup
    fprintf(f,"%lu: customer %lu: refused\n",args->action++,id);
    if(sem_post(&semaphores[OUTPUT])==-1) return 1;//uvolneni vystupu
    if(sem_post(&semaphores[ENTER])==-1) return 1; //uvolneni vstupu do cekarny
  }
  return EOK;
}
