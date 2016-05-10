/*!
\file server.h
Hlavickovy soubor s tridou Server, ktera slouzi pro
vytvoreni serveru pro komunikaci se serverem.
\author Pavel Fryz
\author Vojtech Bartl
\date 7. 5. 2012
*/

#ifndef SERVER_H
#define SERVER_H

#include <QTcpServer>

//! Trida pro server
/*!
Server prijima prichozi spojeni a
obsluhuje klienty. Server poskytuje
spolehlivou komunikaci.
*/
class Server : public QTcpServer
{
    Q_OBJECT
public:
    //! Vytvori server
    /*!
    Vytvori novy server.
    */
    Server(QObject *parent = 0);

protected:
    //! Obslouzi zadost na nove spojeni
    /*!
    Vytvori nove vlakno na obsluhu klienta.
    Komunikace probiha na pres uvedeny soket.
    \param socketDescriptor Deskriptor soketu pro komunikaci s klientem
    */
    void incomingConnection(int socketDescriptor);

};

#endif // SERVER_H
