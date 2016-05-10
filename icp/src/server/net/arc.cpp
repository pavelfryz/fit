/*!
\file arc.cpp
Soubor s implementaci tridy Arc, ktera slouzi pro reprezentaci hrany.
\author Pavel Fryz
\author Vojtech Bartl
\date 7. 5. 2012
*/

#include "arc.h"
#include "place.h"
#include "transition.h"
#include <QtNetwork>

//! Vytvori novou hranu
/*!
Vytvori novou hranu spojujici misto a prechod,
hrana je orientovana k prechodu.
\param place Spojovane misto
\param transition Spojovany prechod
*/
Arc::Arc(Place *place, Transition *transition) :
    place(place),transition(transition)
{
    direction=toTransition;
}

//! Vytvori novou hranu
/*!
Vytvori novou hranu spojujici prechod a misto,
hrana je orientovana k mistu.
\param transition Spojovany prechod
\param place Spojovane mistu
*/
Arc::Arc(Transition *transition, Place *place) :
    place(place),transition(transition)
{
    direction=toPlace;
}

//! Vrati jesli je hrana konstatni
/*!
Vrati jestli je nazev dane hrany cislo
\return true Pokud je nazev cislo, jinak false.
*/
bool Arc::isConst(){
    QRegExp rx("\\d+");
    return rx.exactMatch(label);
}

//! Prida hranu do xml dokumentu
/*!
Prevede hranu na xml element a prida ji do dokumentu na zvolene misto.
\param document Dokument, do ktereho se hrana pridava
\param node Uzel dokumentu, do ktereho se hrana pridava
*/
void Arc::toXML(QDomDocument &document,QDomNode &node)
{
    QDomElement root=document.createElement("Arc");
    node.appendChild(root);
    QDomElement placeElem=document.createElement("place_id");
    root.appendChild(placeElem);
    placeElem.appendChild(document.createTextNode(QString::number(place->id)));
    QDomElement transitionElem=document.createElement("transition_id");
    root.appendChild(transitionElem);
    transitionElem.appendChild(document.createTextNode(QString::number(transition->id)));
    QDomElement labelElem=document.createElement("constOrVar");
    root.appendChild(labelElem);
    labelElem.appendChild(document.createTextNode(label));

    QDomElement orientationElem=document.createElement("orientation");
    root.appendChild(orientationElem);
    if(direction==toPlace){
        orientationElem.appendChild(document.createTextNode("toPlace"));
    }else{
        orientationElem.appendChild(document.createTextNode("toTransition"));
    }
}
