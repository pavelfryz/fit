/*!
\file arc.h
Hlavickovy soubor s tridou Arc, ktera slouzi pro reprezentaci hrany.
\author Pavel Fryz
\author Vojtech Bartl
\date 7. 5. 2012
*/

#ifndef ARC_H
#define ARC_H
#include <QString>

class Place;
class Transition;
class Net;

#include <QDomDocument>
#include <QDomNode>

//! Trida reprezentujici hranu
/*!
Hrana spojuje misto a prechod, hrana je orientovana.
Umoznuje docasne ulozit token prochazejici pres misto.
*/
class Arc
{
public:
    //! Vytvori novou hranu
    /*!
    Vytvori novou hranu spojujici misto a prechod,
    hrana je orientovana k prechodu.
    \param place Spojovane misto
    \param transition Spojovany prechod
    */
    Arc(Place *place,Transition *transition);

    //! Vytvori novou hranu
    /*!
    Vytvori novou hranu spojujici prechod a misto,
    hrana je orientovana k mistu.
    \param transition Spojovany prechod
    \param place Spojovane mistu
    */
    Arc(Transition *transition,Place *place);

    //! Vrati jesli je hrana konstatni
    /*!
    Vrati jestli je nazev dane hrany cislo
    \return true Pokud je nazev cislo, jinak false.
    */
    bool isConst();

    //! Orientace hrany.
    /*! Vycet moznych orientaci hrany. */
    enum Direction{
        toTransition, /*!< Orientace od mista k prechodu*/
        toPlace       /*!< Orientace od prechodu k mistu*/
    };
    friend class Place;
    friend class Transition;
    friend class Net;

    //! Prida hranu do xml dokumentu
    /*!
    Prevede hranu na xml element a prida ji do dokumentu na zvolene misto.
    \param document Dokument, do ktereho se hrana pridava
    \param node Uzel dokumentu, do ktereho se hrana pridava
    */
    void toXML(QDomDocument &document, QDomNode &node);
private:
    Place *place;           /*!< Spojovane misto*/
    Transition *transition; /*!< Spojovany prechod*/
    Direction direction;    /*!< Orientace hrany*/
    QString label;          /*!< Nazev hrany*/
    int buffer;             /*!< Buffer s hodnotou tokenu, prochazejiciho pres hranu*/
};

#endif // ARC_H
