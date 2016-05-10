/*!
\file communication.h
Hlavickovy soubor s tridou communication pouzivanou pro komunikaci klienta se serverem.
\author Pavel Fryz
\author Vojtech Bartl
\date 7. 5. 2012
*/
#ifndef COMMUNICATION_H
#define COMMUNICATION_H

#include <QTcpSocket>

//! Trida pro komunikaci se serverem
/*!
  Obsahuje QTcpSocket pro komunikaci
  */
class communication{

public:
    communication();            /*!< Konstruktor*/
    ~communication();           /*!< Destruktor*/
    //! Pripojeni na server
    /*!
      \param hostName Nazev hosta
      \param portNumber Cislo portu
      */
    bool connectToServer(QString hostName, int portNumber);
    //! Cteni radku ze serveru
    /*!
      \return Nacteny radek
      */
    QString readFromServer();
    //! Poslani radku na server
    /*!
      \param send Radek na poslani
      \return Zda se poslani zdarilo
      */
    bool sendToServer(QString send);
    //! Nacteni radku ze serveru a kontrola na spravnou odpoved
    /*!
      \return Zda byla ocekavana odpoved
      */
    bool checkOk();
    void stopConnect();     /*!< Ukonceni spojeni se serverem */

private:
    static const int TIMEOUT=5000;  /*!< Cas po ktery se ceka na spojeni*/
    QTcpSocket *socket;         /*!< Soket pro komunikaci*/

    };

#endif // COMMUNICATION_H
