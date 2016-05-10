/*!
\file bpnet.cpp
Hlavickovy soubor k tride BPNet, ktera reprezentuje neuronovaou sit, ktera
umoznuje uceni metodou backpropagation. 
\author Pavel Frýz
\date 28. 11. 2014
*/

#ifndef BPNET_H
#define BPNET_H

#include <utility>
#include <vector>

class BPNet
{
  public:
    //! Vytvori sit
    /*!
    Vytvori s odpovidajicim poctem vstupnich neuronu a se zadanym poctem vrstev,
    kazda vrstva oobsahuje zadany pocet neuronu.
    \param inputs Pocet vstupu
    \param layers Pocet vrstev
    \param neuronsInLayers Pocet neuronu v jednotlivych vrstvach
    */
    BPNet(int inputs, int layers, int* neuronsInLayers);
    
    //! Smaze sit
    /*!
    Smaze sit a uvolni pamet, kterou zabirala.
    */
    virtual ~BPNet();
    
    //! Zpocita odezvu site
    /*!
    Pro prilozeny vstupni vektor spocita postupne hodnoty jednotlivych neuronu az po vystupni vrstvu
    \param input Vstupni vektor
    */
    void feedForward(const double* input);
    
    //! Zpetna propagece chyby site
    /*!
    Provede zpetnou propagaci chyby site a upravi pripadne vahy mezi neurony
    \param input Vstupni vektor
    \param output Odpovidajici vystupni vektor ke vstupnimu vektoru
    \return Chybu site
    */
    double backPropagate(const double* input, const double* output); 
    
    //! Vraci odezvu site
    /*!
    Prilozi vektor na vstup, spocita odezvu a vrati vysledny vektor
    \param input Vstupni vektor
    \return Vektor s odezvou site
    */
    double* getOutput(const double* input);
    
    //! Uci sit
    /*!
    Uci sit pomoci algortimu backpropagation
    \param trainingSet Mnozina se vstupy a k nim odpovidajicimi vystupy
    */
    void train(std::vector <std::pair<double* ,double* > > trainingSet);
    
    //! Nacte sit
    /*!
    Nacte sit ze souboru
    \param filename Soubor se siti
    \return Jesli se povedlo nacist sit
    */
    bool loadFile(const char* filename);
    
    //! Uklada sit
    /*!
    Uklada sit do souboru
    \param filename Cilovy soubor
    \return Jesli se povedlo sit ulozit
    */
    bool saveFile(const char* filename);
    
    double learningRate; /*!< Koeficient uceni*/
    double momentum; /*!< Koeficient momenta*/
    int inputCount; /*!< Velikost vstupu*/
    int outputCount; /*!< Velikost vystupu*/
    unsigned int maxSteps; /*!< Maximani pocet kroku uceni*/
    unsigned int maxTime; /*!< Maximalni doba uceni v sekundach*/
    double maxError; /*!< Pozadovana maximalni chyba naucene site*/
    bool batch; /*!< Povoluje davkovou upravu vah*/
  private:
    int layerCount; /*!< Pocet vrstev*/
    int*  neuronCounts; /*!< Pocet neuronu v jednotlivych vrstvach*/
    double** neurons; /*!< Neurony site*/
    double*** weights; /*!< Vahy mezi neurony*/
    double*** weightDeltas; /*!< Zmena vahy mezi neurony*/
    double** errorGradients; /*!< Gradient chyby jednotlivych neuronu*/
};

#endif /* BPNET_H */ 
