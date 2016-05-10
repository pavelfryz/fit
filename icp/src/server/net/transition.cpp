/*!
\file transition.cpp
Soubor s implementaci tridy Transition, ktera slouzi pro reprezentaci prechodu.
\author Pavel Fryz
\author Vojtech Bartl
\date 7. 5. 2012
*/

#include "transition.h"
#include "arc.h"
#include "place.h"

#include <QStringList>

//! Vytvori novy prechod
/*!
Vytvori novy prechod z xml souboru.
\param root Korenovsky element prechodu
*/
Transition::Transition(QDomElement root):
    executable(false)
{
    x=root.firstChildElement("x").text().toInt();
    y=root.firstChildElement("y").text().toInt();
    id=root.firstChildElement("id").text().toInt();
    guard=root.firstChildElement("guard").text();
    expression=root.firstChildElement("expr").text();
}

//! Spoji prechod s mistem
/*!
Spoji tento prechod se zadanym mistem.
\param place Spojovane misto
\return Hranu, ktera spojuje tento prechod a misto
*/
Arc *Transition::connectTo(Place *place)
{
    Arc *arc = new Arc(this,place);
    place->addInput(arc);
    outArcs.push_back(arc);
    return arc;
}

//! Prida vstupni hranu
/*!
Prida vstupni hranu do tohoto prechodu.
\param arc Vstupni hrana
*/
void Transition::addInput(Arc *arc)
{
    inArcs.push_back(arc);
}

//! Provede simulaci tohoto prechodu
/*!
Provede navazani tokenu ze vstupnich mist,
otestuje strazni podminku a provede prechod.
*/
void Transition::step()
{
    if(!testGuard(0)) return;
    for(unsigned int i=0;i<outArcs.size();i++){
        if(outArcs[i]->isConst()){
            outArcs[i]->place->tokens.push_back(outArcs[i]->label.toInt());
            continue;
        }
        if(outArcs[i]->label==expression.section("=",0,0).trimmed()){
            outArcs[i]->place->tokens.push_back(evalExpression());
            continue;
        }
        for(unsigned int j=0;j<inArcs.size();j++){
            if(inArcs[j]->label==outArcs[i]->label){
                outArcs[i]->place->tokens.push_back(inArcs[j]->buffer);
                break;
            }
        }
    }
}

//! Otestuje proveditelnost prechodu
/*!
Zjisti jestli jsou tokeny ve vsech vstupnich
mistech a otestuje strazni podminku.
\return true Pokud je prechod mozny, jinak false.
*/
bool Transition::testGuard()
{
    bool result = testGuard(0);
    if(result){
        for(unsigned int i=0;i<inArcs.size();i++){
            inArcs[i]->place->tokens.push_back(inArcs[i]->buffer);
        }
    }
    executable = result;
    return result;
}

//! Otestuje proveditelnost prechodu
/*!
Pomoci backtrackingu otestuje proveditelnost
daneho prechod, postupne vyzkousi vsechny mozny
kombinace vstupu. Testovani probiha rekurzivne.
\param limit Zarazka rekurze
\return true Pokud je prechod mozny, jinak false.
*/
bool Transition::testGuard(unsigned int limit)
{
    if(limit==inArcs.size())
        return test();
    Arc *arc=inArcs.at(limit);
    for(unsigned int i=0;i<arc->place->tokens.size();i++){
        arc->buffer=arc->place->tokens.front();
        arc->place->tokens.pop_front();
        if(arc->isConst()&&arc->label.toInt()!=arc->buffer){
            arc->place->tokens.push_back(arc->buffer);
            continue;
        }
        if(testGuard(limit+1)){
            return true;
        }
        arc->place->tokens.push_back(arc->buffer);
    }
    return false;
}

//! Vrati hodnotu vyrazu
/*!
Vypocita vyraz prechodu a vrati jeho hodnotu.
\return Hodnota vyrazu.
*/
int Transition::evalExpression()
{
    enum Operation{
        NoOperation,
        Minus,
        Plus
    };

    Operation state = NoOperation;
    int result=0;
    QStringList list = expression.replace(QRegExp("\\s"),"").split(QRegExp("\\b"),QString::SkipEmptyParts);
    for(int i=2;i<list.size();i++){
        switch(state){
        case NoOperation:
            if(list.at(i)=="-"){
                state=Minus;
            }else if(list.at(i)=="+"){
                state=Plus;
            }else{
                result=valueOf(list.at(i));
            }
            break;
        case Plus:
            result+=valueOf(list.at(i));
            state=NoOperation;
            break;
        case Minus:
            result-=valueOf(list.at(i));
            state=NoOperation;
            break;
        }
    }
    return result;
}

//! Vrati hodnotu operandu
/*!
Pokud je operand cislo, vrati jeho hodnotu.
Pokud je retezec, vrati hodnotu vyrazu nebo
vstupni hrany s danym jmenem.
\param operand Vyhodnocovany operand
\return Hodnotu operandu.
*/
int Transition::valueOf(QString operand)
{
     QRegExp rx("\\d+");
     if(rx.exactMatch(operand)){
         return operand.toInt();
     }
     for(unsigned int i=0;i<inArcs.size();i++){
         if(inArcs[i]->label==operand){
             return inArcs[i]->buffer;
         }
     }
     return -1;
}

//! Otestuje straznu podminku
/*!
Otestuje jestli je strazni podminka splnena,
s navazanymi tokeny.
\return true Pokud je podminka splnena, jinak false.
*/
bool Transition::test()
{
    QStringList list = guard.replace(QRegExp("\\s"),"").split(QRegExp("\\b"),QString::SkipEmptyParts);
    enum Operation{
        WantFirstOperand,
        HaveFirstOperand,
        IsEqual,
        IsGreater,
        IsLess,
        IsNotEqual,
        IsEqualOrGreater,
        IsEqualOrLess,
        WantSeparator
    };

    int firstOperand=0, secondOperand=0;
    Operation state=WantFirstOperand;

    for(int i=0;i<list.size();i++){
        switch(state){
        case WantFirstOperand:
            firstOperand=valueOf(list.at(i));
            state=HaveFirstOperand;
            break;
        case HaveFirstOperand:
            if(list.at(i)=="=="){
                state=IsEqual;
            }else if(list.at(i)==">"){
                state=IsGreater;
            }else if(list.at(i)=="<"){
                state=IsLess;
            }else if(list.at(i)=="!="){
                state=IsNotEqual;
            }else if(list.at(i)==">="){
                state=IsEqualOrGreater;
            }else if(list.at(i)=="<="){
                state=IsEqualOrLess;
            }
            break;
        case WantSeparator:
            state=WantFirstOperand;
            break;
        default:
            secondOperand=valueOf(list.at(i));
            if((state==IsEqual && firstOperand!=secondOperand)||
                    (state==IsGreater && firstOperand<=secondOperand)||
                    (state==IsLess && firstOperand>=secondOperand)||
                    (state==IsNotEqual && firstOperand==secondOperand)||
                    (state==IsEqualOrGreater && firstOperand<secondOperand)||
                    (state==IsEqualOrLess && firstOperand>secondOperand)){
                return false;
            }
            break;
        }
    }
    return true;
}

//! Prida prechod do xml dokumentu
/*!
Prevede prechod na xml element a prida ho do dokumentu na zvolene misto.
\param document Dokument, do ktereho se prechod pridava
\param node Uzel dokumentu, do ktereho se prechod pridava
*/
void Transition::toXML(QDomDocument &document,QDomNode &node)
{
    QDomElement root=document.createElement("Transition");
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
    QDomElement guardElem=document.createElement("guard");
    root.appendChild(guardElem);
    guardElem.appendChild(document.createTextNode(guard));
    QDomElement exprElem=document.createElement("expr");
    root.appendChild(exprElem);
    exprElem.appendChild(document.createTextNode(expression));

    QDomElement executableElem=document.createElement("executable");
    root.appendChild(executableElem);
    if(executable){
        executableElem.appendChild(document.createTextNode("true"));
    }else{
        executableElem.appendChild(document.createTextNode("false"));
    }
}

//! Zkontroluje prechod
/*!
Zkontroluje jesli dana prechod ma definovany
vsechny promeny ve vyrazech, vstupy a vystupy.
\return true Pokud je dany prechod validni, jinak false.
*/
bool Transition::isValid()
{
    for(unsigned int i=0;i<outArcs.size();i++){
        if(outArcs.at(i)->label=="") return false;
        if(outArcs.at(i)->isConst()){
            continue;
        }else if(outArcs.at(i)->label==expression.section("=",0,0).trimmed()){
            continue;
        }else if(isInput(outArcs.at(i)->label)){
            continue;
        }
        return false;
    }
    return true;
}

//! Zjisti jesli existuje vstupni hrana
/*!
Zjisti jestli existuje vstupni hrana s danym nazvem.
\param label Nazev hrany
\return true Pokud hrana existuje, jinak false.
*/
bool Transition::isInput(QString label)
{
    for(unsigned int i=0;i<inArcs.size();i++){
        if(inArcs.at(i)->label==label){
            return true;
        }
    }
    return false;
}
