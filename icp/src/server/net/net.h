/*!
\file net.h
Hlavickovy soubor s tridou Net, ktera slouzi pro reprezentaci petriho site.
\author Pavel Fryz
\author Vojtech Bartl
\date 7. 5. 2012
*/

#ifndef NET_H
#define NET_H

#include <QString>
#include <QDomElement>
#include <QDomNode>
#include <vector>

class Arc;
class Place;
class Transition;

//! Trida reprezentujici sit
/*!
Sit obsahuje seznam mist, prechodu a hran. Dale obsahuje jmeno
a popis site. Sit je mozne nacist z xml souboru. Umoznuje
krokovat a simulovat sit. Odsimulovanou sit je pote mozne
ulozit do xml souboru.
*/
class Net
{
public:
    //! Vytvori novou sit
    /*!
    Vytvori novou sit z xml souboru.
    \param root Korenovsky element site
    */
    Net(QDomElement root);

    //! Odstrani sit
    /*!
    Smaze vsechny mista, prechodu a hrany dane site.
    A dealokuje pouzite zdroje.
    */
    ~Net();

    //! Provede krok simulace
    /*!
    Provede krok simulace daneho prechodu.
    \param transition Identifikator prechodu
    \return true Pokud se prechod provedl, jinak false.
    */
    bool step(int transition);

    //! Provede simulaci site
    /*!
    Provede simulaci site, dokud neni mozne udelat
    dalsi prechod, nebo bylo dosazeno maximalniho
    poctu kroku.
    \param steps Maximalni pocet kroku
    */
    void run(int steps);

    friend class Arc;
    friend class Place;
    friend class Transition;

    //! Prida sit do xml dokumentu
    /*!
    Prevede sit na xml element a prida ho do dokumentu na zvolene misto.
    \param document Dokument, do ktereho se pridava sit
    \param node Uzel dokumentu, do ktereho se pridava sit
    */
    void toXML(QDomDocument &document, QDomNode &node);

    //! Zkontroluje sit
    /*!
    Zkontroluje jesli dana sit neobsahuje nedefinovane hrany,
    nebo spatne vyrazy v prechodech.
    \return true Pokud je dana sit validni, jinak false.
    */
    bool isValid();
private:
    QString name;  /*!< Nazev site*/
    QString description;  /*!< Popis site*/
    std::vector<Arc *> arcs;  /*!< Seznam hran*/
    std::vector<Place *> places;  /*!< Seznam mist*/
    std::vector<Transition *> transitions;  /*!< Seznam prechodu*/

    //! Provede krok simulace
    /*!
    Provede krok simulace daneho prechodu.
    \param transition Simulovany prechod
    \return true Pokud se prechod provedl, jinak false.
    */
    bool step(Transition *transition);
};

#endif // NET_H
