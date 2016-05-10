/*
 * Soubor:  tail2.cc
 * Datum:   18.4.2011
 * Autor:   Pavel Fryz, xfryzp00@stud.fit.vutbr.cz, FIT
 * Projekt: IJC-DU2
 * Popis:   Program pro vypis poslednich N radku souboru, nebo od N-teho
 *          radku (obdoba k programu POSIX tail v linuxech).
 */

#include <string>
#include <iostream>
#include <fstream>
#include <deque>
#include <locale>
#include <cstdlib>
#include <cerrno>
using namespace std;

/** Kody chyb programu */
enum tecodes {
  EOK = 0,     /**< Bez chyby */
  ECLWRONG,    /**< Chybny prikazovy radek. */
  ENUMBER,     /**< Chybny pocet radku. */
  EFOPEN,      /**< Chyba pri otevirani souboru. */
  EUNKNOWN     /**< Neznama chyba */
};

/** Stavove kody programu */
enum tstates {
  CTAIL,     /**< Tisk poslednich N radku */
  CAFTER     /**< Tisk od N-teho radku */
};

/** Chybova hlaseni odpovidajici chybovym kodum. */
string ECODEMSG[] = {
  /* EOK */
  "Vse v poradku.\n",
  /* ECLWRONG */
  "Chybne parametry prikazoveho radku!\n",
  /* ENUMBER */
  "Chybny pocet radku.\n",
  /* EFOPEN */
  "Chyba pri otevirani vstupniho souboru.\n",
  /* EUNKNOWN */
  "Nastala nepredvidana chyba! Vypnete pocitac a rychle utecte.\n"
};

/**
 * Struktura obsahujici hodnoty parametru prikazove radky.
 */
struct opts_t {
  unsigned long N; /**< Hodnota N z prikazove radky. */
  char *file;      /**< Vstupni soubor. */
  int ecode;       /**< Chybovy kod programu, odpovida vyctu tecodes. */
  int state;     /**< Stavovy kod programu, odpovida vyctu tstates. */
};

///////////////////////////////////////////////////////////////////////
void after(istream & in, unsigned long N);
void tail(istream & in, unsigned long N);
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
  ifstream inFile;

  if (opts.ecode != EOK) {      // neco nestandardniho
    printECode(opts.ecode);
    return opts.ecode;
  }

  if (opts.file == NULL) {      // nebyl zadan vstupni soubor
    if (opts.state == CAFTER)
      // tisk od N teho radku
      after(cin, opts.N);
    else
      // tisk poslednich N radku
      tail(cin, opts.N);
  }
  else {                        // byl zadan vstupni soubor
    inFile.open(opts.file);
    if (inFile.fail()) {        // nepodarilo se otevrit vystupni soubor
      printECode(EFOPEN);
      return EFOPEN;
    }
    if (opts.state == CAFTER)
      // tisk od N teho radku
      after(inFile, opts.N);
    else
      // tisk poslednich N radku
      tail(inFile, opts.N);
    inFile.close();
  }
  return EOK;
}

/**
 * Vypise obsah souboru od N-teho radku vcetne.
 * @param in Vstupni soubor.
 * @param N Cislo radku.
 */
void after(istream & in, unsigned long N)
{
  string line;
  // preskoci prvnich N-1 radku
  while (N != 1 && getline(in, line))
    N--;
  // a zbytek vytiskne
  while (getline(in, line))
    cout << line << endl;
}

/**
 * Vypise poslednich N radku souboru.
 * @param in Vstupni soubor.
 * @param N Cislo radku.
 */
void tail(istream & in, unsigned long N)
{
  deque < string > buffer;
  string line;
  // cteni radku
  while (getline(in, line)) {
    // pridani radku nakonec seznamu
    buffer.push_back(line);
    // odstraneni prebytecnych radku ze seznamu
    if (buffer.size() > N)
      buffer.pop_front();
  }
  // vypis vsech radku v seznamu  
  for (deque < string >::iterator i = buffer.begin(); i != buffer.end(); ++i)
    cout << *i << endl;
}

/**
 * Zpracuje argumenty prikazoveho radku a vrati je ve strukture opts_t.
 * @param argc Pocet argumentu.
 * @param argv Pole textovych retezcu s argumenty.
 * @return Analyzovane argumenty prikazoveho radku.
 */
opts_t getopts(int argc, char *argv[])
{
  opts_t opts = {
    10,
    NULL,
    EOK,
    CTAIL
  };
  switch (argc) {
  case 1:                      // zadny parametr 
    break;
  case 2:                      // pouze vstupni soubor
    opts.file = argv[1];
    break;
  case 3:                      // parametr n+cislo
    if (argv[1] == string("-n"))
      getopts_lines(argv[2], &opts);
    else
      opts.ecode = ECLWRONG;
    break;
  case 4:                      // parametry n+cislo a nazev souboru
    if (argv[1] == string("-n")) {  // prvne n+cislo, potom soubor
      getopts_lines(argv[2], &opts);
      opts.file = argv[3];
    }
    else if (argv[2] == string("-n")) { // prvne soubor, potom n+cislo
      getopts_lines(argv[3], &opts);
      opts.file = argv[1];
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
  cerr << ECODEMSG[ecode];
}
