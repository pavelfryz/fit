/*!
\file GTransition.h
Hlavickovy soubor s tridou GTransition popisujici grafickou podobu prechodu.
\author Pavel Fryz
\author Vojtech Bartl
\date 7. 5. 2012
*/
#ifndef GTRANSITION_H
#define GTRANSITION_H

#include <QtGui>
#include "dialogs.h"
#include "drawFrame.h"

class drawFrame;

//! Trida reprezentujici prechod
/*!
  Rozsiruje tridu QGraphicsRectItem. Uchovava si potrebne informace o prechodu.
  */
class GTransition : public QGraphicsRectItem {

public:
    //! Konstruktor prechodu
    /*!
      \param x X-ova souradnice prechodu
      \param y Y-ova souradnice prechodu
      \param frm Scena do ktere se vklada
      */
    GTransition(int x, int y, drawFrame *frm);
    ~GTransition();         /*!< Destruktor*/
    int WIDTH;              /*!< Sirka prechodu*/
    int HEIGHT;             /*!< Vyska vyska*/
    QPen lineColor;         /*!< Barva obvodove cary*/
    QBrush fillColor;       /*!< Barva vyplne*/
    QPen selectColor;       /*!< Barva obvodu pri najeti mysi*/
    //! Zjisteni stredoveho bodu
    /*!
      \return Bod ve stredu prechodu
      */
    QPointF getCenterPoint();
    myTransDialog *dialog;  /*!< Dialog pro vlozeni straze a vyrazu*/
    drawFrame *frame;       /*!< Scena do ktere se vklada*/
    bool executable;        /*!< Informace zda je prechod proveditelny (pri simulaci)*/
    //! Nastaveni novych hodnot pri nacteni stylu
    /*!
      \param lineC Nova barva obvodu prechodu
      \param fillC Nova barva vyplne
      \param selectC Nova barava obvodu pri najeti mysi
      \param w Nova sirka prechodu
      \param h Nova vyska prechodu
      */
    void setStyle(QColor lineC, QColor fillC, QColor selectC, int w, int h);

public slots:
    void hoverEnterEvent(QGraphicsSceneHoverEvent *event);
    void hoverLeaveEvent(QGraphicsSceneHoverEvent *event);

};

#endif // GTRANSITION_H
