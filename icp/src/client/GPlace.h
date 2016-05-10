/*!
\file GPlace.h
Hlavickovy soubor s tridou GPlace popisujici grafickou podobu mista.
\author Pavel Fryz
\author Vojtech Bartl
\date 7. 5. 2012
*/
#ifndef GPLACE_H
#define GPLACE_H

#include <QtGui>
#include "dialogs.h"
#include "drawFrame.h"

class drawFrame;

//! Trida reprezentujici misto
/*!
    Rozsiruje tridu QGraphicsEllipseItem. Uchovava si potrebne informace o miste.
  */
class GPlace : public QGraphicsEllipseItem {

public:
    //! Konstrutor mista
    /*!
      \param x X-ova souradnice mista
      \param y Y-ova souradnice mista
      \param frm Scena do ktere se vklada
      */
    GPlace(int x, int y, drawFrame *frm);
    ~GPlace();                  /*!< Destruktor*/
    int RADIUS;                 /*!< Polomer kruhu*/
    QPen lineColor;             /*!< Barva kruznice*/
    QBrush fillColor;           /*!< Barva vyplne*/
    QPen selectColor;           /*!< Barva kruznice pokud je na miste mys*/
    //! Zjisteni stredoveho bodu
    /*!
      \return Bod ve stredu mista
      */
    QPointF getCenterPoint();
    myPlaceDialog *dialog;      /*!< Dialog pro vlozeni tokenu*/
    drawFrame *frame;           /*!< Scena do ktere se vklada*/
    //! Nastaveni novych hodnot pri nacteni stylu
    /*!
      \param lineC Nova barva kruznice
      \param fillC Nova barva vyplne
      \param selectC Nova barava kruznice pri najeti mysi
      \param r Novy polomer kruznice
      */
    void setStyle(QColor lineC, QColor fillC, QColor selectC, int r);

public slots:
    void hoverEnterEvent(QGraphicsSceneHoverEvent *event);
    void hoverLeaveEvent(QGraphicsSceneHoverEvent *event);

};

#endif // GPLACE_H
