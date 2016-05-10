/*!
\file place.h
Hlavickovy soubor s tridou Place, ktera slouzi pro reprezentaci mista.
\author Pavel Fryz
\author Vojtech Bartl
\date 7. 5. 2012
*/

#ifndef PLACE_H
#define PLACE_H

#include <QString>
#include <QDomElement>
#include <vector>
#include <deque>

class Arc;
class Transition;
class Net;

#include <QDomElement>
#include <QDomNode>

//! Trida reprezentujici misto
/*!
Misto obsahuje seznam tokenu. A je pomoci hrany spojeno
s prechody. Kazde misto je identifikovano jedinecnym id.
Poloha mista je urcena souradnicemi x a y.
*/
class Place
{
public:
    //! Vytvori nove misto
    /*!
    Vytvori nove misto z xml souboru.
    \param root Korenovsky element mista
    */
    Place(QDomElement root);

    //! Spoji misto s prechodem
    /*!
    Spoji toto misto se zadanym prechodem.
    \param transition Spojovany prechod
    \return Hranu, ktera spojuje toto misto a prechod
    */
    Arc *connectTo(Transition *transition);

    //! Prida vstupni hranu
    /*!
    Prida vstupni hranu do tohoto mista.
    \param arc Vstupni hrana
    */
    void addInput(Arc *arc);

    //! Vrati retezec s tokeny
    /*!
    Vrati retezec reprezentujici tokeny daneho mista.
    Jednotlive tokeny jsou oddeleny carkou.
    \return Retezec s tokeny
    */
    QString getTokens();
    friend class Arc;
    friend class Transition;
    friend class Net;

    //! Prida misto do xml dokumentu
    /*!
    Prevede misto na xml element a prida ho do dokumentu na zvolene misto.
    \param document Dokument, do ktereho se misto pridava
    \param node Uzel dokumentu, do ktereho se misto pridava
    */
    void toXML(QDomDocument &document, QDomNode &node);
private:
    int x;  /*!< Pozice mista na ose x*/
    int y;  /*!< Pozice mista na ose y*/
    int id; /*!< Identifikator mist*/
    std::deque<int> tokens;     /*!< Seznam tokenu*/
    std::vector<Arc *> inArcs;  /*!< Seznam vstupnich hran*/
    std::vector<Arc *> outArcs; /*!< Seznam vystupnich hran*/

};

#endif // PLACE_H
