/*!
\file net.cpp
Soubor s implementaci tridy Net, ktera slouzi pro reprezentaci petriho site.
\author Pavel Fryz
\author Vojtech Bartl
\date 7. 5. 2012
*/

#include "net.h"
#include "place.h"
#include "transition.h"
#include "arc.h"
#include <algorithm>

//! Vytvori novou sit
/*!
Vytvori novou sit z xml souboru.
\param root Korenovsky element site
*/
Net::Net(QDomElement root)
{
    QDomNodeList placeList=root.elementsByTagName("Place");
    QDomNodeList transList=root.elementsByTagName("Transition");
    QDomNodeList arcList=root.elementsByTagName("Arc");
    name=root.firstChildElement("netName").text();
    description=root.firstChildElement("description").text();
    // vytvori mista
    for(int i=0; i<placeList.count(); i++){
        Place *place = new Place(placeList.at(i).toElement());
        places.push_back(place);
    }
    // vytvori prechody
    for(int i=0; i<transList.count(); i++){
        Transition *transition = new Transition(transList.at(i).toElement());
        transitions.push_back(transition);
    }
    //vytvori hrany
    for(int i=0; i<arcList.count(); i++){
        Arc *arc;
        QDomElement arcElem = arcList.at(i).toElement();
        int place=arcElem.firstChildElement("place_id").text().toInt();
        int transition=arcElem.firstChildElement("transition_id").text().toInt();
        if(arcElem.firstChildElement("orientation").text()=="toPlace"){
            arc=transitions[transition]->connectTo(places[place]);
        }else{
            arc=places[place]->connectTo(transitions[transition]);
        }
        arc->label=arcElem.firstChildElement("constOrVar").text().trimmed();
        arcs.push_back(arc);
    }
}

//! Odstrani sit
/*!
Smaze vsechny mista, prechodu a hrany dane site.
A dealokuje pouzite zdroje.
*/
Net::~Net()
{
    for(unsigned int i=0; i<places.size(); i++){
        delete(places.at(i));
    }
    for(unsigned int i=0; i<transitions.size(); i++){
        delete(transitions.at(i));
    }
    for(unsigned int i=0; i<arcs.size(); i++){
        delete(arcs.at(i));
    }
}

//! Provede krok simulace
/*!
Provede krok simulace daneho prechodu.
\param transition Simulovany prechod
\return true Pokud se prechod provedl, jinak false.
*/
bool Net::step(Transition *transition)
{
    if(transition->executable){
        transition->step();
        for(unsigned int i=0;i<transitions.size();i++){
            transitions.at(i)->testGuard();
        }
        return true;
    }
    return false;
}

//! Provede krok simulace
/*!
Provede krok simulace daneho prechodu.
\param transition Identifikator prechodu
\return true Pokud se prechod provedl, jinak false.
*/
bool Net::step(int transition)
{
    if(transitions.empty()) return false;
    transitions.at(transition)->testGuard();
    return step(transitions.at(transition));
}

//! Provede simulaci site
/*!
Provede simulaci site, dokud neni mozne udelat
dalsi prechod, nebo bylo dosazeno maximalniho
poctu kroku.
\param steps Maximalni pocet kroku
*/
void Net::run(int steps)
{
    for(unsigned int i=0;i<transitions.size();i++){
        transitions.at(i)->testGuard();
    }
    bool executed=true;
    while(executed && steps>0){
        executed=false;
        for(unsigned int i=0;i<transitions.size();i++){
            if(step(transitions.at(i))){
                executed=true;
                steps--;
            }
            if(steps==0) break;
        }

    }
}

//! Prida sit do xml dokumentu
/*!
Prevede sit na xml element a prida ho do dokumentu na zvolene misto.
\param document Dokument, do ktereho se pridava sit
\param node Uzel dokumentu, do ktereho se pridava sit
*/
void Net::toXML(QDomDocument &document, QDomNode &node){
    QDomElement root=document.createElement("Petri-NET");
    node.appendChild(root);
    for(unsigned int i=0;i<places.size();i++){
        places.at(i)->toXML(document,root);
    }
    for(unsigned int i=0;i<transitions.size();i++){
        transitions.at(i)->toXML(document,root);
    }
    for(unsigned int i=0;i<arcs.size();i++){
        arcs.at(i)->toXML(document,root);
    }
    QDomElement descElem=document.createElement("description");
    root.appendChild(descElem);
    descElem.appendChild(document.createTextNode(description));
    QDomElement nameElem=document.createElement("netName");
    root.appendChild(nameElem);
    nameElem.appendChild(document.createTextNode(name));
}

//! Zkontroluje sit
/*!
Zkontroluje jesli dana sit neobsahuje nedefinovane hrany,
nebo spatne vyrazy v prechodech.
\return true Pokud je dana sit validni, jinak false.
*/
bool Net::isValid()
{
    for(unsigned int i=0;i<transitions.size();i++){
        if(!transitions.at(i)->isValid())
            return false;
    }
    return true;
}
