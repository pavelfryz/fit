/*!
\file drawFrame.cpp
Implemenatace tridy hlavni sceny, ve ktere se edituje a zobrazuje sit.
Obsahuje veskerou obsluhu v prostoru sceny a uchovava si veskere informace
o graficke podobe site.
\author Pavel Fryz
\author Vojtech Bartl
\date 7. 5. 2012
*/
#include "drawFrame.h"
#include "mainwindow.h"
#include <iostream>
#include <QDomDocument>


/*! Konstruktor*/
drawFrame::drawFrame(){
    QGraphicsScene();
    drawArc=false;
    }

/*! Destruktor*/
drawFrame::~drawFrame(){
    for(unsigned int i=0; i<myPlaces.size(); i++)
        delete(myPlaces[i]);
    for(unsigned int i=0; i<myTransitions.size(); i++)
        delete(myTransitions[i]);
    for(unsigned int i=0; i<myArcs.size(); i++)
        delete(myArcs[i]);
    }

//! Nastaveni modu
/*!
  \param mode Mod, ktery se ma nastavit
  */
void drawFrame::setMode(int mode){
    this->myMode=mode;
    }


//! Zjisteni zda je hrana na zadanem bode
/*!
  \param point Bod ve scene
  \return Bod je na nejake hrane site
  */
bool drawFrame::isArcOnPoint(QPointF point){
    for(unsigned i=0; i<myArcs.size(); i++)
        if(myArcs[i]->line->contains(point))
            return true;
    return false;
    }

//! Zjisteni zda je misto na zadanem bode
/*!
  \param point Bod ve scene
  \return Bod je na nejakem miste site
  */
bool drawFrame::isPlaceOnPoint(QPointF point){
    for(unsigned i=0; i<myPlaces.size(); i++){
        QPointF mapPoint=myPlaces[i]->mapFromScene(point);
        if(myPlaces[i]->contains(mapPoint))
            return true;
        }
    return false;
    }

//! Zjisteni zda je prechod na zadanem bode
/*!
  \param point Bod ve scene
  \return Bod je na nejakem prechodu site
  */
bool drawFrame::isTransOnPoint(QPointF point){
    for(unsigned i=0; i<myTransitions.size(); i++){
        QPointF mapPoint=myTransitions[i]->mapFromScene(point);
        if(myTransitions[i]->contains(mapPoint))
            return true;
        }
    return false;
    }

//! Zjisteni ktera hrana je na zadanem bode
/*!
  \param point Bod ve scene
  \return Poradi hrany v seznamu hran site, na ktere je aktualni bod
  */
unsigned drawFrame::arcOnPoint(QPointF point){
    for(unsigned int i=0; i<myArcs.size(); i++)
        if(myArcs[i]->line->contains(point))
            return i;
    return -1;
    }

//! Zjisteni ktere misto je na zadanem bode
/*!
  \param point Bod ve scene
  \return Poradi mista v seznamu mist site, na ktere je aktualni bod
  */
unsigned drawFrame::placeOnPoint(QPointF point){
    for(unsigned int i=0; i<myPlaces.size(); i++){
        QPointF mapPoint=myPlaces[i]->mapFromScene(point);
        if(myPlaces[i]->contains(mapPoint))
            return i;
        }
    return -1;
    }

//! Zjisteni ktery prechod je na zadanem bode
/*!
  \param point Bod ve scene
  \return Poradi prechodu v seznamu prechodu site, na ktere je aktualni bod
  */
unsigned drawFrame::transOnPoint(QPointF point){
    for(unsigned int i=0; i<myTransitions.size(); i++){
        QPointF mapPoint=myTransitions[i]->mapFromScene(point);
        if(myTransitions[i]->contains(mapPoint))
            return i;
        }
    return -1;
    }

//! Zjisteni koncoveho bodu hrany na okraji mista, nebo prechodu
/*!
  \param point Bod ve scene
  \param arcPos Poradi hrany v seznamu hran site, pro kterou hledam koncovy bod
  \return Koncovy bod hrany
  */
QPointF drawFrame::computeEndPoint(QPointF point, int arcPos){
    QPointF result;
    if(isPlaceOnPoint(point)){
        unsigned pos=placeOnPoint(point);
        QRectF rect=myPlaces[pos]->boundingRect();
        QPointF topL=rect.topLeft();
        QPointF topLD=rect.topLeft();
        QPointF bottL=rect.bottomLeft();
        QPointF topR=rect.topRight();
        topL=myPlaces[pos]->mapToScene(topL);
        topLD=myPlaces[pos]->mapToScene(topLD);
        bottL=myPlaces[pos]->mapToScene(bottL);
        topR=myPlaces[pos]->mapToScene(topR);
        for(int i=0; i<myPlaces[pos]->RADIUS; i++){
            if(myArcs[arcPos]->line->contains(topL)){
                result=topL;
                break;
                }
            if(myArcs[arcPos]->line->contains(bottL)){
                result=bottL;
                break;
                }
            if(myArcs[arcPos]->line->contains(topLD)){
                result=topLD;
                break;
                }
            if(myArcs[arcPos]->line->contains(topR)){
                result=topR;
                break;
                }
            topL.rx()++;
            bottL.rx()++;
            topLD.ry()++;
            topR.ry()++;
            }
        result.rx()-=4;
        result.ry()-=4;
        return result;
        }
    else{
        unsigned pos=transOnPoint(point);
        QRectF rect=myTransitions[pos]->boundingRect();
        QPointF topL=rect.topLeft();
        QPointF topLD=rect.topLeft();
        QPointF bottL=rect.bottomLeft();
        QPointF topR=rect.topRight();
        topL=myTransitions[pos]->mapToScene(topL);
        topLD=myTransitions[pos]->mapToScene(topLD);
        bottL=myTransitions[pos]->mapToScene(bottL);
        topR=myTransitions[pos]->mapToScene(topR);
        for(int i=0; i<myTransitions[pos]->WIDTH; i++){
            if(myArcs[arcPos]->line->contains(topL)){
                result=topL;
                break;
                }
            if(myArcs[arcPos]->line->contains(bottL)){
                result=bottL;
                break;
                }
            topL.rx()++;
            bottL.rx()++;
            }
        for(int i=0; i<myTransitions[pos]->HEIGHT; i++){
            if(myArcs[arcPos]->line->contains(topLD)){
                result=topLD;
                break;
                }
            if(myArcs[arcPos]->line->contains(topR)){
                result=topR;
                break;
                }
            topLD.ry()++;
            topR.ry()++;
            }
        result.rx()-=4;
        result.ry()-=4;
        return result;
        }
    return result;
    }

//! Odstraneni prveku na zadane pozici
/*!
  \param point Bod ve scene
  */
void drawFrame::removeItemOnPoint(QPointF point){
    if(isPlaceOnPoint(point)){      //je na pozici misto
        unsigned pos=placeOnPoint(point);
        for(unsigned int i=0; i<myArcs.size(); i++){
            if(myArcs[i]->from==myPlaces[pos] || myArcs[i]->to==myPlaces[pos]){     //hrana z nebo do mista
                removeItem(myArcs[i]->line);        //se taky odstrani
                delete(myArcs[i]);
                myArcs.erase(myArcs.begin()+i);
                i=-1;
                }
            }
        //odstraneni mista
        removeItem(myPlaces[pos]);
        delete(myPlaces[pos]);
        myPlaces.erase(myPlaces.begin()+pos);
        return;
        }
    else if(isTransOnPoint(point)){     //je na pozici prechod
        unsigned pos=transOnPoint(point);
        for(unsigned int i=0; i<myArcs.size(); i++){
            if(myArcs[i]->from==myTransitions[pos] || myArcs[i]->to==myTransitions[pos]){
                removeItem(myArcs[i]->line);    //hrana z nebo do prechodu se taky odstrani
                delete(myArcs[i]);
                myArcs.erase(myArcs.begin()+i);
                i=-1;
                }
            }
        //odstraneni prechodu
        removeItem(myTransitions[pos]);
        delete(myTransitions[pos]);
        myTransitions.erase(myTransitions.begin()+pos);
        return;
        }
    else if(isArcOnPoint(point)){   //je na pozici hrana
        unsigned pos=arcOnPoint(point);
        removeItem(myArcs[pos]->line);      //odstrani se
        delete(myArcs[pos]);
        myArcs.erase(myArcs.begin()+pos);
        }
    return;
    }

/*! Vymaze celou sit */
void drawFrame::clearAll(){
    for(unsigned i=0; i<myPlaces.size(); i++){
        removeItem(myPlaces[i]);
        delete(myPlaces[i]);
        }
    myPlaces.clear();
    for(unsigned i=0; i<myTransitions.size(); i++){
        removeItem(myTransitions[i]);
        delete(myTransitions[i]);
        }
    myTransitions.clear();
    for(unsigned i=0; i<myArcs.size(); i++){
        removeItem(myArcs[i]->line);
        delete(myArcs[i]);
        }
    myArcs.clear();
    }

//! Vytvoreni XML ze site do souboru
/*!
  \param fileName Jmeno souboru do ktereho se bude ukladat
  \return Zda se vse zdarilo
  */
int drawFrame::createXML(QString fileName){
    QFile file(fileName);
    if(!file.open(QIODevice::WriteOnly))        //otevreni souboru
        return MainWindow::FILEOPEN;
    QDomDocument doc("");
    QDomNode node=doc.createProcessingInstruction("xml",  QString("version=\"1.0\" encoding=\"utf-8\""));
    doc.insertBefore(node, doc.firstChild());       //vlozeni hlavicky
    QDomElement root=doc.createElement("Petri-NET");    //korenovy element
    doc.appendChild(root);
    QDomElement name=doc.createElement("name");
    root.appendChild(name);
    QDomText nameT=doc.createTextNode(QString(window->myNetName.c_str()));
    name.appendChild(nameT);
    QDomElement desc=doc.createElement("description");
    root.appendChild(desc);
    QDomText descT=doc.createTextNode(QString(window->myNetDesc.c_str()));
    desc.appendChild(descT);
    for(unsigned i=0; i<myPlaces.size(); i++){      //ulozeni mist
        QPointF point=myPlaces[i]->scenePos();
        QDomElement place=doc.createElement("Place");
        root.appendChild(place);
        QDomElement xPl=doc.createElement("x");
        place.appendChild(xPl);
        QDomText t=doc.createTextNode(QString::number(point.rx()));
        xPl.appendChild(t);
        QDomElement yPl=doc.createElement("y");
        place.appendChild(yPl);
        QDomText txt=doc.createTextNode(QString::number(point.ry()));
        yPl.appendChild(txt);
        QDomElement id=doc.createElement("id");
        place.appendChild(id);
        QDomText tId=doc.createTextNode(QString::number(i));
        id.appendChild(tId);
        QDomElement token=doc.createElement("tokens");
        place.appendChild(token);
        QDomText tt=doc.createTextNode(QString(myPlaces[i]->dialog->tokens.c_str()));
        token.appendChild(tt);
        }
    for(unsigned i=0; i<myTransitions.size(); i++){     //ulozeni prechodu
        QDomElement tran=doc.createElement("Transition");
        root.appendChild(tran);
        QDomElement xTr=doc.createElement("x");
        tran.appendChild(xTr);
        QDomText xt=doc.createTextNode(QString::number(myTransitions[i]->x()));
        xTr.appendChild(xt);
        QDomElement yTr=doc.createElement("y");
        tran.appendChild(yTr);
        QDomText yt=doc.createTextNode(QString::number(myTransitions[i]->y()));
        yTr.appendChild(yt);
        QDomElement id=doc.createElement("id");
        tran.appendChild(id);
        QDomText tId=doc.createTextNode(QString::number(i));
        id.appendChild(tId);
        QDomElement guard=doc.createElement("guard");
        tran.appendChild(guard);
        QDomText gt=doc.createTextNode(QString(myTransitions[i]->dialog->guard.c_str()));
        guard.appendChild(gt);
        QDomElement expr=doc.createElement("expr");
        tran.appendChild(expr);
        QDomText et=doc.createTextNode(QString(myTransitions[i]->dialog->expr.c_str()));
        expr.appendChild(et);
        }
    for(unsigned i=0; i<myArcs.size(); i++){        //ulozeni hran
        QDomElement arc=doc.createElement("Arc");
        root.appendChild(arc);
        QDomElement orient=doc.createElement("orientation");
        arc.appendChild(orient);
        bool toPlace=false;
        for(unsigned j=0; j<myPlaces.size(); j++)       //zjisteni oreientace hrany
            if(myPlaces[j]==myArcs[i]->to)
                toPlace=true;
        if(toPlace){
            QDomText tOrient=doc.createTextNode(QString("toPlace"));
            orient.appendChild(tOrient);
            QDomElement placeId=doc.createElement("place_id");
            arc.appendChild(placeId);
            for(unsigned j=0; j<myPlaces.size(); j++){
                if(myPlaces[j]==myArcs[i]->to){
                    QDomText tPlId=doc.createTextNode(QString::number(j));
                    placeId.appendChild(tPlId);
                    }
                }
            QDomElement transId=doc.createElement("transition_id");
            arc.appendChild(transId);
            for(unsigned j=0; j<myTransitions.size(); j++){
                if(myTransitions[j]==myArcs[i]->from){
                    QDomText tTrId=doc.createTextNode(QString::number(j));
                    transId.appendChild(tTrId);
                    }
                }
            }
        else{
            QDomText tOrient=doc.createTextNode(QString("toTransition"));
            orient.appendChild(tOrient);
            QDomElement placeId=doc.createElement("place_id");
            arc.appendChild(placeId);
            for(unsigned j=0; j<myPlaces.size(); j++){
                if(myPlaces[j]==myArcs[i]->from){
                    QDomText tPlId=doc.createTextNode(QString::number(j));
                    placeId.appendChild(tPlId);
                    }
                }
            QDomElement transId=doc.createElement("transition_id");
            arc.appendChild(transId);
            for(unsigned j=0; j<myTransitions.size(); j++){
                if(myTransitions[j]==myArcs[i]->to){
                    QDomText tTrId=doc.createTextNode(QString::number(j));
                    transId.appendChild(tTrId);
                    }
                }
            }
        QDomElement constOrVar=doc.createElement("constOrVar");
        arc.appendChild(constOrVar);
        QDomText tCOV=doc.createTextNode(QString((myArcs[i]->dialog->constOrVar).c_str()));
        constOrVar.appendChild(tCOV);
        }
    QString xml=doc.toString();
    if(file.write(xml.toStdString().c_str())==-1)   //zapsani do souboru
        return MainWindow::FILEWRITE;
    file.close();
    return MainWindow::OK;
    }

//! Vytvori XML ze site pro poslani na server
/*!
  \param idOrSteps Id prechodu, ktery se ma provest, nebo maximalni pocet kroku
  \param name Jmeno site
  \param desc Popis site
  \return XML jako retezec
  */
QString drawFrame::createXMLrun(int idOrSteps, QString name, QString desc){
    name=name.simplified();
    desc=desc.simplified();
    for(unsigned i=0; i<myArcs.size(); i++)
        if(!myArcs[i]->dialog->constOrVar.empty())
            myArcs[i]->dialog->constOrVar=(QString(myArcs[i]->dialog->constOrVar.c_str()).simplified()).toStdString();
    for(unsigned i=0; i<myPlaces.size(); i++)
        if(!myPlaces[i]->dialog->tokens.empty())
            myPlaces[i]->dialog->tokens=(QString(myPlaces[i]->dialog->tokens.c_str()).simplified()).toStdString();
    for(unsigned i=0; i<myTransitions.size(); i++){
        if(!myTransitions[i]->dialog->guard.empty())
            myTransitions[i]->dialog->guard=(QString(myTransitions[i]->dialog->guard.c_str()).simplified()).toStdString();
        if(!myTransitions[i]->dialog->expr.empty())
            myTransitions[i]->dialog->expr=(QString(myTransitions[i]->dialog->expr.c_str()).simplified()).toStdString();
        }
    QDomDocument doc("");
    QDomElement root=doc.createElement("Petri-NET");    //korenovy element
    doc.appendChild(root);
    for(unsigned i=0; i<myPlaces.size(); i++){      //ulozeni mist
        QPointF point=myPlaces[i]->scenePos();
        QDomElement place=doc.createElement("Place");
        root.appendChild(place);
        QDomElement xPl=doc.createElement("x");
        place.appendChild(xPl);
        QDomText t=doc.createTextNode(QString::number(point.rx()));
        xPl.appendChild(t);
        QDomElement yPl=doc.createElement("y");
        place.appendChild(yPl);
        QDomText txt=doc.createTextNode(QString::number(point.ry()));
        yPl.appendChild(txt);
        QDomElement id=doc.createElement("id");
        place.appendChild(id);
        QDomText tId=doc.createTextNode(QString::number(i));
        id.appendChild(tId);
        QDomElement token=doc.createElement("tokens");
        place.appendChild(token);
        QDomText tt=doc.createTextNode(QString(myPlaces[i]->dialog->tokens.c_str()));
        token.appendChild(tt);
        }
    for(unsigned i=0; i<myTransitions.size(); i++){     //ulozeni prechodu
        QDomElement tran=doc.createElement("Transition");
        root.appendChild(tran);
        QDomElement xTr=doc.createElement("x");
        tran.appendChild(xTr);
        QDomText xt=doc.createTextNode(QString::number(myTransitions[i]->x()));
        xTr.appendChild(xt);
        QDomElement yTr=doc.createElement("y");
        tran.appendChild(yTr);
        QDomText yt=doc.createTextNode(QString::number(myTransitions[i]->y()));
        yTr.appendChild(yt);
        QDomElement id=doc.createElement("id");
        tran.appendChild(id);
        QDomText tId=doc.createTextNode(QString::number(i));
        id.appendChild(tId);
        QDomElement guard=doc.createElement("guard");
        tran.appendChild(guard);
        QDomText gt=doc.createTextNode(QString(myTransitions[i]->dialog->guard.c_str()));
        guard.appendChild(gt);
        QDomElement expr=doc.createElement("expr");
        tran.appendChild(expr);
        QDomText et=doc.createTextNode(QString(myTransitions[i]->dialog->expr.c_str()));
        expr.appendChild(et);
        }
    for(unsigned i=0; i<myArcs.size(); i++){        //ulozeni hran
        QDomElement arc=doc.createElement("Arc");
        root.appendChild(arc);
        QDomElement orient=doc.createElement("orientation");
        arc.appendChild(orient);
        bool toPlace=false;
        for(unsigned j=0; j<myPlaces.size(); j++)       //zjisteni oreientace hrany
            if(myPlaces[j]==myArcs[i]->to)
                toPlace=true;
        if(toPlace){
            QDomText tOrient=doc.createTextNode(QString("toPlace"));
            orient.appendChild(tOrient);
            QDomElement placeId=doc.createElement("place_id");
            arc.appendChild(placeId);
            for(unsigned j=0; j<myPlaces.size(); j++){
                if(myPlaces[j]==myArcs[i]->to){
                    QDomText tPlId=doc.createTextNode(QString::number(j));
                    placeId.appendChild(tPlId);
                    }
                }
            QDomElement transId=doc.createElement("transition_id");
            arc.appendChild(transId);
            for(unsigned j=0; j<myTransitions.size(); j++){
                if(myTransitions[j]==myArcs[i]->from){
                    QDomText tTrId=doc.createTextNode(QString::number(j));
                    transId.appendChild(tTrId);
                    }
                }
            }
        else{
            QDomText tOrient=doc.createTextNode(QString("toTransition"));
            orient.appendChild(tOrient);
            QDomElement placeId=doc.createElement("place_id");
            arc.appendChild(placeId);
            for(unsigned j=0; j<myPlaces.size(); j++){
                if(myPlaces[j]==myArcs[i]->from){
                    QDomText tPlId=doc.createTextNode(QString::number(j));
                    placeId.appendChild(tPlId);
                    }
                }
            QDomElement transId=doc.createElement("transition_id");
            arc.appendChild(transId);
            for(unsigned j=0; j<myTransitions.size(); j++){
                if(myTransitions[j]==myArcs[i]->to){
                    QDomText tTrId=doc.createTextNode(QString::number(j));
                    transId.appendChild(tTrId);
                    }
                }
            }
        QDomElement constOrVar=doc.createElement("constOrVar");
        arc.appendChild(constOrVar);
        QDomText tCOV=doc.createTextNode(QString((myArcs[i]->dialog->constOrVar).c_str()));
        constOrVar.appendChild(tCOV);
        }
    QDomElement steps=doc.createElement("steps");
    root.appendChild(steps);
    QDomText tSteps=doc.createTextNode(QString::number(idOrSteps));
    steps.appendChild(tSteps);
    QDomElement netName=doc.createElement("netName");
    root.appendChild(netName);
    QDomText tName=doc.createTextNode(name);
    netName.appendChild(tName);
    QDomElement description=doc.createElement("description");
    root.appendChild(description);
    QDomText tDesc=doc.createTextNode(desc);
    description.appendChild(tDesc);
    QString result=doc.toString(-1);
    return result;
    }

//! Nacteni site ze souboru nebo z retezce (XML)
/*!
  \param fileName Nazev souboru nebo retezec s XML
  \return Zda se vse zdarilo
  */
int drawFrame::readXML(QString fileName){
    QDomDocument doc("");
    bool fromFile=true;
    if(fileName.contains(QString("</Petri-NET>")))
        fromFile=false;
    if(fromFile){
        QFile file(fileName);
        if(!file.open(QIODevice::ReadOnly))     //otevreni souboru
            return MainWindow::FILEOPEN;
        if(!doc.setContent(&file)){
            file.close();
            return MainWindow::FILEREAD;        //nacteni obsahu souboru
            }
        file.close();
        }
    else doc.setContent(fileName);

    QDomElement docElem = doc.documentElement();
    if(docElem.tagName().compare("Petri-NET")!=0)       //zda obsahuje korenovy element
        return MainWindow::WRONGFORMAT;
    clearAll();     //vymaze se aktualni sit
    QDomNodeList placeList=docElem.elementsByTagName("Place");  //seznam mist
    QDomNodeList transList=docElem.elementsByTagName("Transition");     //seznam prechodu
    QDomNodeList arcList=docElem.elementsByTagName("Arc");      //seznam hran
    QDomElement name=docElem.firstChild().toElement();
    if(name.tagName().compare("name")==0)
        window->myNetName=name.text().toStdString();    //nacteni jmena site
    QDomElement desc=name.nextSibling().toElement();
    if(desc.tagName().compare("description")==0)        //nacteni popisu site
        window->myNetDesc=desc.text().toStdString();
    for(int i=0; i<placeList.count(); i++){
        int x=0, y=0;
        std::string tokens;
        QDomElement placeNode=placeList.at(i).toElement();
        QDomNode elemNode=placeNode.firstChild();
        while(!elemNode.isNull()){
            QDomElement elem=elemNode.toElement();
            QString elemText=elem.tagName();
            if(elemText.compare("x")==0)
                x=elem.text().toInt();
            else if(elemText.compare("y")==0)
                y=elem.text().toInt();
            if(elemText.compare("tokens")==0)
                tokens=elem.text().toStdString();
            elemNode=elemNode.nextSibling();
            }
        GPlace *place=new GPlace(x, y, this); //vytvoreni noveho mista
        place->dialog->tokens=tokens;
        this->addItem(place);
        myPlaces.push_back(place);
        }
    for(int i=0; i<transList.count(); i++){
        int x=0, y=0;
        std::string guard, expr, exec;
        QDomElement transNode=transList.at(i).toElement();
        QDomNode elemNode=transNode.firstChild();
        while(!elemNode.isNull()){
            QDomElement elem=elemNode.toElement();
            QString elemText=elem.tagName();
            if(elemText.compare("x")==0)
                x=elem.text().toInt();
            else if(elemText.compare("y")==0)
                y=elem.text().toInt();
            else if(elemText.compare("guard")==0)
                guard=elem.text().toStdString();
            else if(elemText.compare("expr")==0)
                expr=elem.text().toStdString();
            else if(elemText.compare("executable")==0)
                exec=elem.text().toStdString();
            elemNode=elemNode.nextSibling();
            }
        GTransition *trans=new GTransition(x, y, this); //vytvoreni noveho prechodu
        trans->dialog->guard=guard;
        trans->dialog->expr=expr;
        if(exec.compare("true")==0){
            trans->executable=true;
            }
        else{
            trans->executable=false;
            }
        this->addItem(trans);
        myTransitions.push_back(trans);
        }
    for(int i=0; i<arcList.count(); i++){       //hrany
        int placeId=0, transId=0;
        bool toPlace=false;
        std::string constOrVar;
        QDomElement arcNode=arcList.at(i).toElement();
        QDomNode elemNode=arcNode.firstChild();
        while(!elemNode.isNull()){
            QDomElement elem=elemNode.toElement();
            QString elemText=elem.tagName();
            if(elemText.compare("orientation")==0){     //zjisteni orientace hrany
                if(elem.text().compare("toPlace")==0)
                    toPlace=true;
                else toPlace=false;
                }
            else if(elemText.compare("place_id")==0)
                placeId=elem.text().toInt();
            else if(elemText.compare("transition_id")==0)
                transId=elem.text().toInt();
            else if(elemText.compare("constOrVar")==0)
                constOrVar=elem.text().toStdString();
            elemNode=elemNode.nextSibling();
            }
        if(toPlace){
            QPointF fromPoint=myTransitions[transId]->getCenterPoint(); //hrana do mista
            fromPoint=myTransitions[transId]->mapToScene(fromPoint);
            QPointF toPoint=myPlaces[placeId]->getCenterPoint();
            toPoint=myPlaces[placeId]->mapToScene(toPoint);
            GArc *arc=new GArc(this);
            arc->from=myTransitions[transId];
            arc->to=myPlaces[placeId];
            arc->dialog->constOrVar=constOrVar;
            arc->line=this->addLine(fromPoint.x(), fromPoint.y(), toPoint.x(), toPoint.y());
            myArcs.push_back(arc);
            QPointF end=computeEndPoint(toPoint, myArcs.size()-1);
            myArcs[myArcs.size()-1]->endPoint->setPos(end.x(), end.y());
            this->addItem(myArcs[myArcs.size()-1]->endPoint);
            }
        else{
            QPointF fromPoint=myPlaces[placeId]->getCenterPoint();  //hrana do prechodu
            fromPoint=myPlaces[placeId]->mapToScene(fromPoint);
            QPointF toPoint=myTransitions[transId]->getCenterPoint();
            toPoint=myTransitions[transId]->mapToScene(toPoint);
            GArc *arc=new GArc(this);
            arc->from=myPlaces[placeId];
            arc->to=myTransitions[transId];
            arc->dialog->constOrVar=constOrVar;
            arc->line=this->addLine(fromPoint.x(), fromPoint.y(), toPoint.x(), toPoint.y());
            myArcs.push_back(arc);
            QPointF end=computeEndPoint(toPoint, myArcs.size()-1);
            myArcs[myArcs.size()-1]->endPoint->setPos(end.x(), end.y());
            this->addItem(myArcs[myArcs.size()-1]->endPoint);
            }
        }
    repaintFrame();
    recomputeEndPoints();
    repaintSimulation();
    return MainWindow::OK;
    }

/*! Stisk mysi ve scene*/
void drawFrame::mousePressEvent(QGraphicsSceneMouseEvent *event){
    QPointF mousePos=event->scenePos(); //pozice mysi
    if(this->myMode==SIMULATION){    //simulace
        if(isTransOnPoint(mousePos)){
            unsigned pos=transOnPoint(mousePos);
            QString net=createXMLrun(pos, QString(window->myNetName.c_str()), "");
            if(!server->sendToServer(QString("Step.\n"))){        //posle se ze se ma simulovat
                QMessageBox::warning(window, tr("Connect error"), tr("Error while sending data"), QMessageBox::Ok, QMessageBox::Ok);
                window->showLoginDialog();
                return;
                }
            server->checkOk();
            if(!server->sendToServer(net+"\n")){        //posle se sit
                QMessageBox::warning(window, tr("Connect error"), tr("Error while sending data"), QMessageBox::Ok, QMessageBox::Ok);
                window->showLoginDialog();
                return;
                }
            QString result=server->readFromServer();
            readXML(result);
            recomputeEndPoints();
            }
        return;
        }
    if(this->myMode==MOVE){
        if(isPlaceOnPoint(mousePos) || isTransOnPoint(mousePos))
            QGraphicsScene::mousePressEvent(event);     //pri move modu se vola super()
        return;
        }
    //odstraneni
    if(this->myMode==REMOVE){
        removeItemOnPoint(mousePos);    //funkce pro odstraneni prvku na pozici
        }

    //jinak place ci transition mod - vkladani nebo tvoreni hran
    if(isPlaceOnPoint(mousePos)){   //misto na pozici mysi
        unsigned pos=placeOnPoint(mousePos);
        if(event->button()==Qt::RightButton){
            myPlaces[pos]->dialog->show();      //pri stisku praveho tlacitka se vykresli dialog
            return;
            }
        this->drawArc=true;         //bude se tvorit hrana
        this->fromItem=myPlaces[pos];   //zapamatuje se prvek ze ktereho se bude hrana tvorit
        return;
        }
    if(isTransOnPoint(mousePos)){   //prechod na pozici
        unsigned pos=transOnPoint(mousePos);
        if(event->button()==Qt::RightButton){   //stisk praveho vyvola dialog
            myTransitions[pos]->dialog->show();
            return;
            }
        this->drawArc=true;
        this->fromItem=myTransitions[pos];  //tvorba hrany z prvku
        return;
        }
    if(isArcOnPoint(mousePos)){     //hrana na pozici
        unsigned pos=arcOnPoint(mousePos);
        if(event->button()==Qt::RightButton){   //zobrazeni dialogu
            myArcs[pos]->dialog->show();
            }
        return;
        }


    //stisk mysi kde nic neni-vytvoreni noveho prvku
    if(this->myMode==PLACE){
        GPlace *place=new GPlace(mousePos.x(), mousePos.y(), this);   //vytvoreni mista
        this->addItem(place);
        myPlaces.push_back(place);      //ulozeni do seznamu
        }
    else if(this->myMode==TRANSITION){
        GTransition *trans=new GTransition(mousePos.x(), mousePos.y(), this);     //vytvoreni prechodu
        this->addItem(trans);
        myTransitions.push_back(trans);     //ulozeni do seznamu
        }
}


/*! Pusteni mysi*/
void drawFrame::mouseReleaseEvent(QGraphicsSceneMouseEvent *event){
    QPointF mousePos=event->scenePos();     //pozice mysi
    if(this->myMode==SIMULATION)
        return;
    if(this->myMode==MOVE){
        QGraphicsScene::mouseReleaseEvent(event);   //pri move modu se vola super()
        return;
        }

    //tvorba hrany
    if(this->drawArc){
        if(isPlaceOnPoint(mousePos)){       //mys pustena ma miste
            unsigned pos=placeOnPoint(mousePos);
            for(unsigned i=0; i<myPlaces.size(); i++){
                if(myPlaces[i]==this->fromItem){    //poku byl from taky misto, hrana se netvori
                    this->drawArc=false;
                    return;
                    }
                }
            //kontrola na vlozeni jiz existujici hrany (kterymkoliv smerem)
            for(unsigned i=0; i<myArcs.size(); i++){
                if((myArcs[i]->from==this->fromItem && myArcs[i]->to==myPlaces[pos]) || (myArcs[i]->from==myPlaces[pos] && myArcs[i]->to==this->fromItem)){
                    this->drawArc=false;
                    return;
                    }
                }
            GArc *arc=new GArc(this);
            arc->from=this->fromItem;   //vytvoreni hrany
            arc->to=myPlaces[pos];
            QPointF centPoint=myPlaces[pos]->getCenterPoint();
            centPoint=myPlaces[pos]->mapToScene(centPoint);     //vypocet souradnic
            QRectF rect=this->fromItem->boundingRect();
            QPointF centPointFrom=rect.center();
            centPointFrom=this->fromItem->mapToScene(centPointFrom);
            arc->line=this->addLine(centPointFrom.x(), centPointFrom.y(), centPoint.x(), centPoint.y(), arc->lineColor);
            myArcs.push_back(arc);      //ulozeni do seznamu
            QPointF end=computeEndPoint(centPoint, myArcs.size()-1);
            myArcs[myArcs.size()-1]->endPoint->setPos(end.x(), end.y());
            this->addItem(myArcs[myArcs.size()-1]->endPoint);
            }
        else if(isTransOnPoint(mousePos)){      //mys pustena na prechodu
            unsigned pos=transOnPoint(mousePos);
            for(unsigned i=0; i<myTransitions.size(); i++){
                if(myTransitions[i]==this->fromItem){       //pokud je prvek from taky prechod, hrana se netvori
                    this->drawArc=false;
                    return;
                    }
                }
            //kontrola na vlozeni jiz existujici hrany (kterymkoliv smerem)
            for(unsigned i=0; i<myArcs.size(); i++){
                if((myArcs[i]->from==this->fromItem && myArcs[i]->to==myTransitions[pos]) || (myArcs[i]->from==myTransitions[pos] && myArcs[i]->to==this->fromItem)){
                    this->drawArc=false;
                    return;
                    }
                }
            GArc *arc=new GArc(this);       //tvorba hrany
            arc->from=this->fromItem;
            arc->to=myTransitions[pos];
            QPointF centPoint=myTransitions[pos]->getCenterPoint();
            centPoint=myTransitions[pos]->mapToScene(centPoint);        //vypocet souradnic
            QRectF rect=this->fromItem->boundingRect();
            QPointF centPointFrom=rect.center();
            centPointFrom=this->fromItem->mapToScene(centPointFrom);
            arc->line=this->addLine(centPointFrom.x(), centPointFrom.y(), centPoint.x(), centPoint.y(), arc->lineColor);
            myArcs.push_back(arc);      //ulozeni do seznamu
            QPointF end=computeEndPoint(centPoint, myArcs.size()-1);
            myArcs[myArcs.size()-1]->endPoint->setPos(end.x(), end.y());
            this->addItem(myArcs[myArcs.size()-1]->endPoint);
            }
        //vytvoreni noveho prvku na miste pusteni mysi
        else{
            bool typePlace=false;       //zda se tvori hrana z mista do prechodu
            for(unsigned i=0; i<myPlaces.size(); i++)
                if(myPlaces[i]==this->fromItem)
                    typePlace=true;
            if(typePlace){
                GTransition *trans=new GTransition(mousePos.x(), mousePos.y(), this);     //tvorba prechodu
                this->addItem(trans);
                myTransitions.push_back(trans); //ulozeni prechodu do seznamu
                GArc *arc=new GArc(this);
                arc->from=this->fromItem;   //vytvoreni hrany
                arc->to=trans;
                QPointF centPoint=myTransitions[myTransitions.size()-1]->getCenterPoint();
                centPoint=myTransitions[myTransitions.size()-1]->mapToScene(centPoint);     //vypocet souradnic
                QRectF rect=this->fromItem->boundingRect();
                QPointF centPointFrom=rect.center();
                centPointFrom=this->fromItem->mapToScene(centPointFrom);
                arc->line=this->addLine(centPointFrom.x(), centPointFrom.y(), centPoint.x(), centPoint.y(), arc->lineColor);
                myArcs.push_back(arc);      //ulozeni hrany do seznamu
                QPointF end=computeEndPoint(centPoint, myArcs.size()-1);
                myArcs[myArcs.size()-1]->endPoint->setPos(end.x(), end.y());
                this->addItem(myArcs[myArcs.size()-1]->endPoint);
                }
            else{   //tvori se nove misto
                GPlace *place=new GPlace(mousePos.x(), mousePos.y(), this);
                this->addItem(place);
                place->setFlag(QGraphicsItem::ItemIsMovable, true);
                myPlaces.push_back(place);
                GArc *arc=new GArc(this);
                arc->from=this->fromItem;
                arc->to=place;
                QPointF centPoint=myPlaces[myPlaces.size()-1]->getCenterPoint();
                centPoint=myPlaces[myPlaces.size()-1]->mapToScene(centPoint);   //vypocet souradnic hrany
                QRectF rect=this->fromItem->boundingRect();
                QPointF centPointFrom=rect.center();
                centPointFrom=this->fromItem->mapToScene(centPointFrom);
                arc->line=this->addLine(centPointFrom.x(), centPointFrom.y(), centPoint.x(), centPoint.y(), arc->lineColor);
                myArcs.push_back(arc);
                QPointF end=computeEndPoint(centPoint, myArcs.size()-1);
                myArcs[myArcs.size()-1]->endPoint->setPos(end.x(), end.y());
                this->addItem(myArcs[myArcs.size()-1]->endPoint);
                }
            }
        this->drawArc=false;
        }
    }

/** Obsluha pohybu mysi*/
void drawFrame::mouseMoveEvent(QGraphicsSceneMouseEvent *event){
    recomputeEndPoints();
    QPointF mousePos=event->scenePos();     //aktualni pozice mysi
    if(this->myMode==MOVE){
        QGraphicsScene::mouseMoveEvent(event);
        if(isPlaceOnPoint(mousePos)){       //mys je na miste
            unsigned pos=placeOnPoint(mousePos);
            for(unsigned i=0; i<myArcs.size(); i++){        //pro kazdou hranu se prekresluji jeji souradnice dle pohybu
                if(myArcs[i]->from==myPlaces[pos]){ //hrana z mista
                    QPointF centPoint=myPlaces[pos]->getCenterPoint();
                    centPoint=myPlaces[pos]->mapToScene(centPoint);
                    QRectF rect=myArcs[i]->to->boundingRect();      //vypocet novych souradnic
                    QPointF centPointTo=rect.center();
                    centPointTo=myArcs[i]->to->mapToScene(centPointTo);
                    myArcs[i]->line->setLine(centPoint.x(), centPoint.y(), centPointTo.x(), centPointTo.y());
                    QPointF end=computeEndPoint(centPointTo, i);
                    myArcs[i]->endPoint->setPos(end.x(), end.y());
                    }
                else if(myArcs[i]->to==myPlaces[pos]){  //hrana do mista
                    QPointF centPoint=myPlaces[pos]->getCenterPoint();
                    centPoint=myPlaces[pos]->mapToScene(centPoint);
                    QRectF rect=myArcs[i]->from->boundingRect();        //vypocet novych souradnic
                    QPointF centPointFrom=rect.center();
                    centPointFrom=myArcs[i]->from->mapToScene(centPointFrom);
                    myArcs[i]->line->setLine(centPointFrom.x(), centPointFrom.y(), centPoint.x(), centPoint.y());
                    QPointF end=computeEndPoint(centPoint, i);
                    myArcs[i]->endPoint->setPos(end.x(), end.y());
                    }
                }
            }
        else if(isTransOnPoint(mousePos)){      //mys je na prechodu
            unsigned pos=transOnPoint(mousePos);
            for(unsigned i=0; i<myArcs.size(); i++){    //pro kazdou hranu
                if(myArcs[i]->from==myTransitions[pos]){    //hrana z prechodu
                    QPointF centPoint=myTransitions[pos]->getCenterPoint();
                    centPoint=myTransitions[pos]->mapToScene(centPoint);    //vypocet souradnic
                    QRectF rect=myArcs[i]->to->boundingRect();
                    QPointF centPointTo=rect.center();
                    centPointTo=myArcs[i]->to->mapToScene(centPointTo);
                    myArcs[i]->line->setLine(centPoint.x(), centPoint.y(), centPointTo.x(), centPointTo.y());
                    QPointF end=computeEndPoint(centPointTo, i);
                    myArcs[i]->endPoint->setPos(end.x(), end.y());
                    }
                else if(myArcs[i]->to==myTransitions[pos]){ //hrana do prechodu
                    QPointF centPoint=myTransitions[pos]->getCenterPoint();
                    centPoint=myTransitions[pos]->mapToScene(centPoint);    //vypocet souradnic
                    QRectF rect=myArcs[i]->from->boundingRect();
                    QPointF centPointFrom=rect.center();
                    centPointFrom=myArcs[i]->from->mapToScene(centPointFrom);
                    myArcs[i]->line->setLine(centPointFrom.x(), centPointFrom.y(), centPoint.x(), centPoint.y());
                    QPointF end=computeEndPoint(centPoint, i);
                    myArcs[i]->endPoint->setPos(end.x(), end.y());
                    }
                }
            }
        }
    QGraphicsScene::mouseMoveEvent(event);  //vola se super()
    if(isArcOnPoint(mousePos) && !isPlaceOnPoint(mousePos) && !isTransOnPoint(mousePos)){     //mys na hrane ji zmeni barvu
        unsigned pos=arcOnPoint(mousePos);
        myArcs[pos]->line->setPen(myArcs[pos]->selectColor);
        }
    else{
        for(unsigned i=0; i<myArcs.size(); i++)
            myArcs[i]->line->setPen(myArcs[i]->lineColor); //zmena na puvodni barvu
        }
    //nastaveni tool tipu pro vsechny prvky site
    for(unsigned i=0; i<myPlaces.size(); i++)
        myPlaces[i]->setToolTip(QString(("Tokens: "+myPlaces[i]->dialog->tokens).c_str()));
    for(unsigned i=0; i<myTransitions.size(); i++)
        myTransitions[i]->setToolTip(QString(("Guard: "+myTransitions[i]->dialog->guard+"\nExpression: "+myTransitions[i]->dialog->expr).c_str()));
    for(unsigned i=0; i<myArcs.size(); i++)
        myArcs[i]->line->setToolTip(QString(myArcs[i]->dialog->constOrVar.c_str()));
    return;
    }


//! Nacteni stylu ze souboru
/*!
  \param name Jmeno stylu ktery se ma nacist
  */
void drawFrame::readStyles(std::string name)
{
    int stylePlaceLineColorR=0, stylePlaceLineColorG=0, stylePlaceLineColorB=0;
    int stylePlaceFillColorR=0, stylePlaceFillColorG=0, stylePlaceFillColorB=0;
    int stylePlaceSelectColorR=0, stylePlaceSelectColorG=0, stylePlaceSelectColorB=0;
    int styleTransLineColorR=0, styleTransLineColorG=0, styleTransLineColorB=0;
    int styleTransFillColorR=0, styleTransFillColorG=0, styleTransFillColorB=0;
    int styleTransSelectColorR=0, styleTransSelectColorG=0, styleTransSelectColorB=0;
    int styleArcLineColorR=0, styleArcLineColorG=0, styleArcLineColorB=0;
    int styleArcSelectColorR=0, styleArcSelectColorG=0, styleArcSelectColorB=0;
    QDomDocument doc("");
    QFile file(QCoreApplication::applicationDirPath()+"/styles.xml");
    //QFile file("examples/styles.xml");
    if(!file.open(QIODevice::ReadOnly))     //otevreni souboru
        return;
    if(!doc.setContent(&file)){
        file.close();
        return;
        }
    file.close();
    QDomElement docElem = doc.documentElement();
    QDomNodeList styleList=doc.elementsByTagName("Style");
    for(int i=0; i<styleList.count(); i++){
        QDomElement styleNode=styleList.at(i).toElement();
        QDomNode elemNode=styleNode.firstChild();
        while(!elemNode.isNull()){
            QDomElement elem=elemNode.toElement();
            QString elemText=elem.tagName();
            if(elemText.toStdString().compare("name")==0){
                styleName=elem.text().toStdString();
                }
            if(elemText.toStdString().compare("placeradius")==0){       //nacteni polomeru mista
                stylePlaceRadius=elem.text().toInt();
                }
            if(elemText.toStdString().compare("transitionwidth")==0){   //nacteni sirky prachodu
                styleTransWidth=elem.text().toInt();
                }
            if(elemText.toStdString().compare("transitionheight")==0){  //nacteni vysky prechodu
                styleTransHeight=elem.text().toInt();
                }
            if(elemText.toStdString().compare("arclinecolor")==0){      //nacteni barvy hrany
                QDomNode colors=elem.firstChild();
                while(!colors.isNull()){
                    QDomElement colorElem=colors.toElement();
                    QString colorStr=colorElem.tagName();
                    if(colorStr.toStdString().compare("red")==0)
                        styleArcLineColorR=colorElem.text().toInt();
                    if(colorStr.toStdString().compare("green")==0)
                        styleArcLineColorG=colorElem.text().toInt();
                    if(colorStr.toStdString().compare("blue")==0)
                        styleArcLineColorB=colorElem.text().toInt();
                    colors=colors.nextSibling();
                    }
                }
            if(elemText.toStdString().compare("arcselectcolor")==0){        //nacteni barvy hrany pri vyberu
                QDomNode colors=elem.firstChild();
                while(!colors.isNull()){
                    QDomElement colorElem=colors.toElement();
                    QString colorStr=colorElem.tagName();
                    if(colorStr.toStdString().compare("red")==0)
                        styleArcSelectColorR=colorElem.text().toInt();
                    if(colorStr.toStdString().compare("green")==0)
                        styleArcSelectColorG=colorElem.text().toInt();
                    if(colorStr.toStdString().compare("blue")==0)
                        styleArcSelectColorB=colorElem.text().toInt();
                    colors=colors.nextSibling();
                    }
                }
            if(elemText.toStdString().compare("placelinecolor")==0){        //nacteni barvy mista
                QDomNode colors=elem.firstChild();
                while(!colors.isNull()){
                    QDomElement colorElem=colors.toElement();
                    QString colorStr=colorElem.tagName();
                    if(colorStr.toStdString().compare("red")==0)
                        stylePlaceLineColorR=colorElem.text().toInt();
                    if(colorStr.toStdString().compare("green")==0)
                        stylePlaceLineColorG=colorElem.text().toInt();
                    if(colorStr.toStdString().compare("blue")==0)
                        stylePlaceLineColorB=colorElem.text().toInt();
                    colors=colors.nextSibling();
                    }
                }
            if(elemText.toStdString().compare("placefillcolor")==0){        //nacteni vyplne barvy
                QDomNode colors=elem.firstChild();
                while(!colors.isNull()){
                    QDomElement colorElem=colors.toElement();
                    QString colorStr=colorElem.tagName();
                    if(colorStr.toStdString().compare("red")==0)
                        stylePlaceFillColorR=colorElem.text().toInt();
                    if(colorStr.toStdString().compare("green")==0)
                        stylePlaceFillColorG=colorElem.text().toInt();
                    if(colorStr.toStdString().compare("blue")==0)
                        stylePlaceFillColorB=colorElem.text().toInt();
                    colors=colors.nextSibling();
                    }
                }
            if(elemText.toStdString().compare("placeselectcolor")==0){      //nacteni barvy mista pri vyberu
                QDomNode colors=elem.firstChild();
                while(!colors.isNull()){
                    QDomElement colorElem=colors.toElement();
                    QString colorStr=colorElem.tagName();
                    if(colorStr.toStdString().compare("red")==0)
                        stylePlaceSelectColorR=colorElem.text().toInt();
                    if(colorStr.toStdString().compare("green")==0)
                        stylePlaceSelectColorG=colorElem.text().toInt();
                    if(colorStr.toStdString().compare("blue")==0)
                        stylePlaceSelectColorB=colorElem.text().toInt();
                    colors=colors.nextSibling();
                    }
                }
            if(elemText.toStdString().compare("transitionlinecolor")==0){       //barva prechodu
                QDomNode colors=elem.firstChild();
                while(!colors.isNull()){
                    QDomElement colorElem=colors.toElement();
                    QString colorStr=colorElem.tagName();
                    if(colorStr.toStdString().compare("red")==0)
                        styleTransLineColorR=colorElem.text().toInt();
                    if(colorStr.toStdString().compare("green")==0)
                        styleTransLineColorG=colorElem.text().toInt();
                    if(colorStr.toStdString().compare("blue")==0)
                        styleTransLineColorB=colorElem.text().toInt();
                    colors=colors.nextSibling();
                    }
                }
            if(elemText.toStdString().compare("transitionfillcolor")==0){       //barva vyplne prechodu
                QDomNode colors=elem.firstChild();
                while(!colors.isNull()){
                    QDomElement colorElem=colors.toElement();
                    QString colorStr=colorElem.tagName();
                    if(colorStr.toStdString().compare("red")==0)
                        styleTransFillColorR=colorElem.text().toInt();
                    if(colorStr.toStdString().compare("green")==0)
                        styleTransFillColorG=colorElem.text().toInt();
                    if(colorStr.toStdString().compare("blue")==0)
                        styleTransFillColorB=colorElem.text().toInt();
                    colors=colors.nextSibling();
                    }
                }
            if(elemText.toStdString().compare("transitionselectcolor")==0){     //barva prechodu pri vyberu
                QDomNode colors=elem.firstChild();
                while(!colors.isNull()){
                    QDomElement colorElem=colors.toElement();
                    QString colorStr=colorElem.tagName();
                    if(colorStr.toStdString().compare("red")==0)
                        styleTransSelectColorR=colorElem.text().toInt();
                    if(colorStr.toStdString().compare("green")==0)
                        styleTransSelectColorG=colorElem.text().toInt();
                    if(colorStr.toStdString().compare("blue")==0)
                        styleTransSelectColorB=colorElem.text().toInt();
                    colors=colors.nextSibling();
                    }
                }
            elemNode=elemNode.nextSibling();
            }
        //nacteni barev
        stylePlaceLineColor=QColor(stylePlaceLineColorR, stylePlaceLineColorG, stylePlaceLineColorB);
        stylePlaceFillColor=QColor(stylePlaceFillColorR, stylePlaceFillColorG, stylePlaceFillColorB);
        stylePlaceSelectColor=QColor(stylePlaceSelectColorR, stylePlaceSelectColorG, stylePlaceSelectColorB);
        styleTransLineColor=QColor(styleTransLineColorR, styleTransLineColorG, styleTransLineColorB);
        styleTransFillColor=QColor(styleTransFillColorR, styleTransFillColorG, styleTransFillColorB);
        styleTransSelectColor=QColor(styleTransSelectColorR, styleTransSelectColorG, styleTransSelectColorB);
        styleArcLineColor=QColor(styleArcLineColorR, styleArcLineColorG, styleArcLineColorB);
        styleArcSelectColor=QColor(styleArcSelectColorR, styleArcSelectColorG, styleArcSelectColorB);
        if(styleName.compare(name)==0){  //nalezen hleadny styl
            repaintFrame();     //prekresli se scena
            recomputeArcs();    //prepocitaji se hrany
            recomputeEndPoints();
            break;
            }
        }
    }


/*! Prekresleni sceny po nacteni noveho stylu */
void drawFrame::repaintFrame(){
    for(unsigned i=0; i<myArcs.size(); i++)
        myArcs[i]->setStyle(styleArcLineColor, styleArcSelectColor);
    for(unsigned i=0; i<myPlaces.size(); i++)
        myPlaces[i]->setStyle(stylePlaceLineColor, stylePlaceFillColor, stylePlaceSelectColor, stylePlaceRadius);
    for(unsigned i=0; i<myTransitions.size(); i++)
        myTransitions[i]->setStyle(styleTransLineColor, styleTransFillColor, styleTransSelectColor, styleTransWidth, styleTransHeight);
    }


/*! Prepocitani souradnic hran pri nacteni noveho stylu */
void drawFrame::recomputeArcs(){
    for(unsigned i=0; i<myArcs.size(); i++){
        QRectF fromRect=myArcs[i]->from->boundingRect();
        QPointF fromPoint=fromRect.center();
        fromPoint=myArcs[i]->from->mapToScene(fromPoint);
        QRectF toRect=myArcs[i]->to->boundingRect();
        QPointF toPoint=toRect.center();
        toPoint=myArcs[i]->to->mapToScene(toPoint);
        myArcs[i]->line->setLine(fromPoint.x(), fromPoint.y(), toPoint.x(), toPoint.y());
        QPointF end=computeEndPoint(toPoint, i);
        myArcs[i]->endPoint->setPos(end.x(), end.y());
        }
    }

/*! Prepocitani koncovych bodu hran */
void drawFrame::recomputeEndPoints(){
    for(unsigned i=0; i<myArcs.size(); i++){
        QRectF rect=myArcs[i]->to->boundingRect();
        QPointF centPointTo=rect.center();
        centPointTo=myArcs[i]->to->mapToScene(centPointTo);
        QPointF end=computeEndPoint(centPointTo, i);
        myArcs[i]->endPoint->setPos(end.x(), end.y());
        }
    }

/*! Prekresleni prechody puvodnimi barvami po skoneceni modu simulace*/
void drawFrame::simulationEnd(){
    for(unsigned i=0; i<myTransitions.size(); i++){
        myTransitions[i]->setPen(QPen(styleTransLineColor));
        myTransitions[i]->lineColor=QPen(styleTransLineColor);
        myTransitions[i]->selectColor=QPen(styleTransSelectColor);
        }
    }

/*! Prekresleni neproveditelnych prechodu cervenou barvou pri simulaci */
void drawFrame::repaintSimulation(){
    for(unsigned i=0 ;i<myTransitions.size(); i++){
        if(!myTransitions[i]->executable){
            myTransitions[i]->setPen(QPen(Qt::red));
            myTransitions[i]->selectColor=QPen(Qt::red);
            myTransitions[i]->lineColor=QPen(Qt::red);
            }
        }
    }
