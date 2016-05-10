/*!
\file GArc.h
Hlavickovy soubor s tridou GArc popisujici grafickou podobu hrany.
\author Pavel Fryz
\author Vojtech Bartl
\date 7. 5. 2012
*/
#ifndef GARC_H
#define GARC_H

#include <QtGui>
#include "dialogs.h"
#include "drawFrame.h"


class drawFrame;

//! Trida reprezentujici hranu
/*!
    Trida v sobe obsahuje objekt tridy QGraphicsLineItem, ktery reprezentuje
    hranu v grafice. Uchovava informace o hranou spojenych prvcich.
  */
class GArc : public QObject {
    Q_OBJECT

public:
    //! Konstruktor
    /*!
      \param frm Scena do ktere se vklada
      */
    GArc(drawFrame *frm);
    ~GArc();                    /*!< Destruktor*/
    QGraphicsLineItem *line;    /*!< Primka reprezentujici hranu*/
    QPen lineColor;             /*!< Barva hrany*/
    QPen selectColor;           /*!< Barva hrany pri najeti mysi*/
    QGraphicsItem *from;        /*!< Prvek ze ktereho hrana smeruje*/
    QGraphicsItem *to;          /*!< Prvek do ktereho hrana smeruje*/
    myArcDialog *dialog;        /*!< Dialog pro pridani labelu na hranu*/
    drawFrame *frame;           /*!< Scena pro vykreslovani*/
    QGraphicsEllipseItem *endPoint; /*!< Oznaceni konce hrany */
    //! Nastaveni novych hodnot pri nacteni stylu
    /*!
      \param lineC Barva hrany
      \param selectC Barva hrany pri najeti mysi
      */
    void setStyle(QColor lineC, QColor selectC);
    

};

#endif // GARC_H
