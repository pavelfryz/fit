/*!
\file GTransition.cpp
Implementace tridy GTransition. Graficke podoby prechodu.
\author Pavel Fryz
\author Vojtech Bartl
\date 7. 5. 2012
*/
#include "GTransition.h"
#include "drawFrame.h"
#include "mainwindow.h"


//! Konstruktor prechodu
/*!
  \param x X-ova souradnice prechodu
  \param y Y-ova souradnice prechodu
  \param frm Scena do ktere se vklada
  */
GTransition::GTransition(int x, int y, drawFrame *frm):QGraphicsRectItem(0, 0, 0, 0){
    WIDTH=frm->styleTransWidth;
    HEIGHT=frm->styleTransHeight;
    executable=false;
    lineColor.setBrush(frm->styleTransLineColor);  //barva hrany
    fillColor.setColor(frm->styleTransFillColor);  //barva vyplne
    fillColor.setStyle(Qt::SolidPattern);   //vzor vyplne
    selectColor.setBrush(frm->styleTransSelectColor);    //barva hrany pri vyberu
    dialog=new myTransDialog();     //vytvoreni dialogu
    setRect(0, 0, WIDTH, HEIGHT);
    setPos(x, y);       //nastaveni pozice
    setPen(lineColor);
    setBrush(fillColor);
    setZValue(1);   //posun dopredu
    setFlag(QGraphicsItem::ItemIsMovable, true);
    setAcceptHoverEvents(true);
    frame=frm;
    }

/*! Destruktor*/
GTransition::~GTransition(){
    delete dialog;
    }

//! Zjisteni stredoveho bodu
/*!
  \return Bod ve stredu prechodu
  */
QPointF GTransition::getCenterPoint(){
    QRectF rect=this->boundingRect();
    QPointF point=rect.center();
    return point;
    }

/*! Mys najede na prechod*/
void GTransition::hoverEnterEvent(QGraphicsSceneHoverEvent *event){
    setPen(selectColor);
    }

/*! Mys opusti prechod*/
void GTransition::hoverLeaveEvent(QGraphicsSceneHoverEvent *event){
    setPen(lineColor);
    }

//! Nastaveni novych hodnot pri nacteni stylu
/*!
  \param lineC Nova barva obvodu prechodu
  \param fillC Nova barva vyplne
  \param selectC Nova barava obvodu pri najeti mysi
  \param w Nova sirka prechodu
  \param h Nova vyska prechodu
  */
void GTransition::setStyle(QColor lineC, QColor fillC, QColor selectC, int w, int h){
    int x=this->x();
    int y=this->y();
    WIDTH=w;
    HEIGHT=h;
    setPen(lineC);
    setBrush(fillC);
    selectColor.setBrush(selectC);
    setRect(0, 0, w, h);
    setPos(x, y);
    }
