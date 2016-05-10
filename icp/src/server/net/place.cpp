/*!
\file place.cpp
Soubor s implementaci tridy Place, ktera slouzi pro reprezentaci mista.
\author Pavel Fryz
\author Vojtech Bartl
\date 7. 5. 2012
*/

#include "place.h"
#include "arc.h"
#include "transition.h"

#include <QStringList>
#include <algorithm>

//! Vytvori nove misto
/*!
Vytvori nove misto z xml souboru.
\param root Korenovsky element mista
*/
Place::Place(QDomElement root)
{
    x=root.firstChildElement("x").text().toInt();
    y=root.firstChildElement("y").text().toInt();
    id=root.firstChildElement("id").text().toInt();
    QString tokenString=root.firstChildElement("tokens").text();
    QStringList tokenList = tokenString.split(",");
    for (int i = 0; i < tokenList.size(); ++i){
        QString token=tokenList.at(i).trimmed();
        if(token=="") continue;
        tokens.push_back(token.toInt());
    }
}

//! Spoji misto s prechodem
/*!
Spoji toto misto se zadanym prechodem.
\param transition Spojovany prechod
\return Hranu, ktera spojuje toto misto a prechod
*/
Arc *Place::connectTo(Transition *transition)
{
    Arc *arc = new Arc(this,transition);
    transition->addInput(arc);
    outArcs.push_back(arc);
    return arc;
}

//! Prida vstupni hranu
/*!
Prida vstupni hranu do tohoto mista.
\param arc Vstupni hrana
*/
void Place::addInput(Arc *arc)
{
    inArcs.push_back(arc);
}

//! Vrati retezec s tokeny
/*!
Vrati retezec reprezentujici tokeny daneho mista.
Jednotlive tokeny jsou oddeleny carkou.
\return Retezec s tokeny
*/
QString Place::getTokens()
{
    QString result;
    std::sort(tokens.begin(),tokens.end());
    for(unsigned int i=0;i<tokens.size();i++){
        result+=QString::number(tokens.at(i))+",";
    }
    result.truncate(result.length()-1);
    return result;
}

//! Prida misto do xml dokumentu
/*!
Prevede misto na xml element a prida ho do dokumentu na zvolene misto.
\param document Dokument, do ktereho se misto pridava
\param node Uzel dokumentu, do ktereho se misto pridava
*/
void Place::toXML(QDomDocument &document,QDomNode &node)
{
    QDomElement root=document.createElement("Place");
    node.appendChild(root);
    QDomElement xElem=document.createElement("x");
    root.appendChild(xElem);
    xElem.appendChild(document.createTextNode(QString::number(x)));
    QDomElement yElem=document.createElement("y");
    root.appendChild(yElem);
    yElem.appendChild(document.createTextNode(QString::number(y)));
    QDomElement idElem=document.createElement("id");
    root.appendChild(idElem);
    idElem.appendChild(document.createTextNode(QString::number(id)));
    QDomElement tokensElem=document.createElement("tokens");
    root.appendChild(tokensElem);
    tokensElem.appendChild(document.createTextNode(getTokens()));
}
