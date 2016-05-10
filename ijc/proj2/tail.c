/*
 * Soubor:  tail.c
 * Datum:   11.4.2011
 * Autor:   Pavel Fryz, xfryzp00@stud.fit.vutbr.cz, FIT
 * Projekt: IJC-DU2
 * Popis:   Program pro vypis poslednich N radku souboru, nebo od N-teho
 *          radku (obdoba k programu POSIX tail v linuxech).
 */

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <stdbool.h>
#include <string.h>
#include <ctype.h>

// Maximalni delka radku
#define LINE_LENGTH 1024

/** Kody chyb programu */
enum tecodes {
  EOK = 0,     /**< Bez chyby */
  ECLWRONG,    /**< Chybny prikazovy radek. */
  ELINE,       /**< Prekrocena max delka radku. */
  EALLOC,      /**< Chyba alokace pameti. */
  ENUMBER,     /**< Chybny pocet radku. */
  EFOPEN,      /**< Chyba pri otevirani souboru. */
  EUNKNOWN,    /**< Neznama chyba */
};

/** Stavove kody programu */
enum tstates {
  CTAIL,     /**< Tisk poslednich N radku */
  CAFTER,    /**< Tisk od N-teho radku */
};

/** Chybova hlaseni odpovidajici chybovym kodum. */
const char *ECODEMSG[] = {
  /* EOK */
  "Vse v poradku.\n",
  /* ECLWRONG */
  "Chybne parametry prikazoveho radku!\n",
  /* ELINE */
  "Varovani: Prekrocena max delka radku.\n",
  /* EALLOC */
  "Chyba pri alokaci pameti.\n",
  /* ENUMBER */
  "Chybny pocet radku.\n",
  /* EFOPEN */
  "Chyba pri otevirani vstupniho souboru.\n",
  /* EUNKNOWN */
  "Nastala nepredvidana chyba! Vypnete pocitac a rychle utecte.\n",
};

/**
 * Struktura obsahujici hodnoty parametru prikazove radky.
 */
typedef struct opts_t {
  unsigned long N;  /**< Hodnota N z prikazove radky. */
  char *filename;   /**< Nazev vstupniho souboru. */
  int ecode;        /**< Chybovy kod programu, odpovida vyctu tecodes. */
  int state;        /**< Stavovy kod programu, odpovida vyctu tstates. */
} opts_t;

///////////////////////////////////////////////////////////////////////
void after(FILE * f, unsigned long N);
int tail(FILE * f, unsigned long N);
int freadline(char *s, int max, FILE * f);
opts_t getopts(int argc, char *argv[]);
void getopts_lines(const char *s, opts_t * opts);
void printECode(int ecode);
///////////////////////////////////////////////////////////////////////

/**
 * Hlavni program.
 */
int main(int argc, char *argv[])
{
  opts_t opts = getopts(argc, argv);
  if (opts.ecode != EOK) {      // neco nestandardniho
    printECode(opts.ecode);
    return EXIT_FAILURE;
  }
  FILE *f = stdin;
  if (opts.filename != NULL)
    if ((f = fopen(opts.filename, "r")) == NULL) {  // chyba pri otevirani souboru
      printECode(EFOPEN);
      return EXIT_FAILURE;
    }
  if (opts.state == CAFTER)
    // tisk od N teho radku
    after(f, opts.N);
  else {                        // tisk poslednich N radku
    int errcode = EOK;
    errcode = tail(f, opts.N);
    if (errcode != EOK) {       // neco nestandardniho
      if (f != stdin)
        fclose(f);
      printECode(opts.ecode);
      return EXIT_FAILURE;
    }
  }
  if (f != stdin)
    fclose(f);
  return EXIT_SUCCESS;
}

/**
 * Vypise obsah souboru od N-teho radku vcetne.
 * @param f Vstupni soubor.
 * @param N Cislo radku.
 */
void after(FILE * f, unsigned long N)
{
  int c;
  // preskoci prvnich N-1 radku
  while (N != 1 && (c = getc(f)) != EOF)
    if (c == '\n')
      N--;
  // a vytiskne vsechny nasledujici znaky
  while ((c = getc(f)) != EOF)
    putc(c, stdout);
}

/**
 * Vypise poslednich N radku souboru.
 * @param f Vstupni soubor.
 * @param N Cislo radku.
 * @return Kod chyby.
 */
int tail(FILE * f, unsigned long N)
{
  char **buffer;
  unsigned long index = 0, i = 0;
  // Alokace bufferu
  if ((buffer = malloc(N * sizeof(char *))) == NULL)
    // chyba alokace
    return EALLOC;
  for (index = 0; index < N; index++)
    if ((buffer[index] = malloc(LINE_LENGTH * sizeof(char))) != NULL)
      // "vynulovani" bufferu, nenacitam znak '\n'
      buffer[index][0] = '\n';
    else {                      // chyba alokace, uvolneni jiz naalokovane pameti
      for (i = 0; i < index; i++)
        free(buffer[i]);
      free(buffer);
      return EALLOC;
    }
  index = 0;
  // nacitani radku do bufferu
  while (freadline(buffer[index], LINE_LENGTH, f) != -1)
    index = (index + 1) % N;
  // vypis radku bufferu a uvolneni pameti
  while (buffer[index] != NULL) {
    if (buffer[index][0] != '\n')
      puts(buffer[index]);
    free(buffer[index]);
    buffer[index] = NULL;
    index = (index + 1) % N;
  }
  free(buffer);
  return EOK;
}

/**
 * Nacte jeden radek omezene delky ze souboru (bez znaku '\n').
 * Pri prekroceni max delky, zbytek znaku zahodi
 * a vypise varovani na stderr.
 * @param s Cilovy retezec.
 * @param max Limit delky radku.
 * @param f Vstupni soubor.
 * @return Pocet nactenych znaku, -1 pri EOF.
 */
int freadline(char *s, int max, FILE * f)
{
  int i = 0, c;
  static bool eline = false;
  while ((c = getc(f)) != EOF && c != '\n') {
    if (i < max - 1) {
      s[i] = c;
      i++;
    }
    else if (!eline) {          // tisk varovani o prekroceni max delky radku
      printECode(ELINE);
      eline = true;
    }
  }
  if (feof(f))
    return -1;
  s[i] = '\0';
  return i;
}

/**
 * Zpracuje argumenty prikazoveho radku a vrati je ve strukture opts_t.
 * @param argc Pocet argumentu.
 * @param argv Pole textovych retezcu s argumenty.
 * @return Analyzovane argumenty prikazoveho radku.
 */
opts_t getopts(int argc, char *argv[])
{
  opts_t opts = {               // inicializace struktury
    .N = 10,
    .filename = NULL,
    .ecode = EOK,
    .state = CTAIL,
  };
  switch (argc) {
  case 1:                      // zadny parametr
    break;
  case 2:                      // pouze vstupni soubor
    opts.filename = argv[1];
    break;
  case 3:                      // parametr n+cislo
    if (strcmp("-n", argv[1]) == 0)
      getopts_lines(argv[2], &opts);
    else
      opts.ecode = ECLWRONG;
    break;
  case 4:                      // parametry n+cislo a nazev souboru
    if (strcmp("-n", argv[1]) == 0) { // prvne n+cislo, potom soubor
      getopts_lines(argv[2], &opts);
      opts.filename = argv[3];
    }
    else if (strcmp("-n", argv[2]) == 0) {  // prvne soubor, potom n+cislo
      getopts_lines(argv[3], &opts);
      opts.filename = argv[1];
    }
    else
      opts.ecode = ECLWRONG;
    break;
  default:                     // prilis mnoho parametru
    opts.ecode = ECLWRONG;
    break;
  }
  return opts;
}

/**
 * Analyzuje retezec s a prevede na cislo.
 * Pokud cislo zacina znakem '+' nastavi stavovy kod programu na CAFTER.
 * @param s Retezec.
 * @param N Prevedene cislo.
 * @param state Stavovy kod programu.
 */
void getopts_lines(const char *s, opts_t * opts)
{
  unsigned i = 0;
  char *ptr = NULL;
  // preskoceni bilych znaku
  while (isspace(s[i]))
    i++;
  // preskoceni znaku '-'
  if (s[i] == '-')
    i++;
  // preskoceni znaku '+' a nastaveni stavu na tisk od N teho radku
  else if (s[i] == '+') {
    opts->state = CAFTER;
    i++;
  }
  // dalsi znak musi byt cislice
  if (!isdigit(s[i]))
    opts->ecode = ENUMBER;
  errno = 0;
  opts->N = strtoul(s + i, &ptr, 10);
  if (errno || *ptr != '\0')
    opts->ecode = ENUMBER;
}

/**
 * Vytiskne hlaseni odpovidajici chybovemu kodu.
 * @param ecode kod chyby programu
 */
void printECode(int ecode)
{
  if (ecode < EOK || ecode > EUNKNOWN)
    ecode = EUNKNOWN;
  fprintf(stderr, "%s", ECODEMSG[ecode]);
}
