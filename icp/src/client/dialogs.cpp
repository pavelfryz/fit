/*!
\file dialogs.cpp
Implemenatace trid dialogu pro pridavani tokenu do mist, pridavani labelu na
hrany a pridavani strazi a vyrazu do prechodu. Obsahuje metody na kontrolu
vkladanych hodnot, ktere musi dodrzet presna pravidla. Jiny format vstupniho
textu je zamitnut.
\author Pavel Fryz
\author Vojtech Bartl
\date 7. 5. 2012
*/
#include "ui_dialogArc.h"
#include "ui_dialogPlace.h"
#include "ui_dialogTrans.h"
#include "dialogs.h"

/*! Konstruktor*/
myArcDialog::myArcDialog(QWidget *parent):
        QDialog(parent),
        ui(new Ui::dialogArc){
            ui->setupUi(this);
            }

/*! Destruktor*/
myArcDialog::~myArcDialog(){
    delete ui;
    }

//! Zjisteni textu zadaneho do dialogu
/*!
  \return Text v dialogu (promenna nebo konstanta)
  */
std::string myArcDialog::textInDialog(){
    QString text;
    text=ui->textEdit->toPlainText();
    return text.toStdString();
    }

/*! Stisk tlacitka OK-ulozi se nova hodnota z dialogu*/
void myArcDialog::on_buttonOKArc_clicked()
{
    this->hide();
    QRegExp re("\\d*\\s*|[a-zA-Z]*\\s*|^[a-zA-Z][a-zA-Z0-9]*\\s*");    //koretni jsou jen cisla, jen pismena, nebo promenna musi zacinat alespon jednim pismenem
    if(!re.exactMatch(QString(textInDialog().c_str()))){
        QMessageBox::warning(this, tr("Warning"), tr("Wrong constant or variable"), QMessageBox::Ok, QMessageBox::Ok);
        this->show();
        return;
        }
    constOrVar=textInDialog();
}

/*!< Zobrazeni dialogu*/
void myArcDialog::show(){
    ui->textEdit->setText(QString(constOrVar.c_str()));
    QDialog::show();
    }

/////////////////////////////////////////////////////////////////////

/*! Konstruktor*/
myPlaceDialog::myPlaceDialog(QWidget *parent):
        QDialog(parent),
        ui(new Ui::dialogPlace){
            ui->setupUi(this);
            }

/*! Destruktor*/
myPlaceDialog::~myPlaceDialog(){
    delete ui;
    }

//! Zjisteni textu zadaneho do dialogu
/*!
  \return Text v dialogu (tokeny)
  */
std::string myPlaceDialog::textInDialog(){
    QString text;
    text=ui->textEdit->toPlainText();
    return text.toStdString();
    }

/*! Stisk tlacitka OK-ulozi se nova hodnota z dialogu*/
void myPlaceDialog::on_buttonOKPlace_clicked()
{
    this->hide();
    QRegExp re("^\\d+(,\\s*\\d+)*|");    //tokeny musi byt cislice oddelene carkami (libovolny pocet bilych znaku mezi nimi)
    if(!re.exactMatch(QString(textInDialog().c_str()))){
        QMessageBox::warning(this, tr("Warning"), tr("Wrong tokens format"), QMessageBox::Ok, QMessageBox::Ok);
        this->show();
        return;
        }
    tokens=textInDialog();

}

/*! Zobrazeni dialogu*/
void myPlaceDialog::show(){
    ui->textEdit->setText(QString(tokens.c_str()));
    QDialog::show();
    }

/////////////////////////////////////////////////////////////////////

/*! Konstruktor*/
myTransDialog::myTransDialog(QWidget *parent):
        QDialog(parent),
        ui(new Ui::dialogTrans){
            ui->setupUi(this);
            }

/*! Destruktor*/
myTransDialog::~myTransDialog(){
    delete ui;
    }

//! Zjisteni straze zadane do dialogu
/*!
  \return Straz v dialogu
  */
std::string myTransDialog::getGuardText(){
    QString text;
    text=ui->textEditGuard->toPlainText();
    return text.toStdString();
    }

//! Zjisteni vyrazu zadaneho do dialogu
/*!
  \return Vyraz v dialogu
  */
std::string myTransDialog::getExprText(){
    QString text;
    text=ui->textEditExpr->toPlainText();
    return text.toStdString();
    }

/*! Stisk tlacitka OK-ulozi se nove hodnoty z dialogu*/
void myTransDialog::on_buttonOKTrans_clicked()
{
    this->hide();
    QRegExp reG("(([a-zA-Z]+|^[a-zA-Z][a-zA-Z0-9]+)(<|<=|>=|>|==|!=){1}(\\d+|[a-zA-Z]+|[a-zA-Z]+[a-zA-Z0-9]+))?(\\s*&\\s*([a-zA-Z]+|[a-zA-Z][a-zA-Z0-9]*)(<|<=|>=|>|==|!=){1}(\\d+|[a-zA-Z]+|[a-zA-Z]+[a-zA-Z0-9]+))*");
    if(!reG.exactMatch(QString(getGuardText().c_str()))){
        QMessageBox::warning(this, tr("Warning"), tr("Wrong guard format"), QMessageBox::Ok, QMessageBox::Ok);
        this->show();
        return;
        }
    QRegExp reE("([a-zA-Z]+|^[a-zA-Z][a-zA-Z0-9]+)?\\s*={1}\\s*(\\d+|[a-zA-Z]+|[a-zA-Z][a-zA-Z0-9]+){1}\\s*((\\+|\\-){1}\\s*(\\d+|[a-zA-Z]+|[a-zA-Z][a-zA-Z0-9]+){1}\\s*)*|");
    if(!reE.exactMatch(getExprText().c_str())){
        QMessageBox::warning(this, tr("Warning"), tr("Wrong expression format"), QMessageBox::Ok, QMessageBox::Ok);
        this->show();
        return;
        }
    guard=getGuardText();
    expr=getExprText();
}

 /*! Zobrazeni dialogu*/
void myTransDialog::show(){
    ui->textEditExpr->setText(QString(expr.c_str()));
    ui->textEditGuard->setText(QString(guard.c_str()));
    QDialog::show();
    }
