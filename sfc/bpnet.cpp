/*!
\file bpnet.cpp
Soubor s implementaci tridy BPNet, ktera reprezentuje neuronovaou sit, ktera
umoznuje uceni metodou backpropagation. 
\author Pavel Frýz
\date 28. 11. 2014
*/
 
#include <algorithm> 
#include <math.h>  
#include <cstdlib>
#include <fstream>  
#include <ctime>
#include <iostream>
#include "bpnet.h"

#define OUTPUT (layerCount - 1)

//! Sigmoidni aktivacni fuknce
double activationFunction(double x)
{
  return 1/(1+exp(-x));
}

//! Derivace sigmoidni funkce
double gradientFunction(double x)
{
  return x*(1-x);
} 

//! Vytvori sit
/*!
Vytvori s odpovidajicim poctem vstupnich neuronu a se zadanym poctem vrstev,
kazda vrstva oobsahuje zadany pocet neuronu.
\param inputs Pocet vstupu
\param layers Pocet vrstev
\param neuronsInLayers Pocet neuronu v jednotlivych vrstvach
*/
BPNet::BPNet(int inputs, int layers, int* neuronsInLayers):inputCount(inputs), layerCount(layers)
{
  batch = false;
  if(layerCount==0) return;
  neuronCounts = new int [layerCount];
  std::copy(neuronsInLayers, neuronsInLayers+layers, neuronCounts);
  
  neurons = new double* [layerCount];
  weights = new double** [layerCount];
  weightDeltas = new double** [layerCount];
  errorGradients = new double* [layerCount];
  
  inputs++;
  for(int i = 0; i < layerCount; i++) {
    int nN = neuronCounts[i];
    neurons[i] = new double [nN];
    weights[i] = new double* [nN];
    weightDeltas[i] = new double* [nN];
    errorGradients[i] = new double [nN];
    
    for(int j = 0; j < nN; j++) {
      weights[i][j] = new double [inputs];
      weightDeltas[i][j] = new double [inputs];
      for(int k = 0; k < inputs; k++) {
        weights[i][j][k] = rand()*(1.0/RAND_MAX)-0.5;
        weightDeltas[i][j][k] = 0;
      }
    }
    inputs = nN + 1;
  }
  outputCount = neuronCounts[OUTPUT];
}

//! Smaze sit
/*!
Smaze sit a uvolni pamet, kterou zabirala.
*/
BPNet::~BPNet()
{
  if(layerCount!=0){
    for(int i = 0; i < layerCount; i++) {
      for(int j = 0; j < neuronCounts[i];j++) {
        delete[] weights[i][j];
        delete[] weightDeltas[i][j];
      }
    }
    
    for(int i = 0; i < layerCount; i++) {
      delete[] neurons[i];
      delete[] weights[i];
      delete[] weightDeltas[i];
      delete[] errorGradients[i];
    }
    
    
    delete[] neurons;
    delete[] weights;
    delete[] weightDeltas;
    delete[] errorGradients;
    
    delete[] neuronCounts;
  }
}

//! Zpocita odezvu site
/*!
Pro prilozeny vstupni vektor spocita postupne hodnoty jednotlivych neuronu az po vystupni vrstvu
\param input Vstupni vektor
*/
void BPNet::feedForward(const double* input)
{
  const double* in = input;
  int inN = inputCount;
  
  for(int l = 0; l < layerCount; l++) {
    double* out = neurons[l];
    int outN = neuronCounts[l];
    double** weight = weights[l]; 
    
    for(int j = 0; j < outN; j++) {
      double u = 0;                           
      for(int i = 0; i < inN; i++) {     
        u += weight[j][i] * in[i]; 
      }
      u += weight[j][inN]; //add bias
      out[j] = activationFunction(u);
    }
    in = out;
    inN = outN;
  }
}

//! Zpetna propagece chyby site
/*!
Provede zpetnou propagaci chyby site a upravi pripadne vahy mezi neurony
\param input Vstupni vektor
\param output Odpovidajici vystupni vektor ke vstupnimu vektoru
\return Chybu site
*/
double BPNet::backPropagate(const double* input, const double* output)
{
  double* out = neurons[OUTPUT];
  double* grad = errorGradients[OUTPUT];
  
  double error = 0;
  for(int i = 0; i < neuronCounts[OUTPUT]; i++) {
    double dOut = (output[i] - out[i]);
    error += dOut * dOut;
    grad[i] = dOut * gradientFunction(out[i]);
  }
  
  for(int l = OUTPUT - 1; l >= 0; l--) {
    for(int j = 0; j < neuronCounts[l]; j++) {
      double sum = 0;
      for(int i = 0; i < neuronCounts[l+1]; i++) {
        sum += errorGradients[l+1][i] * weights[l+1][i][j];
      }
      errorGradients[l][j] = sum * gradientFunction(neurons[l][j]);
    }
  }
  
  const double* in = input;
  int inN = inputCount;
  for(int l = 0; l < layerCount; l++) {
    int nN = neuronCounts[l];
    for(int j = 0; j < nN; j++) {
      for(int i = 0; i < inN; i++) {
        if(batch)
          weightDeltas[l][j][i] += errorGradients[l][j] * in[i];
        else{
          weightDeltas[l][j][i] = learningRate * errorGradients[l][j] * in[i] + momentum * weightDeltas[l][j][i];
          weights[l][j][i] += weightDeltas[l][j][i]; 
        }
      }
      
      if(batch)
        weightDeltas[l][j][inN] += errorGradients[l][j];
      else{
        weightDeltas[l][j][inN] = learningRate * errorGradients[l][j] + momentum * weightDeltas[l][j][inN];
        weights[l][j][inN] += weightDeltas[l][j][inN]; 
      }
    }
    in = neurons[l];
    inN = nN;
  }
  
  return 0.5 * error;
} 

//! Vraci odezvu site
/*!
Prilozi vektor na vstup, spocita odezvu a vrati vysledny vektor
\param input Vstupni vektor
\return Vektor s odezvou site
*/
double* BPNet::getOutput(const double* input)
{
  feedForward(input);
  return neurons[OUTPUT];
}

//! Uci sit
/*!
Uci sit pomoci algortimu backpropagation
\param trainingSet Mnozina se vstupy a k nim odpovidajicimi vystupy
*/
void BPNet::train(std::vector <std::pair<double* ,double* > > trainingSet)
{
  double error;
  int step = 0;
  time_t begin, now;
  time(&begin);
  long seconds;
  
  do{
    error = 0;
    if(!batch)
      std::random_shuffle ( trainingSet.begin(), trainingSet.end() );
    for(auto trainingPair:trainingSet){
      feedForward(trainingPair.first);
      error += backPropagate(trainingPair.first, trainingPair.second);
    }
    
    if(batch) {
      int inN = inputCount;
      for(int l = 0; l < layerCount; l++) {
        for(int j = 0; j < neuronCounts[l]; j++) {
          for(int i = 0; i < inN; i++) {
            weights[l][j][i] += weightDeltas[l][j][i]; 
            weightDeltas[l][j][i] = 0; 
          }
          weights[l][j][inN] += weightDeltas[l][j][inN]; 
          weightDeltas[l][j][inN] = 0; 
        }
        inN = neuronCounts[l];
      }
    }
    time(&now);
    seconds = difftime(now,begin);
    std::cout << error <<" "<<step<<" "<<seconds<< std::endl;
  }while(error > maxError && step++ < maxSteps  && seconds < maxTime);
  std::cout << error <<" "<<step<<" "<<seconds<< std::endl;
}

//! Uklada sit
/*!
Uklada sit do souboru
\param filename Cilovy soubor
\return Jesli se povedlo sit ulozit
*/
bool BPNet::saveFile(const char* filename)
{
  std::ofstream out(filename, std::ios::binary);
  out << "BP" << std::endl << inputCount << std::endl << layerCount << std::endl;
    
  out.write(reinterpret_cast<char*>(neuronCounts),sizeof(int)*layerCount);
  
  int inN = inputCount + 1;
  for(int i = 0; i < layerCount; i++){
    for(int j = 0; j < neuronCounts[i];j++){
      out.write(reinterpret_cast<char*>(weights[i][j]),sizeof(double)*inN);
    }
    inN = neuronCounts[i] + 1;
  }
  out.close();
  return true;
}

//! Nacte sit
/*!
Nacte sit ze souboru
\param filename Soubor se siti
\return Jesli se povedlo nacist sit
*/
bool BPNet::loadFile(const char* filename)
{
  for(int i = 0; i < layerCount; i++) {
    for(int j = 0; j < neuronCounts[i];j++) {
      delete[] weights[i][j];
      delete[] weightDeltas[i][j];
    }
  }
  
  for(int i = 0; i < layerCount; i++) {
    delete[] neurons[i];
    delete[] weights[i];
    delete[] weightDeltas[i];
    delete[] errorGradients[i];
  }
  
  if(layerCount!=0){
    delete[] neurons;
    delete[] weights;
    delete[] weightDeltas;
    delete[] errorGradients;
    
    delete[] neuronCounts;
  }
  std::ifstream in(filename, std::ios::binary);
  if(!(in.get() == 'B' && in.get() == 'P'))  {
    std::cerr<<"Nepodarilo se nacist sit"<<std::endl;
    return false;
  }
  in >> inputCount;
  in >> layerCount;
  in.get();
  
  neuronCounts = new int [layerCount];
  in.read(reinterpret_cast<char*>(neuronCounts),sizeof(int)*layerCount);
  
  neurons = new double* [layerCount];
  weights = new double** [layerCount];
  weightDeltas = new double** [layerCount];
  errorGradients = new double* [layerCount];
  
  int inputs = inputCount + 1;
  for(int i = 0; i < layerCount; i++) {
    int nN = neuronCounts[i];
    neurons[i] = new double [nN];
    weights[i] = new double* [nN];
    weightDeltas[i] = new double* [nN];
    errorGradients[i] = new double [nN];
    
    for(int j = 0; j < nN; j++) {
      weights[i][j] = new double [inputs];
      in.read(reinterpret_cast<char*>(weights[i][j]),sizeof(double)*inputs);
  
      weightDeltas[i][j] = new double [inputs];
      for(int k = 0; k < inputs; k++) {
        weightDeltas[i][j][k] = 0;
      }
    }
    inputs = nN + 1;
  }
  
  in.close();
  outputCount = neuronCounts[OUTPUT];
  return true;
}
