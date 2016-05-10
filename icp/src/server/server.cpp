/*!
\file server.cpp
Soubor s implementaci tridy Server, ktera slouzi pro vytvoreni
serveru pro komunikaci s klientem.
\author Pavel Fryz
\author Vojtech Bartl
\date 7. 5. 2012
*/

#include "server.h"
#include "serverthread.h"

//! Vytvori server
/*!
Vytvori novy server.
*/
Server::Server(QObject *parent) :
    QTcpServer(parent)
{
}

//! Obslouzi zadost na nove spojeni
/*!
Vytvori nove vlakno na obsluhu klienta.
Komunikace probiha na pres uvedeny soket.
\param socketDescriptor Deskriptor soketu pro komunikaci s klientem
*/
void Server::incomingConnection(int socketDescriptor)
{
    ServerThread *thread = new ServerThread(socketDescriptor, this);
    connect(thread, SIGNAL(finished()), thread, SLOT(deleteLater()));
    thread->start();
}
