/*!
\file server2012.cpp
Soubor s funkci main, ktera spousti server.
\author Pavel Fryz
\author Vojtech Bartl
\date 7. 5. 2012
*/

#include <QtCore/QCoreApplication>
#include "server.h"

//! Spusti server
/*!
Spusti server pro obsluhu klientu.
Server nasloucha na portu 9354
*/
int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    Server server;
    server.listen(QHostAddress::Any,9354);
    return a.exec();
}
