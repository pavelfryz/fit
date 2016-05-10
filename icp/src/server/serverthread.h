/*!
\file serverthread.h
Hlavickovy soubor s tridou ServerThread pro vlakno serveru obsluhujici klienta.
\author Pavel Fryz
\author Vojtech Bartl
\date 7. 5. 2012
*/

#ifndef SERVERTHREAD_H
#define SERVERTHREAD_H

#include <QThread>
#include <QString>
#include <QTcpSocket>

//! Trida pro vlakno serveru
/*!
Vlakno slouzi pro obsluhu jednoho
konkretniho klienta.
*/
class ServerThread : public QThread
{
    Q_OBJECT
public:
    //! Vytvori vlakno
    /*!
    Vytvori nove vlakno pro obsluhu klienta.
    \param socketDescriptor Deskriptor soketu pro komunikaci s klientem
    \param parent Rodicovsky QObject
    */
    ServerThread(int socketDescriptor,QObject *parent = 0);

    //! Stav komunikace.
    /*! Vycet moznych stavu komunikace. */
    enum ConnectionState {
        WaitingForGreeting, /*!< Cekani na uvodni zpravu*/
        WaitingForLogin,    /*!< Cekani na login klienta pri prihlasovani*/
        WaitingForPassword, /*!< Cekani na heslo klienta pri prihlasovani*/
        WaitingForName,     /*!< Cekani na login klienta pri registraci*/
        WaitingForKey,      /*!< Cekani na heslo klienta pri registraci*/
        ReadyForUse,        /*!< Cekani na pozadovanou akci*/
        RunSimulation,      /*!< Cekani na simulovanou sit*/
        Step,               /*!< Cekani na krokovanou sit*/
        SaveNet,            /*!< Cekani na ukladanou sit*/
        LoadNet             /*!< Cekani na pozadovanou sit*/
    };

    //! Obsluhuje klienta.
    /*!
    Zacatek vlakna pro obsluhu klienta.
    Prijima a zpracovava zpravy od klienta
    a generuje odpovedi.
    */
    void run();

signals:
     //! Signal chyby
     /*!
     Signal pro oznamovani chyb
     \param socketError Chyba soketu
     */
     void error(QTcpSocket::SocketError socketError);

private:
     int socketDescriptor; /*!< Deskriptor soketu*/

     //! Vytvari odpoved.
     /*!
     Zpracovava zpravu od klienta a vytvrari odpoved na tuto zpravu.
     \param input Zprava od klienta
     \return Odpoved serveru
     */
     QString processInput(QString input);

     //! Overi klienta.
     /*!
     Overi login a heslo klienta.
     \param user Login klienta
     \param password Heslo klienta
     \return true Pokud je kombinace spravna, jinak false.
     */
     bool checkPassword(QString user, QString password);

     //! Prida klienta.
     /*!
     Prida klienta s danym login a heslem
     do databaze klientu.
     \param user Login klienta
     \param password Heslo klienta
     \return true Pokud se podarilo klienta pridat, jinak false.
     */
     bool addUser(QString user,QString password);

     //! Odsimuluje sit.
     /*!
     Odsimuluje klientem poslanou sit a sit po simulaci
     posle zpet klientovi.
     \param input Sit v xml formatu
     \return Odsimulovana sit v xml formatu.
     */
     QString runSimulation(QString input);

     //! Provede krok simulace site.
     /*!
     Provede krok simulace site poslane klientem
     a vyslednou sit posle zpet klientovi.
     \param input Sit v xml formatu
     \return Odsimulovana sit v xml formatu.
     */
     QString step(QString input);

     //! Ulozi sit.
     /*!
     Ulozi sit poslanou klientem
     do databaze siti.
     \param input Sit v xml formatu
     \return Retezec se stavem ulozeni
     */
     QString saveNet(QString input);

     //! Vrati seznam siti.
     /*!
     Vytvori seznam siti, vcetne informaci o verzich
     a simulacich.
     \return Seznam siti v xml formatu
     */
     QString getNets();

     //! Nacte sit.
     /*!
     Nacte sit pozadovanou klientem.
     \param input Retezec s nazvem a verzi pozadovane site
     \return Pozadovana sit v xml formatu
     */
     QString loadNet(QString input);

     ConnectionState state; /*!< Stav komunikace*/
     QString user; /*!< Prihlaseny uzivatel*/
};

#endif // SERVERTHREAD_H
