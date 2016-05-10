/*!
\file GArc.cpp
Implementace tridy GArc. Graficke podoby hrany.
\author Pavel Fryz
\author Vojtech Bartl
\date 7. 5. 2012
*/
#include "GArc.h"
#include "drawFrame.h"
#include "mainwindow.h"

//! Konstruktor
/*!
  \param frm Scena do ktere se vklada
  */
GArc::GArc(drawFrame *frm){
    line=new QGraphicsLineItem();   //vytvori caru
    endPoint=new QGraphicsEllipseItem();
    lineColor.setColor(frm->styleArcLineColor);  //puvodni barva
    selectColor.setColor(frm->styleArcSelectColor);    //barva pri najeti mysi
    lineColor.setWidthF(2.5);       //sirky car
    selectColor.setWidthF(3.5);
    from=NULL;
    to=NULL;
    dialog=new myArcDialog();       //vytvoreni dialogu
    frame=frm;
    endPoint->setRect(0, 0, 8, 8);
    endPoint->setPen(lineColor);
    endPoint->setBrush(QBrush(frm->styleArcLineColor));
    }

/*! Destruktor */
GArc::~GArc(){
    delete line;
    delete endPoint;
    delete dialog;
    }

//! Nastaveni novych hodnot pri nacteni stylu
/*!
  \param lineC Barva hrany
  \param selectC Barva hrany pri najeti mysi
  */
void GArc::setStyle(QColor lineC, QColor selectC){
    QPen pen(lineC);
    pen.setWidthF(2.5);
    line->setPen(pen);
    lineColor.setColor(lineC);
    selectColor.setColor(selectC);
    endPoint->setPen(pen);
    endPoint->setBrush(QBrush(lineC));
    }

