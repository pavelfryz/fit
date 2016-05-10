/*!
\file transition.h
Hlavickovy soubor s tridou Transition, ktera slouzi pro reprezentaci prechodu.
\author Pavel Fryz
\author Vojtech Bartl
\date 7. 5. 2012
*/

#ifndef TRANSITION_H
#define TRANSITION_H

#include <QDomElement>
#include <vector>

class Arc;
class Place;
class Net;

//! Trida reprezentujici prechod
/*!
Prechod obsahuje straz a vyraz. Straz urcuje proveditelnost
prechodu. Vyraz se provede pokud je umoznen prechod.
Prechod je pomoci hrany spojen s misty. Kazdy prechod je
identifikovan jedinecnym id. Poloha prechodu je urcena
souradnicemi x a y.
*/
class Transition
{
public:
    //! Vytvori novy prechod
    /*!
    Vytvori novy prechod z xml souboru.
    \param root Korenovsky element prechodu
    */
    Transition(QDomElement root);

    //! Spoji prechod s mistem
    /*!
    Spoji tento prechod se zadanym mistem.
    \param place Spojovane misto
    \return Hranu, ktera spojuje tento prechod a misto
    */
    Arc *connectTo(Place *place);

    //! Prida vstupni hranu
    /*!
    Prida vstupni hranu do tohoto prechodu.
    \param arc Vstupni hrana
    */
    void addInput(Arc *arc);

    //! Provede simulaci tohoto prechodu
    /*!
    Provede navazani tokenu ze vstupnich mist,
    otestuje strazni podminku a provede prechod.
    */
    void step();

    //! Otestuje proveditelnost prechodu
    /*!
    Zjisti jestli jsou tokeny ve vsech vstupnich
    mistech a otestuje strazni podminku.
    \return true Pokud je prechod mozny, jinak false.
    */
    bool testGuard();
    friend class Arc;
    friend class Place;
    friend class Net;

    //! Prida prechod do xml dokumentu
    /*!
    Prevede prechod na xml element a prida ho do dokumentu na zvolene misto.
    \param document Dokument, do ktereho se prechod pridava
    \param node Uzel dokumentu, do ktereho se prechod pridava
    */
    void toXML(QDomDocument &document, QDomNode &node);

    //! Zjisti jesli existuje vstupni hrana
    /*!
    Zjisti jestli existuje vstupni hrana s danym nazvem.
    \param label Nazev hrany
    \return true Pokud hrana existuje, jinak false.
    */
    bool isInput(QString label);

    //! Zkontroluje prechod
    /*!
    Zkontroluje jesli dana prechod ma definovany
    vsechny promeny ve vyrazech, vstupy a vystupy.
    \return true Pokud je dany prechod validni, jinak false.
    */
    bool isValid();
private:
    int x;  /*!< Pozice prechodu na ose x*/
    int y;  /*!< Pozice prechodu na ose y*/
    int id; /*!< Identifikator mist*/
    bool executable;    /*!< Proveditelnost prechodu*/
    QString guard;      /*!< Straz prechodu*/
    QString expression; /*!< Vyraz prechodu*/
    std::vector<Arc *> inArcs;  /*!< Seznam vstupnich hran*/
    std::vector<Arc *> outArcs; /*!< Seznam vystupnich hran*/

    //! Otestuje proveditelnost prechodu
    /*!
    Pomoci backtrackingu otestuje proveditelnost
    daneho prechod, postupne vyzkousi vsechny mozny
    kombinace vstupu. Testovani probiha rekurzivne.
    \param limit Zarazka rekurze
    \return true Pokud je prechod mozny, jinak false.
    */
    bool testGuard(unsigned int limit);

    //! Otestuje straznu podminku
    /*!
    Otestuje jestli je strazni podminka splnena,
    s navazanymi tokeny.
    \return true Pokud je podminka splnena, jinak false.
    */
    bool test();

    //! Vrati hodnotu operandu
    /*!
    Pokud je operand cislo, vrati jeho hodnotu.
    Pokud je retezec, vrati hodnotu vyrazu nebo
    vstupni hrany s danym jmenem.
    \param operand Vyhodnocovany operand
    \return Hodnotu operandu.
    */
    int valueOf(QString operand);

    //! Vrati hodnotu vyrazu
    /*!
    Vypocita vyraz prechodu a vrati jeho hodnotu.
    \return Hodnota vyrazu.
    */
    int evalExpression();
};

#endif // TRANSITION_H
