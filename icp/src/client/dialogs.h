/*!
\file dialogs.h
Hlavickovy soubor s tridami dialogu pro modifikaci site.
Pro pridavani tokenu do mist, labelu na hrany a strazi a vyrazu do prechodu.
\author Pavel Fryz
\author Vojtech Bartl
\date 7. 5. 2012
*/
#ifndef DIALOGS_H
#define DIALOGS_H

#include <QtGui>

namespace Ui{
    class dialogArc;
    class dialogPlace;
    class dialogTrans;
    }


//! Dialog pro zadani promenne nebo konstanty na hranu
class myArcDialog: public QDialog{
    Q_OBJECT

public:
    myArcDialog(QWidget *parent=0);     /*!< Konstruktor*/
    ~myArcDialog();                 /*!< Destruktor*/
    std::string constOrVar;     /*!< Promenna nebo konstanta zadana v dialogu*/
    void show();                /*!< Zobrazeni dialogu*/

protected:
    //! Zjisteni textu zadaneho do dialogu
    /*!
      \return Text v dialogu (promenna nebo konstanta)
      */
    std::string textInDialog();

private:
    Ui::dialogArc *ui;          /*!< Graficka podoba */

private slots:
    void on_buttonOKArc_clicked();

};


/*!
    Dialog pro zadani tokenu do mista
*/
class myPlaceDialog: public QDialog{
    Q_OBJECT

public:
    myPlaceDialog(QWidget *parent=0);   /*!< Konstruktor*/
    ~myPlaceDialog();           /*!< Destruktor*/
    std::string tokens;     /*!< ulozene tokeny */
    void show();            /*!< Zobrazeni dialogu*/

protected:
    //! Zjisteni textu zadaneho do dialogu
    /*!
      \return Text v dialogu (tokeny)
      */
    std::string textInDialog();


private:
    Ui::dialogPlace *ui;     /*!< Graficka podoba */


private slots:
    void on_buttonOKPlace_clicked();
};


/*!
    Dialog pro zadani straze a vyrazu do prechodu
*/
class myTransDialog: public QDialog{
    Q_OBJECT

public:
    myTransDialog(QWidget *parent=0);   /*!< Konstruktor*/
    ~myTransDialog();           /*!< Destruktor*/
    std::string guard;      /*!< Ulozena straz */
    std::string expr;       /*!< Ulozeny vyraz */
    void show();            /*!< Zobrazeni dialogu*/

protected:
    //! Zjisteni straze zadane do dialogu
    /*!
      \return Straz v dialogu
      */
    std::string getGuardText();
    //! Zjisteni vyrazu zadaneho do dialogu
    /*!
      \return Vyraz v dialogu
      */
    std::string getExprText();


private:
    Ui::dialogTrans *ui;        /*!< Graficka podoba */

private slots:
    void on_buttonOKTrans_clicked();
};


#endif // DIALOGS_H
