/*
 * Soubor:  alc.cpp
 * Datum:   8.12.2012  
 * Autori:  xbartl03, xbartl03@stud.fit.vutbr.cz
 *          xfryzp00, xfryzp00@stud.fit.vutbr.cz
 * Projekt: Reseni prohibicni krize v CR
 */

#include <ctime>
#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <unistd.h>
#include "simlib.h"
#include <vector>

const char *usage = 
    "pouziti: ./alc [-o <vystup>] [i <n>] [l <n>] [t <dny>] [r <n>] [s]\n"
    "VOLBY\n"
    "  -i pocet inspektoru\n"
    "  -l pocet laboratori\n"
    "  -r pocet Ramanovych spektrometru\n"
    "  -t delka simulace - dnu\n"
    "  -s spektrometry se daji do laboratori, zbyvajici sdili inspektori\n"
    "PRIKLADY\n"
    "  ./alc -o exp.out -i 10 -t 10 -r 5 -l 2 -s\n"
    "    inspekci provadi 10 inspektoru, doba simulace 10 dnu, obe laboratore\n" 
    "    maji spektrometr, zbyvajici 3 spektrometry si berou inspektori podle dostupnosti\n";

const double FAILURE_RATE = 0.05; // procento vadnych vzorku
const double failure_rate = 0.10;
const double SUSPECT_RATE = 0.01; // procento podezrelych prodejen
const double SAMPLE_RATE = 0.60;  // pravdepodobnost, ze se vezme dalsi vzorek
const double WORK_TIME = 8.0;     // pracovni doby inspektora
const double LAB_TIME = 16.0;     // pracovni doba laboratore

int labCount = 2;                 //pocet laboratori
int inspektorCount = 215;         //pocet inspektoru
int spectrometerCount = 0;        //pocet Ramanovych spektrometru
double endTime = 24 * 84;         //doba experimentu - 84 dnu
bool labHaveSpec = false;                    

std::string output = "alc.out";

#define TEST_TIME Uniform(25.0/60.0,35.0/60.0)  // doba analyzy vzorku v laboratori
#define SPEC_TIME Uniform(3.0/60.0,8.0/60.0)    // doba analyzy ramanovym spektrometrem
#define INSPECTION_TIME Uniform(3.5,4.3)        // doba inspekce

#define TEST_COST 950                           // naklady na test v laboratori
#define SPEC_COST 25                            // naklady na test spektrometrem
#define BUY_COST 750000                         // naklady za nakoupeni spektrometru

int costLab = 0;               // naklady za anlyzu v laboratori
int costSpec = 0;              // naklady na analyzu spektrometrem
int costBuy = 0;               // naklady na zakoupeni spektrometru
int success = 0;               // pocet nezavadnych vzorku
int failure = 0;               // pocet vadnych vzorku
int taken = 0;                 // pocet vzatych vzorku
int inspections = 0;           // pocet inspekci
Histogram Tabulka("Doba vzorku v laboratori", 0.0, 3.0/60.0, 15*60/3);



class LabTimer:public Event { // casovac laboratore
  bool labTime;               // ?pracovni doba laboratore
  Process *lab;               // laborator
 public:
  LabTimer(Process * lab):lab(lab), labTime(false) {
    Activate(Time + Uniform(8.0, 10.0));
  } 
  void Behavior() {
    labTime = !labTime;           // uzavre nebo otevre laborator
    if (labTime) {
      lab->Activate();        // otevre laborator
      Activate(Time + LAB_TIME);
    }
    else {
      lab->Activate();        // zavre laborator
      Activate(Time + 24 - LAB_TIME);
    }
  }
};

class Laboratory:public Process { // laborator
  LabTimer timer;                 // casovac pracovni doby laboratore
 public:
  bool haveSpectrometer;          // ?vlastni spektrometr
  Facility device;                // zarizeni laboratore
  Facility spectrometer;          // spektrometr laboratore
  Laboratory(bool haveSpectrometer=false):timer(this),
    haveSpectrometer(haveSpectrometer) {} 
  
  // zabrani zarizeni laboratore a tim
  // znemozneni zpracovani vzorku mimo pracovni dobu
  void Behavior() {
    while(true){
      Passivate();
      Seize(device, 1);
      Seize(spectrometer, 1);
      Passivate();
      Release(device);
      Release(spectrometer);
    }
  }
};

class Sample:public Process { // vzorek zpracovavany v laboratori
  bool tested;                 // ?testovan spektrometrem
  double start;                // cas prijeti vzorku do laboratore
  Laboratory *lab;             // laborator, kde je provadena analyza
 public:
  Sample(Laboratory *lab, bool tested):tested(tested), lab(lab) {
    Activate();
  } 
  void Behavior() {
    start = Time;
    
    if(lab->haveSpectrometer){          // testovani spektrometrem
      Seize(lab->spectrometer);         // zabrani spektrometru
      Wait(SPEC_TIME);                 // testovani
      Release(lab->spectrometer);       // vraceni spektrometru
      costSpec += SPEC_COST;           // naklady na test spektrometrem
      tested = true;                   
      if (Random() >= failure_rate) {
        success++;                     // nezavadny vzorek
        Tabulka(Time - start);         // zaznam doby zpracovani vzorku do tabulky
        return;                        
      }
    }
    
    Seize(lab->device);                 // zabrani laboratorniho zarizeni
    Wait(TEST_TIME);                   // testovani
    Release(lab->device);               // uvolneni zarizeni
    costLab += TEST_COST;              // naklady na test laboratornim zarizenim

    if (tested) {                      // ?testovan spektrometrem
      if (Random() < (FAILURE_RATE / failure_rate)) 
        failure++;                     // zavadny vzorek
      else 
        success++;                     // nezavadny vzorek
    }
    else {
      if (Random() < FAILURE_RATE) 
        failure++;                     // zavadny vzorek
      else 
        success++;                     // nezavadny vzorek
    }
    Tabulka(Time - start);             // zaznam doby zpracovani vzorku do tabulky
  } 
};


class WorkTimer:public Event { // casovac prace inspektora
  bool workTime;               // ?pracovni doba
  Process *inspektor;          // inspektor
 public:
  WorkTimer(Process * inspektor):inspektor(inspektor), workTime(false) {
    Activate(Time + Uniform(8.0, 16.0));
  } 
  bool doWork() {
    return workTime;
  }

  void Behavior() {
    workTime = !workTime;        // ukonceni nebo zapoceti pracovni doby
    if (workTime) {
      inspektor->Activate();     // probudi inspektora
      Activate(Time + WORK_TIME);
    }
    else
      Activate(Time + 24 - WORK_TIME);
  }
};

class Inspektor:public Process { // inspektor
  bool haveSpectrometer;         // ?vlastni spektrometr
  Laboratory *lab;               // refernce na laborator kam posila vzorky
  int samples;                   // pocet vzorku
  int samples_tested;            // pocet vzorku - testovano spektrometrem->podezrele
  WorkTimer work;                // casovac pracovni doby inspektora

 public:
  Inspektor(Laboratory *lab):lab(lab), haveSpectrometer(false), work(this), samples(0),
      samples_tested(0) {} 
  
  void Behavior() {
    while (true) {
      if (work.doWork()) { // pracuje
        if (spectrometerCount > 0 && !haveSpectrometer) { 
          spectrometerCount--;                  // vezme spektrometr      
          haveSpectrometer = true;
        }

        Wait(INSPECTION_TIME);                  // provede inspekci
        if (!work.doWork())
          continue;
        inspections++;
        
        if (Random() < SUSPECT_RATE) {          // pochybeni v prodejne
          do {
            taken++;                            // vezme vzorek
            if (haveSpectrometer) {             // otestuje vzorek
              Wait(SPEC_TIME);                  // testuje vzorek
              if (Random() < failure_rate) 
                samples_tested++;               // vzorek k odeslani - testovany
              else 
                success++;                      // nezavadny vzorek
              costSpec += SPEC_COST; // naklady na test spektrometrem
            }
            else 
              samples++;                        // vzorek k odeslani - netestovany
          } while (Random() < SAMPLE_RATE);     // ?vezme dalsi vzorek
        }
      }
      else {               // konec pracovni doby
        if (haveSpectrometer) {                 
          spectrometerCount++;                  // vrati spektrometr
          haveSpectrometer = false;
        }
        while (samples > 0) {                   // posle vzorky do laboratore - netestovane
          new Sample(lab, false);
          samples--;
        }
        while (samples_tested > 0) {            // posle vzorky do laboratore - testovane
          new Sample(lab, true);
          samples_tested--;
        }
        Passivate();       // ceka na zacatek pracovni doby
      }
    }
  }
};

std::vector<Laboratory *> laboratories;

// funkce pro zpracovani parametru programu
int getArgs(int argc, char **argv)
{
  int ch;
  while ((ch = getopt(argc, argv, "i:l:t:r:so:")) != -1) {
    switch (ch) {
    case 'i': // nastavi pocet inspektoru
      inspektorCount = atoi(optarg);
      break;
    case 'l': // nastavi pocet laboratori
      labCount = atoi(optarg);
      break;
    case 't': // nastavi konec simulace
      endTime = atoi(optarg) * 24.0;
      break;
    case 'r': // nastavi pocet spektrometru
      spectrometerCount = atoi(optarg);
      costBuy += spectrometerCount * BUY_COST;
      break;
    case 's': // spektrometry se prvne rozdeli laboratorim
      labHaveSpec = true;
      break;
    case 'o': // nastavi vystupni soubor
      output = std::string(optarg);
      break;
    default:
      std::cerr << usage;
      return -1;
    }
  }
  if (argc - optind) {
    std::cerr << usage;
    return -1;
  }
  return 0;
}


int main(int argc, char **argv)
{
  if (getArgs(argc, argv))   // zpracovani parametru
    return 1;
  
  SetOutput(output.c_str()); // nastavi vystup
  
  Init(0, endTime); 
  RandomSeed(time(NULL));
  
  // vytvoreni laboratori
  for (int i = 0; i < labCount; ++i) { 
    if(labHaveSpec && spectrometerCount > 0){
      spectrometerCount--;
      // pridani spektrometru do laboratore
      laboratories.push_back(new Laboratory(true));
    }else
      // laborator nebo spektrometr
      laboratories.push_back(new Laboratory(false));
  }
  
  // vytvoreni inspektoru
  for (int i = 0; i < inspektorCount; ++i) {
    // rovnomerne rozdeli inspektory k laboratorim
    int lab = static_cast<int>(Uniform(0,laboratories.size()));
    new Inspektor(laboratories[lab]);
  }
 
  Run();
  
  Print("+----------------------------------------------------------+\n"
        "| VYSLEDKY                                                 |\n"
        "+----------------------------------------------------------+\n"
        "|  Pocet vykonanych inspekci         = %18d  |\n"
        "|  Odebranych vzorku                 = %18d  |\n"
        "|  Nezavadnych vzorku                = %18d  |\n"
        "|  Zavadnych vzorku                  = %18d  |\n"
        "|  Celkove naklady                   = %15d Kc  |\n"
        "|  Naklady na analyzu v laboratori   = %15d Kc  |\n"
        "|  Naklady na analyzu spektrometry   = %15d Kc  |\n"
        "|  Porizovaci naklady                = %15d Kc  |\n"
        "+----------------------------------------------------------+\n",
        inspections,taken,success,failure,costSpec+costLab+costBuy,costLab,
        costSpec,costBuy);
  Tabulka.Output();
  return 0;
}
