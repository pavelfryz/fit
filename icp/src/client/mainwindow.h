/*!
\file mainwindow.h
Hlavickovy soubor s hlavnim oknem programu a vsemi prislusnymi dialogy.
\author Pavel Fryz
\author Vojtech Bartl
\date 7. 5. 2012
*/
#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QtCore>
#include <QtGui>
#include <QMouseEvent>
#include <iostream>
#include "drawFrame.h"
#include "communication.h"

namespace Ui {
    class MainWindow;
    class connectDialog;
    class loginDialog;
    class registerDialog;
    class runDialog;
    class propertiesDialog;
    class openDialog;
    class findDialog;
    class simulationsDialog;
    class helpDialog;
}

class myConnectDialog;
class myLoginDialog;
class myRegisterDialog;
class myRunDialog;
class myPropertiesDialog;
class myOpenDialog;
class myFindDialog;
class mySimulationsDialog;
class myHelpDialog;

/*!
    Trida hlavniho okna klienta
  */
class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    MainWindow(QWidget *parent = 0);    /*!< Konstruktor*/
    ~MainWindow();                  /*!< Destruktor*/
    void showLoginDialog();     /*!< Zobrazi login okno */
    void showConnectDialog();   /*!< Zobrazi connect okno */
    void showRegisterDialog();  /*!< Zobrazi register okno */
    void showRunDialog();       /*!< Zobrazi run okno */
    void showPropertiesDialog();/*!< Zobrazi properties okno*/
    //! Zobrazi open dialog
    /*!
      \param nets Popis vsech siti na serveru
      */
    void showOpenDialog(QString nets);
    void showFindDialog();      /*!< Zobrazi find dialog */
    void showSimulationsDialog();/*!< Zobrazi dialog s popisem simulaci */
    void showHelpDialog();      /*!< Zobrazi napovedu */
    //! Nastavi menu podle toho, zda jsme pripojeni na server
    /*!
      \param enable Zda jsme pripojeni na server
      */
    void setMenuEnable(bool enable);
    //! Upravi okno pro simulaci
    /*!
      \param enable Nastavit pro simulaci
      */
    void toSimulation(bool enable);
    bool logOrReg;              /*!< Po dialogu connect nasleduje bud login nebo register */
    /*! Vycet moznych chyb pri zpracovani XML*/
    enum errors{
        OK,             /*!< Vse v poradku*/
        FILEOPEN,       /*!< Chyba pri otvirani soubor*/
        FILEREAD,       /*!< Chyba pri nacitani souboru*/
        FILEWRITE,      /*!< Chyba pri zapisu do souboru*/
        WRONGFORMAT     /*!< Chybny format pri nacitani site*/
        };
    //! Vytvoreni konfiguracniho souboru pro pristi otevreni
    /*!
      \param style Nazev aktualniho stylu
      */
    void createConfig(std::string style);
    void readConfig();                  /*!< Nacteni konfiguracniho souboru-posledniho nastaveni */
    //! Nacteni fontu pro hlavni okno dle stylu
    /*!
      \param name Nazev stylu, ktery se ma nacitat
      */
    void readStyle(std::string name);
    void setLastStyle();                /*!< Nastaveni stylu jako pri poslednim ukonceni */
    //! Zjisteni nazvu posledniho pouziteho stylu-nacten z konfiguracniho souboru
    /*!
      \return Nazev stylu
      */
    void changeFont();                  /*!< Zmena fontu vsech tlacitek*/
    std::string lastUsedStyle;          /*!< Posledni pouzity styl*/
    std::string font;                   /*!< Font z konfiguracniho souboru */
    int fontSize;                       /*!< Velikost fontu */
    communication *server;              /*!< Pripojeni na server */
    bool connected;                     /*!< Zda jsem pripojen na server */
    drawFrame *scene;                   /*!< Vykreslovaci scena */
    mySimulationsDialog *simDialog;     /*!< Dialog simulaci */
    //! Vyhledani zadanych siti na serveru
    /*!
      \param pattern Vzor pro hledani site
      */
    void findNets(QString pattern);
    std::string myNetName;              /*!< Jmeno aktualni site */
    std::string myNetDesc;              /*!< Popis aktualni site */


protected:
    void changeEvent(QEvent *e);

private:
    Ui::MainWindow *ui;             /*!< Graficka podoba */
    myConnectDialog *connectDialog; /*!< Dialog connect */
    myLoginDialog *logDialog;       /*!< Dialog login */
    myRegisterDialog *regDialog;    /*!< Dialog register */
    myRunDialog *runDialog;         /*!< Dialog run */
    myPropertiesDialog *propDialog; /*!< Dialog properties*/
    myOpenDialog *openDialog;       /*!< Dialog open */
    myFindDialog *findDialog;       /*!< Dialog vyhledani */
    myHelpDialog *helpDialog;       /*!< Dialog napovedy */
    MainWindow *newWindow;          /*!< Pripadne nove okno */
    

private slots:
    void on_actionHelp_activated();
    void on_actionSave_to_server_activated();
    void on_actionOpen_from_server_activated();
    void on_actionSet_properties_activated();
    void on_actionNew_activated();
    void on_actionPlain_activated();
    void on_actionModern_activated();
    void on_PB_run_clicked();
    void on_actionStop_activated();
    void on_actionStart_activated();
    void on_actionLogin_activated();
    void on_actionClear_activated();
    void on_actionSave_activated();
    void on_actionOpen_activated();
    void on_PB_remove_clicked();
    void on_PB_move_clicked();
    void on_PB_transition_clicked();
    void on_PB_place_clicked();
    void on_actionExit_activated();
    void on_actionRegister_activated();
};


//! Trida s dialogem pro pripojeni na server
class myConnectDialog: public QDialog{
    Q_OBJECT

public:
    myConnectDialog(QWidget *parent=0); /*!< Konstruktor */
    ~myConnectDialog();                 /*!< Destruktor */
    void connectWindow(MainWindow *);   /*!< Spoji dialog s hlavnim oknem*/

private:
    Ui::connectDialog *ui;      /*!< Graficka podoba */
    MainWindow *window;         /*!< Hlavni okno */

private slots:
    void on_connectButton_clicked();
    void on_connectCancelButton_clicked();
};

//! Trida s dialogem pro prihlaseni klienta na server
class myLoginDialog: public QDialog{
    Q_OBJECT

public:
    myLoginDialog(QWidget *parent=0);   /*!< Konstruktor */
    ~myLoginDialog();                   /*!< Destruktor */
    void connectWindow(MainWindow *);   /*!< Spoji dialog s hlavnim oknem*/

private:
    Ui::loginDialog *ui;        /*!< Graficka podoba */
    MainWindow *window;         /*!< Hlavni okno */


private slots:
    void on_loginButton_clicked();
    void on_loginCancelButton_clicked();
};


//! Trida s dialogem pro registraci noveho klienta na server
class myRegisterDialog: public QDialog{
    Q_OBJECT

public:
    myRegisterDialog(QWidget *parent=0);    /*!< Konstruktor */
    ~myRegisterDialog();                    /*!< Destruktor */
    void connectWindow(MainWindow *);       /*!< Spoji dialog s hlavnim oknem*/

private:
    Ui::registerDialog *ui;     /*!< Graficka podoba */
    MainWindow *window;         /*!< Hlavni okno */

private slots:
    void on_registerCancelButton_clicked();
    void on_registerButton_clicked();
};


/*!
    Trida s dialogem pro beh simulace
  */
class myRunDialog: public QDialog{
    Q_OBJECT

public:
    myRunDialog(QWidget *parent=0); /*!< Konstruktor */
    ~myRunDialog();                 /*!< Destruktor */
    void connectWindow(MainWindow *);  /*!< Spoji dialog s hlavnim oknem*/

private:
    Ui::runDialog *ui;          /*!< Graficka podoba */
    MainWindow *window;         /*!< Hlavni okno */


private slots:
    void on_runButton_clicked();
    void on_runCancelButton_clicked();
};


/*!
    Trida s dialogem pro nastaveni moznosti site
  */
class myPropertiesDialog: public QDialog{
    Q_OBJECT

public:
    myPropertiesDialog(QWidget *parent=0);  /*!< Konstruktor */
    ~myPropertiesDialog();                  /*!< Destruktor */
    void connectWindow(MainWindow *);       /*!< Spoji dialog s hlavnim oknem*/
    //! Vraci hodnotu zapsanou jako jmeno site
    /*!
      \return Jmeno site
      */
    QString getName();
    //! Vraci hodnotu zapsanou jako popis site
    /*!
      \return Popis site
      */
    QString getDesc();
    void show();            /*!< Zobrazeni dialogu*/

private:
    Ui::propertiesDialog *ui;       /*!< Graficka podoba */
    MainWindow *window;             /*!< Hlavni okno */

private slots:
    void on_propertiesButton_clicked();
};

/*!
    Trida s dialogem pro otevreni site ze serveru
  */
class myOpenDialog: public QDialog{
    Q_OBJECT

public:
    myOpenDialog(QWidget *parent=0);    /*!< Konstruktor */
    ~myOpenDialog();                    /*!< Destruktor */
    void connectWindow(MainWindow *);       /*!< Spoji dialog s hlavnim oknem*/
    //! Nacteni informeci o sitich ze serveru
    /*!
      \param nets Popis vsech siti ze serveru
      \param netName Nazev site pro kterou hledam informace
      \param version Verze site pro kterou hledam informace
      */
    void readInfo(QString nets, QString netName, QString version);
    //! Nacteni informaci o simulacich dane site
    /*!
      \param nets Popis vsech siti ze serveru
      \param netName Jmeno site pro kterou chci vypsat simulace
      */
    void readSimulations(QString nets, QString netName);
    //! Nacteni siti pri vyhledavani
    /*!
      \param pattern Klic pro vyhledani siti
      */
    void readFind(QString pattern);

private:
    Ui::openDialog *ui;             /*!< Graficka podoba */
    MainWindow *window;             /*!< Hlavni okno */
    QString myNets;                 /*!< Popis vsech siti ze serveru */


private slots:
    void on_version_activated(int index);
    void on_name_activated(int index);
    void on_simulationsButton_clicked();
    void on_openButton_clicked();
    void on_cancelButton_clicked();
    void on_findButton_clicked();
};

//! Trida s dialogem pro vyhledani site
class myFindDialog: public QDialog{
    Q_OBJECT

public:
    myFindDialog(QWidget *parent=0);    /*!< Konstruktor */
    ~myFindDialog();                    /*!< Destruktor */
    void connectWindow(MainWindow *);   /*!< Spoji dialog s hlavnim oknem */

private:
    Ui::findDialog *ui;         /*!< Graficka podoba */
    MainWindow *window;         /*!< Hlavni okno */


private slots:
    void on_findButton_clicked();
    void on_cancelButton_clicked();
};

//! Trida s dialogem simulaci
class mySimulationsDialog: public QDialog{
    Q_OBJECT

public:
    mySimulationsDialog(QWidget *parent=0); /*!< Konstruktor */
    ~mySimulationsDialog();                 /*!< Destruktor */
    void connectWindow(MainWindow *);       /*!< Spoji dialog s hlavnim oknem */
    Ui::simulationsDialog *ui;              /*!< Graficka podoba */

private:
    MainWindow *window;             /*!< Hlavni okno */



private slots:
    void on_okButton_clicked();
};

//! Trida s dialogem napovedy
class myHelpDialog: public QDialog{
    Q_OBJECT

public:
    myHelpDialog(QWidget *parent=0);    /*!< Konstruktor */
    ~myHelpDialog();                    /*!< Destruktor */
    void connectWindow(MainWindow *);   /*!< Spoji dialog s hlavnim oknem*/

private:
    Ui::helpDialog *ui;         /*!< Graficka podoba */
    MainWindow *window;         /*!< Hlavni okno */


private slots:
    void on_button_clicked();
};
#endif // MAINWINDOW_H
