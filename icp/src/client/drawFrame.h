/*!
\file drawFrame.h
Hlavickovy soubor s tridou drawFrame, ktera je hlavni scenou pro vkladani
a editaci site. Zobrazuje grafickou podobu site.
\author Pavel Fryz
\author Vojtech Bartl
\date 7. 5. 2012
*/
#ifndef DRAWFRAME_H
#define DRAWFRAME_H

#include <QtGui>
#include <vector>
#include "GPlace.h"
#include "GTransition.h"
#include "GArc.h"
#include "communication.h"
#include "mainwindow.h"

class GPlace;
class GTransition;
class GArc;
class MainWindow;

/*!
    Hlavni scena pro vykresleni site
  */
class drawFrame : public QGraphicsScene {
    Q_OBJECT

public:
    drawFrame();                /*!< Konstruktor*/
    ~drawFrame();               /*!< Destruktor*/
    //! Nastaveni modu
    /*!
      \param mode Mod, ktery se ma nastavit
      */
    void setMode(int mode);
    void clearAll();            /*!< Vymaze celou sit */
    //! Vytvoreni XML ze site do souboru
    /*!
      \param fileName Jmeno souboru do ktereho se bude ukladat
      \return Zda se vse zdarilo
      */
    int createXML(QString fileName);
    //! Vytvori XML ze site pro poslani na server
    /*!
      \param idOrSteps Id prechodu, ktery se ma provest, nebo maximalni pocet kroku
      \param name Jmeno site
      \param desc Popis site
      \return XML jako retezec
      */
    QString createXMLrun(int idOrSteps, QString name, QString desc);
    //! Nacteni site ze souboru nebo z retezce (XML)
    /*!
      \param fileName Nazev souboru nebo retezec s XML
      \return Zda se vse zdarilo
      */
    int readXML(QString fileName);
    //! Vycet moznych modu
    enum modes{
        PLACE=1,        /*!< Vkladani mista */
        TRANSITION,     /*!< Vkladani prechodu */
        MOVE,           /*!< Pohyb s prvky */
        REMOVE,         /*!< Odstraneni prvku */
        SIMULATION      /*!< Mod simulace */
        };
    //! Nacteni stylu ze souboru
    /*!
      \param name Jmeno stylu ktery se ma nacist
      */
    void readStyles(std::string name);
    void simulationEnd();           /*!< Prekresleni prechody puvodnimi barvami po skoneceni modu simulace*/
    std::string styleName;          /*!< Jmeno aktualniho stylu */
    int stylePlaceRadius;           /*!< Polomer mista v aktualnim stylu */
    QColor stylePlaceLineColor;     /*!< Barva mista v aktualnim stylu */
    QColor stylePlaceFillColor;     /*!< Barva vyplne mista v aktualnim stylu */
    QColor stylePlaceSelectColor;   /*!< Barva mista pri vyberu v aktualnim stylu */
    int styleTransWidth;            /*!< Sirka prechodu v aktualnim stylu */
    int styleTransHeight;           /*!< Vyska prechodu v aktualnim stylu */
    QColor styleTransLineColor;     /*!< Barva prechodu v aktualnim stylu */
    QColor styleTransFillColor;     /*!< Barva vyplne prechodu v aktualnim stylu */
    QColor styleTransSelectColor;   /*!< Barva prechodu pri vyberu v aktualnim stylu */
    QColor styleArcLineColor;       /*!< Barva hrany v aktualnim stylu */
    QColor styleArcSelectColor;     /*!< Barva hrany pri vyberu v aktualnim stylu */
    communication *server;          /*!< Konumikace se serverem */
    MainWindow *window;             /*!< Hlavni okno programu*/

protected:
    int myMode;         /*!< Aktualni mod */
    bool drawArc;       /*!< Zda se bude vykreslovat hrana */
    QGraphicsItem *fromItem;        /*!< Prvek ze ktereho se tvori hrana */
    std::vector<GPlace *> myPlaces;     /*!< Vsechna nactena mista site */
    std::vector<GTransition *> myTransitions;   /*!< Vsechny nactene prechody site */
    std::vector<GArc *> myArcs;     /*!< vsechny nactene hrany site */
    //! Zjisteni zda je hrana na zadanem bode
    /*!
      \param point Bod ve scene
      \return Bod je na nejake hrane site
      */
    bool isArcOnPoint(QPointF point);
    //! Zjisteni zda je misto na zadanem bode
    /*!
      \param point Bod ve scene
      \return Bod je na nejakem miste site
      */
    bool isPlaceOnPoint(QPointF point);
    //! Zjisteni zda je prechod na zadanem bode
    /*!
      \param point Bod ve scene
      \return Bod je na nejakem prechodu site
      */
    bool isTransOnPoint(QPointF point);
    //! Zjisteni ktera hrana je na zadanem bode
    /*!
      \param point Bod ve scene
      \return Poradi hrany v seznamu hran site, na ktere je aktualni bod
      */
    unsigned arcOnPoint(QPointF point);
    //! Zjisteni ktere misto je na zadanem bode
    /*!
      \param point Bod ve scene
      \return Poradi mista v seznamu mist site, na ktere je aktualni bod
      */
    unsigned placeOnPoint(QPointF point);
    //! Zjisteni ktery prechod je na zadanem bode
    /*!
      \param point Bod ve scene
      \return Poradi prechodu v seznamu prechodu site, na ktere je aktualni bod
      */
    unsigned transOnPoint(QPointF point);
    //! Odstraneni prveku na zadane pozici
    /*!
      \param point Bod ve scene
      */
    void removeItemOnPoint(QPointF point);
    void repaintFrame();            /*!< Prekresleni sceny po nacteni noveho stylu */
    void recomputeArcs();           /*!< Prepocitani souradnic hran pri nacteni noveho stylu */
    void recomputeEndPoints();      /*!< Prepocitani koncovych bodu hran */
    void repaintSimulation();       /*!< Prekresleni neproveditelnych prechodu cervenou barvou pri simulaci */
    //! Zjisteni koncoveho bodu hrany na okraji mista, nebo prechodu
    /*!
      \param point Bod ve scene
      \param arcPos Poradi hrany v seznamu hran site, pro kterou hledam koncovy bod
      \return Koncovy bod hrany
      */
    QPointF computeEndPoint(QPointF point, int arcPos);


public slots:
    void mousePressEvent(QGraphicsSceneMouseEvent *event);
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *event);
    void mouseMoveEvent(QGraphicsSceneMouseEvent *event);

};

#endif // DRAWFRAME_H
