/*!
\file communication.cpp
Implementace tridy communication, ktera se pouziva pro snadnejsi komunikaci
klienata se serverem. Vytvari a ukoncuje spojeni. Posila a prijima zpravy.
\author Pavel Fryz
\author Vojtech Bartl
\date 7. 5. 2012
*/
#include "communication.h"


/*! Konstruktor*/
communication::communication(){
    socket=new QTcpSocket();
    }

/*! Destruktor*/
communication::~communication(){
    delete socket;
    }

//! Pripojeni na server
/*!
  \param hostName Nazev hosta
  \param portNumber Cislo portu
  */
bool communication::connectToServer(QString hostName, int portNumber){
    socket->connectToHost(hostName, portNumber);
    if(!socket->waitForConnected(TIMEOUT)){
        return false;
        }
    return true;
    }

//! Cteni radku ze serveru
/*!
  \return Nacteny radek
  */
QString communication::readFromServer(){
    while(socket->waitForReadyRead(TIMEOUT) && socket->canReadLine()){
        return socket->readLine();
        }
    return NULL;
    }

//! Poslani radku na server
/*!
  \param send Radek na poslani
  \return Zda se poslani zdarilo
  */
bool communication::sendToServer(QString send){
    if(socket->write(send.toStdString().c_str())==-1)
        return false;
    return true;
    }


//! Nacteni radku ze serveru a kontrola na spravnou odpoved
/*!
  \return Zda byla ocekavana odpoved
  */
bool communication::checkOk(){
    if(readFromServer().compare("Ok.\n")!=0)
        return false;
    return true;
    }

/*! Ukonceni spojeni se serverem */
void communication::stopConnect(){
    socket->write("Bye.\n");
    checkOk();
    socket->disconnectFromHost();
    if(socket->state() == QAbstractSocket::UnconnectedState || socket->waitForDisconnected(TIMEOUT)){}
    }
