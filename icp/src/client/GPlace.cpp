/*!
\file GPlace.cpp
Implementace tridy GPlace. Graficke podoby mista.
\author Pavel Fryz
\author Vojtech Bartl
\date 7. 5. 2012
*/
#include "GPlace.h"
#include "drawFrame.h"
#include "mainwindow.h"
#include <iostream>


//! Konstrutor mista
/*!
  \param x X-ova souradnice mista
  \param y Y-ova souradnice mista
  \param frm Scena do ktere se vklada
  */
GPlace::GPlace(int x, int y, drawFrame *frm):QGraphicsEllipseItem(0, 0, 0, 0){
    RADIUS=frm->stylePlaceRadius;
    lineColor.setBrush(frm->stylePlaceLineColor);  //barva hrany
    fillColor.setColor(frm->stylePlaceFillColor);  //barva vyplne
    fillColor.setStyle(Qt::SolidPattern);   //vzor vyplne
    selectColor.setBrush(frm->stylePlaceSelectColor);    //barva hrany pri najeti mysi
    dialog=new myPlaceDialog();     //vytvoreni dialogu
    setRect(0, 0, RADIUS, RADIUS);
    setPos(x, y);       //nastaveni pozice
    setPen(lineColor);
    setBrush(fillColor);
    setZValue(1);       //posun dopredu
    setFlag(QGraphicsItem::ItemIsMovable, true);
    setAcceptHoverEvents(true);
    frame=frm;
    }

/*! Destruktor*/
GPlace::~GPlace(){
    delete dialog;
    }

//! Zjisteni stredoveho bodu
/*!
  \return Bod ve stredu mista
  */
QPointF GPlace::getCenterPoint(){
    QRectF rect=this->boundingRect();
    QPointF point=rect.center();
    return point;
    }

/*! Najeti misi na misto*/
void GPlace::hoverEnterEvent(QGraphicsSceneHoverEvent *event){
    setPen(selectColor);
    }

/*! Mys opusti misto*/
void GPlace::hoverLeaveEvent(QGraphicsSceneHoverEvent *event){
    setPen(lineColor);
    }

//! Nastaveni novych hodnot pri nacteni stylu
/*!
  \param lineC Nova barva kruznice
  \param fillC Nova barva vyplne
  \param selectC Nova barava kruznice pri najeti mysi
  \param r Novy polomer kruznice
  */
void GPlace::setStyle(QColor lineC, QColor fillC, QColor selectC, int r){
    int x=this->x();
    int y=this->y();
    RADIUS=r;
    setPen(lineC);
    setBrush(fillC);
    selectColor.setBrush(selectC);
    setRect(0, 0, r, r);
    setPos(x, y);
    }
