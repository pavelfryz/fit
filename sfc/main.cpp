#include <iostream>     
#include <string>       
#include <climits> 
#include <cstdlib>     

#include "pgmimage.h"
#include "bpnet.h"

const char *CARD_NAMES[] =
{
  "eso  ",
  "dva  ",
  "tri  ",
  "ctyri",
  "pet  ",
  "sest ",
  "sedm ",
  "osm  ",
  "devet",
  "deset",
  "kluk ",
  "dama ",
  "kral "
};

int main(int argc, char ** argv)
{
  srand (time(NULL));
  
  const int CARDS = 13;
  const int LENGTH = 5;
  const int CARD_SIZE = 13*28;
  
  if(argc==2){ //asociace karty
    BPNet net(0,0,nullptr);
    if(!net.loadFile("net.bin")) return 1;
    
    pgmImage im;
    if(!im.loadFile(argv[1])) return 1;
    double *in = nullptr;
    im.encode(in);
    double *out = net.getOutput(in);
    char cnames[LENGTH+1];
    for(int j=0;j<LENGTH;j++){
      char c1 = std::min(static_cast<int>(out[j*3]*8),7);
      char c2 = std::min(static_cast<int>(out[j*3+1]*8),7);
      char c3 = std::min(static_cast<int>(out[j*3+2]*8),7);
      cnames[j]= c1|(c2<<3)|(c3<<6);
    }
    cnames[LENGTH] = '\0';
    std::cout << cnames << std::endl;
    delete[] in;
  }
  else{  // uceni site
    double names[CARDS][LENGTH*3];
    double c=1.0/8;
    for(int i=0;i<CARDS;i++){
      for(int j=0;j<LENGTH;j++){
        char c1 = CARD_NAMES[i][j];
        char c2 = CARD_NAMES[i][j];
        char c3 = CARD_NAMES[i][j];
        names[i][j*3] = ((c1&7)+0.5)*c;
        names[i][j*3+1] = (((c2>>3)&7)+0.5)*c;
        names[i][j*3+2] = (((c3>>6)&7)+0.5)*c;
      }
    }
    
    std::vector <std::pair<double* ,double* > > trainSet;
    pgmImage im;
    double *in[CARDS];
    for(int i=0;i<CARDS;i++){
      in[i]=nullptr;
      if(!im.loadFile(("deck1/"+std::to_string(i+1)+"h.pgm").c_str()))
        return 1;
      im.encode(in[i]);
      trainSet.push_back(std::pair<double* ,double* >(in[i],names[i]));
    }
    
    const int LAYER_COUNT = 2;
    int layers[LAYER_COUNT] = {20,LENGTH*3};
    BPNet net(CARD_SIZE,LAYER_COUNT,layers);
    net.learningRate = 0.85;
    net.momentum = 0.001;
    net.maxSteps = 250000;
    net.maxError = 0.001;  
    net.maxTime = 60*5; // 5 minut
    
    net.train(trainSet);
    net.saveFile("net.bin");
  }
  return 0;
}
