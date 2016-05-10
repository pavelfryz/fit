/*!
\file pn2012.cpp
Hlavni funkce klienta
\author Pavel Fryz
\author Vojtech Bartl
\date 7. 5. 2012
*/
#include <QtGui/QApplication>
#include "mainwindow.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    w.setLastStyle();   //zobrazeni s poslednim nactenym stylem
    w.show();
    return a.exec();
}
