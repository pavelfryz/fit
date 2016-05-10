/*!
\file pgmimage.k
Hlavickovy soubor tridy pgmImage, ktera slouzi pro praci s obrazky typu pgm. 
\author Pavel Frýz
\date 26. 11. 2014
*/


#ifndef PGMIMAGE_H
#define PGMIMAGE_H
typedef unsigned char pixel;

class pgmImage
{
  public:
    int width;/*!< Sirka obrazku*/
    int height;/*!< Vyska obrazku*/
    int pixelMax;/*!< Rozsah hodnot pixelu*/
    pixel* raster;/*!< Data obrazku*/
    
    //! Vytvori prazdny obrazek
    /*!
    Vytvori prazdny obrazek, pro reprezentaci obrazku typu pgm.
    \param index Celociselny index polohy
    */
    pgmImage();
    
    //! Smaze obrazek
    /*!
    Smaze obrazek a uvolni pamet, kterou zabiral.
    */
    virtual ~pgmImage();
    
    //! Nacte obrazek
    /*!
    Nacte obrazek ze souboru
    \param filename Soubor s obrazkem
    \return Jesli se povedlo nacist obrazek
    */
    bool loadFile(const char* filename);
    
    //! Uklada obrazek
    /*!
    Uklada obrazek do souboru
    \param filename Cilovy soubor
    \return Jesli se povedlo obrazek ulozit
    */
    bool saveFile(const char* filename);
    
    //! Zakoduje obrazek
    /*!
    Prevede celociselne hodnoty pixelu na rozsah od 0 do 1
    \param image Vraceny zakodovany obrazek
    \return Uspesnost operace
    */
    bool encode(double *&image);
    
    //! Dekoduje obrazek
    /*!
    Prevede zakodovany obrazek s rozsahem hodnot pixelu od 0 do 1 na celociselne hodnoty pixelu
    \param image Zakodovany obrazek
    \return Uspesnost operace
    */
    bool decode(const double *image);
    
    //! Vraci pocet pixelu
    /*!
    \return Pocet pixelu
    */
    int pixelCount();    
};

#endif /* PGMIMAGE_H */ 
