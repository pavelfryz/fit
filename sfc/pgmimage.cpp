/*!
\file pgmimage.cpp
Soubor s implementaci tridy pgmImage, ktera slouzi pro praci s obrazky typu pgm. 
\author Pavel Frýz
\date 26. 11. 2014
*/

#include <algorithm>
#include <cstdio>
#include <fstream>  
#include <iostream>  
#include <limits>  
#include <string> 

#include "pgmimage.h"


//! Vytvori prazdny obrazek
/*!
Vytvori prazdny obrazek, pro reprezentaci obrazku typu pgm.
\param index Celociselny index polohy
*/
pgmImage::pgmImage():width(0),height(0),pixelMax(255),raster()
{
}

//! Smaze obrazek
/*!
Smaze obrazek a uvolni pamet, kterou zabiral.
*/
pgmImage::~pgmImage()
{
  if(raster) delete[] raster;
}

//! Preskakuje komenty
/*!
Pomocna funkce, ktera ve vstupnim souboru zpracovava komentare, ktere preskakuje.
\param in Vstupni soubor
\param skipWS Prepinac, ktery urcuje jestli muze byt pred zacatkem komentu whitespace
*/
inline void skipComment(std::istream &in, double skipWS = true)
{
  int c = (skipWS)?(in >> std::ws).peek():in.peek();
  while(c == '#' && c != EOF){
    in.ignore(std::numeric_limits<std::streamsize>::max(),'\n');
    c = (skipWS)?(in >> std::ws).peek():in.peek();
  }
}

//! Nacte obrazek
/*!
Nacte obrazek ze souboru
\param filename Soubor s obrazkem
\return Jesli se povedlo nacist obrazek
*/
bool pgmImage::loadFile(const char* filename)
{
  std::ifstream in(filename, std::ios::binary);
  
  skipComment(in);                        
  if(!(in.get() == 'P' && in.get() == '5'))  {
    std::cerr<<"Nepodarilo se nacist obrazek"<<std::endl;
    return false;
  }
  skipComment(in);
  in >> width;
  skipComment(in);
  in >> height; 
  skipComment(in);
  in >> pixelMax;
  skipComment(in,false);
  in.get();
  
  if(raster==nullptr)
    raster = new pixel[width*height];
  in.read(reinterpret_cast<char*>(raster),sizeof(pixel)*width*height);
  
  in.close();
  return true;
}

//! Uklada obrazek
/*!
Uklada obrazek do souboru
\param filename Cilovy soubor
\return Jesli se povedlo obrazek ulozit
*/
bool pgmImage::saveFile(const char* filename)
{
  std::ofstream out(filename, std::ios::binary);
  out << "P5" << std::endl << width << std::endl << height << std::endl <<
    pixelMax << std::endl;
    
  out.write(reinterpret_cast<char*>(raster),sizeof(pixel)*width*height);
  out.close();
  return true;
}

//! Zakoduje obrazek
/*!
Prevede celociselne hodnoty pixelu na rozsah od 0 do 1
\param image Vraceny zakodovany obrazek
\return Uspesnost operace
*/
bool pgmImage::encode(double*& image)
{
  if(image==nullptr){
    image = new double[width*height];
  }
  
  double c=1.0/(pixelMax+1);
  for(int i=0;i<width*height;i++){
    image[i] = (raster[i]+0.5)*c;
  }
  return true;
}

//! Dekoduje obrazek
/*!
Prevede zakodovany obrazek s rozsahem hodnot pixelu od 0 do 1 na celociselne hodnoty pixelu
\param image Zakodovany obrazek
\return Uspesnost operace
*/
bool pgmImage::decode(const double* image)
{
  int c=pixelMax+1;
  for(int i=0;i<width*height;i++){
    raster[i] = std::min(static_cast<int>(image[i]*c),pixelMax);
  }
  return true;
}

//! Vraci pocet pixelu
/*!
\return Pocet pixelu
*/
int pgmImage::pixelCount()
{
  return height*width;
}


