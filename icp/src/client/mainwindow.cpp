/*!
\file mainwindow.cpp
Implementace hlavniho okna klienta. Obsluhuje vsechny dialogy
s uzivatelem a komunikuje se serverem.
\author Pavel Fryz
\author Vojtech Bartl
\date 7. 5. 2012
*/
#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "dialogs.h"
#include "ui_connection.h"
#include "ui_loginDialog.h"
#include "ui_registerDialog.h"
#include "ui_runDialog.h"
#include "ui_propertiesDialog.h"
#include "ui_openDialog.h"
#include "ui_findDialog.h"
#include "ui_simulationsDialog.h"
#include "ui_helpDialog.h"
#include <iostream>
#include <QMessageBox>
#include <QDomDocument>


/*! Konstruktor*/
MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    scene=new drawFrame();
    ui->graphicsView->setScene(scene);  //nastaveni sceny
    ui->graphicsView->setRenderHints(QPainter::Antialiasing);
    ui->PB_run->setVisible(false);      //tlacitko run az do modu simulace neviditelne
    ui->statusBar->showMessage("Disconnected");
    scene->setMode(scene->PLACE);
    scene->window=this;
    ui->PB_place->setEnabled(false);
    server=new communication();
    connected=false;
    scene->server=server;
    connectDialog=NULL;
    logDialog=NULL;
    regDialog=NULL;
    runDialog=NULL;
    propDialog=NULL;
    openDialog=NULL;
    findDialog=NULL;
    simDialog=NULL;
    newWindow=NULL;
    helpDialog=NULL;

}


/*! Destruktor*/
MainWindow::~MainWindow()
{
    delete ui;
    delete scene;
    delete connectDialog;
    delete logDialog;
    delete regDialog;
    delete runDialog;
    delete propDialog;
    delete openDialog;
    delete findDialog;
    delete simDialog;
    delete helpDialog;
    delete newWindow;
    delete server;
}


void MainWindow::changeEvent(QEvent *e)
{
    QMainWindow::changeEvent(e);
    switch (e->type()) {
    case QEvent::LanguageChange:
        ui->retranslateUi(this);
        break;
    default:
        break;
    }
}

/*! Menu exit*/
void MainWindow::on_actionExit_activated()
{
    if(connected)
        server->stopConnect();
    this->close();
}

/*! Stisk tlacitka place*/
void MainWindow::on_PB_place_clicked()
{
    ui->PB_place->setEnabled(false);
    ui->PB_transition->setEnabled(true);
    ui->PB_move->setEnabled(true);
    ui->PB_remove->setEnabled(true);
    scene->setMode(scene->PLACE);   //nastaveni modu a enable tlacitek
}

/*! Stisk tlacitka transition*/
void MainWindow::on_PB_transition_clicked()
{
    ui->PB_transition->setEnabled(false);
    ui->PB_place->setEnabled(true);
    ui->PB_move->setEnabled(true);
    ui->PB_remove->setEnabled(true);
    scene->setMode(scene->TRANSITION);
}

/*! Stisk tlacitka move*/
void MainWindow::on_PB_move_clicked()
{
    ui->PB_move->setEnabled(false);
    ui->PB_place->setEnabled(true);
    ui->PB_transition->setEnabled(true);
    ui->PB_remove->setEnabled(true);
    scene->setMode(scene->MOVE);
}

/*! Stisk tlacitka remove*/
void MainWindow::on_PB_remove_clicked()
{
    ui->PB_place->setEnabled(true);
    ui->PB_transition->setEnabled(true);
    ui->PB_move->setEnabled(true);
    ui->PB_remove->setEnabled(false);
    scene->setMode(scene->REMOVE);
}

/*! Menu open*/
void MainWindow::on_actionOpen_activated()
{
    QString fileName=QFileDialog::getOpenFileName(this, tr("Open File"),
          QCoreApplication::applicationDirPath()+"/nets", tr("All files(*);;XML files(*.xml)"));
    if(fileName==NULL)
        return;
    int ret=scene->readXML(fileName);   //kontrola zda nenastala chyba
    if(ret==FILEOPEN)
        QMessageBox::warning(this, tr("File open error"), tr("Error while opening file"), QMessageBox::Ok, QMessageBox::Ok);
    else if(ret==FILEREAD)
        QMessageBox::warning(this, tr("File read error"), tr("Error while reading file"), QMessageBox::Ok, QMessageBox::Ok);
    scene->simulationEnd();
}

/*! Menu save*/
void MainWindow::on_actionSave_activated()
{
    QString fileName=QFileDialog::getSaveFileName(this, tr("Save File"),
          QCoreApplication::applicationDirPath()+"/nets");
    if(fileName==NULL)
        return;
    int ret=scene->createXML(fileName);     //kontrola zda nenastala chyba
    if(ret==FILEOPEN)
        QMessageBox::warning(this, tr("File open error"), tr("Error while opening file"), QMessageBox::Ok, QMessageBox::Ok);
    else if(ret==FILEWRITE)
        QMessageBox::warning(this, tr("File write error"), tr("Error while writing into file"), QMessageBox::Ok, QMessageBox::Ok);
    else if(ret==WRONGFORMAT)
        QMessageBox::warning(this, tr("File read error"), tr("Wrong file format"), QMessageBox::Ok, QMessageBox::Ok);
}

/*! Menu clear*/
void MainWindow::on_actionClear_activated()
{
    scene->clearAll();  //funkce vymaze sit
    myNetName="";
    myNetDesc="";
}

/*! Menu login*/
void MainWindow::on_actionLogin_activated()
{
    logOrReg=true;
    showConnectDialog();    //zobrazi se connect dialog
}

/*! Menu register*/
void MainWindow::on_actionRegister_activated()
{
    logOrReg=false;
    showConnectDialog();    //zobrazi se register dialog
}

/*! Zobrazi login okno */
void MainWindow::showLoginDialog(){
    if(logDialog==NULL){
        logDialog=new myLoginDialog();  //vytvoreni dialogu
        logDialog->connectWindow(this); //a spojeni s oknem
        }
    logDialog->show();
    }

/*! Zobrazi connect okno */
void MainWindow::showConnectDialog(){
    if(connectDialog==NULL){
        connectDialog=new myConnectDialog();    //vytvoreni a spojeni s oknem
        connectDialog->connectWindow(this);
        }
    connectDialog->show();
    }

/*! Zobrazi register okno */
void MainWindow::showRegisterDialog(){
    if(regDialog==NULL){
        regDialog=new myRegisterDialog();   //vytvoreni a spojeni s oknem
        regDialog->connectWindow(this);
        }
    regDialog->show();
    }

/*! Zobrazi run okno */
void MainWindow::showRunDialog(){
    if(runDialog==NULL){
        runDialog=new myRunDialog();
        runDialog->connectWindow(this); //vytvoreni a spojeni s oknem
        }
    runDialog->show();
    }

/*! Zobrazi properties okno*/
void MainWindow::showPropertiesDialog(){
    if(propDialog==NULL){
        propDialog=new myPropertiesDialog();
        propDialog->connectWindow(this);    //vytvoreni a spojeni s oknem
        }
    propDialog->show();
    }

//! Zobrazi open dialog
/*!
  \param nets Popis vsech siti na serveru
  */
void MainWindow::showOpenDialog(QString nets){
    if(openDialog==NULL){
        openDialog=new myOpenDialog();
        openDialog->connectWindow(this);    //vytvoreni a spojeni s oknem
        }
    openDialog->readInfo(nets, "", QString("-1"));
    openDialog->show();
    }

/*! Zobrazi dialog s popisem simulaci */
void MainWindow::showSimulationsDialog(){
    if(simDialog==NULL){
        simDialog=new mySimulationsDialog();
        simDialog->connectWindow(this);    //vytvoreni a spojeni s oknem
        }
    simDialog->show();
    }

 /*! Zobrazi find dialog */
void MainWindow::showFindDialog(){
    if(findDialog==NULL){
        findDialog=new myFindDialog();
        findDialog->connectWindow(this);
        }
    findDialog->show();
    }

/*! Zobrazi napovedu */
void MainWindow::showHelpDialog(){
    if(helpDialog==NULL){
        helpDialog=new myHelpDialog();
        helpDialog->connectWindow(this);
        }
    helpDialog->show();
    }

//! Nastavi menu podle toho, zda jsme pripojeni na server
/*!
  \param enable Zda jsme pripojeni na server
  */
void MainWindow::setMenuEnable(bool enable){
    ui->actionOpen_from_server->setEnabled(enable);
    ui->actionSave_to_server->setEnabled(enable);
    ui->actionStart->setEnabled(enable);
    if(connected)
        ui->statusBar->showMessage("Connected");
    else ui->statusBar->showMessage("Disconnected");
    }

/*! Menu start simulation*/
void MainWindow::on_actionStart_activated()
{
    toSimulation(true); //funkce zmeni okno pro simulaci
    scene->setMode(scene->SIMULATION);
    QString net;
    if(!myNetName.empty())
        net=scene->createXMLrun(0, QString(myNetName.c_str()), "");
    else net=scene->createXMLrun(0, "", "");
    server->sendToServer(QString("Step.\n"));
    server->checkOk();
    server->sendToServer(net+"\n");
    QString result=server->readFromServer();
    scene->readXML(result);
}

/*! Menu stop simulation*/
void MainWindow::on_actionStop_activated()
{
    scene->simulationEnd();
    toSimulation(false);    //funkce zmeni okno nazpet
    ui->PB_place->setEnabled(false);
    ui->PB_transition->setEnabled(true);
    ui->PB_move->setEnabled(true);
    ui->PB_remove->setEnabled(true);
    scene->setMode(scene->PLACE);   //nastaveni modu a enable tlacitek
}

/*! Stisk tlacitka run*/
void MainWindow::on_PB_run_clicked()
{
    showRunDialog();    //zobrazi se dialog run
}

/*! Stisk tlacitak set properties*/
void MainWindow::on_actionSet_properties_activated()
{
    showPropertiesDialog();
}


//! Upravi okno pro simulaci
/*!
  \param enable Nastavit pro simulaci
  */
void MainWindow::toSimulation(bool enable){
    ui->PB_run->setVisible(enable);
    ui->PB_move->setVisible(!enable);
    ui->PB_place->setVisible(!enable);
    ui->PB_remove->setVisible(!enable);
    ui->PB_transition->setVisible(!enable);
    ui->actionStart->setEnabled(!enable);
    ui->actionStop->setEnabled(enable);
    }

/*! Zmena stylu na modern*/
void MainWindow::on_actionModern_activated()
{
    scene->readStyles("Modern");
    createConfig("Modern");
    readConfig();
    readStyle(lastUsedStyle);   //nacteni fontu pro hlavni okno
    changeFont();
}

/*! Zmena stylu na plain*/
void MainWindow::on_actionPlain_activated()
{
    scene->readStyles("Plain");
    createConfig("Plain");
    readConfig();
    readStyle(lastUsedStyle);   //nacteni fontu pro hlavni okno
    changeFont();
}

/*! Zmena fontu vsech tlacitek*/
void MainWindow::changeFont(){
    ui->PB_move->setFont(QFont(QString(font.c_str()), fontSize));
    ui->PB_place->setFont(QFont(QString(font.c_str()), fontSize));
    ui->PB_remove->setFont(QFont(QString(font.c_str()), fontSize));
    ui->PB_run->setFont(QFont(QString(font.c_str()), fontSize));
    ui->PB_transition->setFont(QFont(QString(font.c_str()), fontSize));
    }

//! Nacteni fontu pro hlavni okno dle stylu
/*!
  \param name Nazev stylu, ktery se ma nacitat
  */
void MainWindow::readStyle(std::string name){
    QDomDocument doc("");
    QFile file(QCoreApplication::applicationDirPath()+"/styles.xml");
    //QFile file("examples/styles.xml");
    if(!file.open(QIODevice::ReadOnly))     //otevreni souboru
        QMessageBox::warning(this, tr("File open error"), tr("Error while opening style file"), QMessageBox::Ok, QMessageBox::Ok);
    if(!doc.setContent(&file)){
        file.close();
        QMessageBox::warning(this, tr("File open error"), tr("Error while reading style file"), QMessageBox::Ok, QMessageBox::Ok);
        }
    file.close();
    std::string styleName;
    QDomElement docElem = doc.documentElement();
    QDomNodeList styleList=docElem.elementsByTagName("Style");
    for(int i=0; i<styleList.count(); i++){
        QDomElement styleNode=styleList.at(i).toElement();
        QDomNode elemNode=styleNode.firstChild();
        while(!elemNode.isNull()){
            QDomElement elem=elemNode.toElement();
            QString elemText=elem.tagName();
            if(elemText.toStdString().compare("name")==0)
                styleName=elem.text().toStdString();
            if(elemText.toStdString().compare("font")==0)
                font=elem.text().toStdString();
            if(elemText.toStdString().compare("fontsize")==0)
                fontSize=elem.text().toInt();
            elemNode=elemNode.nextSibling();
            }
        if(styleName.compare(name)==0)      //nalezen spravny styl
            break;
        }
    }

/*! Nastaveni stylu jako pri poslednim ukonceni */
void MainWindow::setLastStyle(){
    readConfig();               //nacteni stylu z konfiguracniho souboru
    readStyle(lastUsedStyle);   //nacteni fontu pro hlavni okno
    changeFont();
    scene->readStyles(lastUsedStyle);   //nastaveni stylu pro vykreslovaci okno
    }

//! Vytvoreni konfiguracniho souboru pro pristi otevreni
/*!
  \param style Nazev aktualniho stylu
  */
void MainWindow::createConfig(std::string style){
    QFile file(QCoreApplication::applicationDirPath()+"/config.xml");
    //QFile file("examples/config.xml");
    if(!file.open(QIODevice::WriteOnly)){        //otevreni souboru
        QMessageBox::warning(this, tr("File open error"), tr("Error while opening file"), QMessageBox::Ok, QMessageBox::Ok);
        return;
        }
    QDomDocument doc("");
    QDomElement root=doc.createElement("configuration");
    doc.appendChild(root);
    QDomElement sty=doc.createElement("style");
    root.appendChild(sty);
    QDomText txt=doc.createTextNode(QString(style.c_str()));    //ulozeni stylu
    sty.appendChild(txt);
    QString xml=doc.toString();
    if(file.write(xml.toStdString().c_str())==-1)
        QMessageBox::warning(this, tr("File write error"), tr("Error while writing to file"), QMessageBox::Ok, QMessageBox::Ok);
    file.close();
    }


/*! Nacteni konfiguracniho souboru-posledniho nastaveni */
void MainWindow::readConfig(){
    QDomDocument doc("");
    QFile file(QCoreApplication::applicationDirPath()+"/config.xml");
    //QFile file("examples/config.xml");
    if(!file.open(QIODevice::ReadOnly))     //otevreni souboru
        QMessageBox::warning(this, tr("File open error"), tr("Error while opening config file"), QMessageBox::Ok, QMessageBox::Ok);
    if(!doc.setContent(&file)){
        file.close();
        QMessageBox::warning(this, tr("File open error"), tr("Error while reading config file"), QMessageBox::Ok, QMessageBox::Ok);
        }
    file.close();
    QDomElement docElem = doc.documentElement();
    QDomNode node = docElem.firstChild();
    while(!node.isNull()) {
        QDomElement e = node.toElement();
        if(!e.isNull()) {
            if(e.tagName().toStdString().compare("style")==0)
                lastUsedStyle=e.text().toStdString();
            }
        node = node.nextSibling();
        }
    }

/*! Vytvoreni noveho okna */
void MainWindow::on_actionNew_activated()
{
    newWindow=new MainWindow();
    newWindow->setLastStyle();
    newWindow->show();
}

/*! Otevreni site ze serveru */
void MainWindow::on_actionOpen_from_server_activated()
{
    if(!server->sendToServer(QString("Load net.\n"))){        //posle se ze se ma simulovat
        QMessageBox::warning(this, tr("Connect error"), tr("Error while sending data"), QMessageBox::Ok, QMessageBox::Ok);
        server->checkOk();
        return;
        }
    QString nets=server->readFromServer();
    showOpenDialog(nets);
}

/*! Ulozeni site na server*/
void MainWindow::on_actionSave_to_server_activated()
{
    if(propDialog==NULL || propDialog->getName().toStdString().empty()){
        QMessageBox::warning(this, tr("Save error"), tr("No net name (Edit->Set properties)"), QMessageBox::Ok, QMessageBox::Ok);
        return;
        }
    QString net=scene->createXMLrun(-1, QString(myNetName.c_str()), propDialog->getDesc());
    if(!server->sendToServer(QString("Save net.\n"))){        //posle se ze se ma simulovat
        QMessageBox::warning(this, tr("Connect error"), tr("Error while sending data"), QMessageBox::Ok, QMessageBox::Ok);
        server->checkOk();
        return;
        }
    server->checkOk();
    if(!server->sendToServer(net+"\n")){
        QMessageBox::warning(this, tr("Connect error"), tr("Error while sending data"), QMessageBox::Ok, QMessageBox::Ok);
        server->checkOk();
        return;
        }
    server->checkOk();
}

/*! Stisk menu help*/
void MainWindow::on_actionHelp_activated()
{
    showHelpDialog();
}

//! Vyhledani zadanych siti na serveru
/*!
  \param pattern Vzor pro hledani site
  */
void MainWindow::findNets(QString pattern){
    openDialog->readFind(pattern);
    }


/////////////////////////////////////////////////////////////////////

/*! Konstruktor */
myConnectDialog::myConnectDialog(QWidget *parent):
        QDialog(parent),
        ui(new Ui::connectDialog){
            ui->setupUi(this);
            ui->host->setText("localhost"); //nastaveni implicitnich hodnot
            ui->port->setText("9354");
            window=NULL;
            }

/*! Destruktor */
myConnectDialog::~myConnectDialog(){
    delete ui;
    }

/*! Stisk Cancel tlacitka pri connect dialogu*/
void myConnectDialog::on_connectCancelButton_clicked()
{
    this->hide();
}

/*! Stisk OK pri connect dialog*/
void myConnectDialog::on_connectButton_clicked()
{
    this->hide();
    if(window->connected)
       window->server->stopConnect();
    QRegExp re("\\d+");
    if(!re.exactMatch(ui->port->text())){   //port musi byt cislo
        QMessageBox::warning(this, tr("Wrong port number"), tr("Port must be a number"), QMessageBox::Ok, QMessageBox::Ok);
        window->showConnectDialog();
        }
    else{
        if(ui->host->text().length()==0){
            QMessageBox::warning(this, tr("Wrong host"), tr("Host must be specified"), QMessageBox::Ok, QMessageBox::Ok);
            window->showConnectDialog();
            return;
            }
        if(window->server->connectToServer(ui->host->text(), ui->port->text().toInt())){
            QString serverLine=window->server->readFromServer();
            if(serverLine.compare("Hello, do you want to login or register?\n")!=0){
                QMessageBox::warning(this, tr("Connect error"), tr("Unknown protocol"), QMessageBox::Ok, QMessageBox::Ok);
                window->showConnectDialog();
                return;
                }
            }
        else{
            QMessageBox::warning(this, tr("Connect error"), tr("Unable to connect to server"), QMessageBox::Ok, QMessageBox::Ok);
            this->window->showConnectDialog();
            return;
            }
        if(this->window->logOrReg){  //jestli je login nebo registrace
            if(!window->server->sendToServer(QString("Login.\n"))){
                QMessageBox::warning(this, tr("Connect error"), tr("Error while sending data"), QMessageBox::Ok, QMessageBox::Ok);
                window->showConnectDialog();
                return;
                }
            this->window->showLoginDialog();    //vykresli se login dialog
            }
        else{
            if(!window->server->sendToServer(QString("Register.\n"))){
                QMessageBox::warning(this, tr("Connect error"), tr("Error while sending data"), QMessageBox::Ok, QMessageBox::Ok);
                window->showConnectDialog();
                return;
                }
            this->window->showRegisterDialog();     //vykresli se register dialog
            }
        window->server->checkOk();
        window->connected=true;
        }
}

/*! Spoji dialog s hlavnim oknem*/
void myConnectDialog::connectWindow(MainWindow *win){
    this->window=win;
    }

/////////////////////////////////////////////////////////////////////

/*! Konstruktor*/
myLoginDialog::myLoginDialog(QWidget *parent):
        QDialog(parent),
        ui(new Ui::loginDialog){
            ui->setupUi(this);
            window=NULL;
            }

/*! Destruktor */
myLoginDialog::~myLoginDialog(){
    delete ui;
    }

/*! Stisk tlacitka Cancel pri login*/
void myLoginDialog::on_loginCancelButton_clicked()
{
    window->server->sendToServer(QString("admin\n"));      //navraceni protokolu do uvodniho stavu
    window->server->checkOk();
    window->server->sendToServer(QString("XXXXXX\n"));
    window->server->checkOk();
    this->hide();
}

/*! Stisk Log In*/
void myLoginDialog::on_loginButton_clicked()
{
    this->hide();
    if(ui->login->text().length()==0 || ui->password->text().length()==0){
        QMessageBox::warning(this, tr("Login error"), tr("Login or password empty"), QMessageBox::Ok, QMessageBox::Ok);
        window->showLoginDialog();
        }
    if(!window->server->sendToServer(ui->login->text()+"\n")){      //poslani loginu
        QMessageBox::warning(this, tr("Connect error"), tr("Error while sending data"), QMessageBox::Ok, QMessageBox::Ok);
        window->showLoginDialog();
        return;
        }
    window->server->checkOk();     //kontrola odpovedi

    if(!window->server->sendToServer(ui->password->text()+"\n")){       //poslani hesla
        QMessageBox::warning(this, tr("Connect error"), tr("Error while sending data"), QMessageBox::Ok, QMessageBox::Ok);
        window->showLoginDialog();
        return;
        }
    QString response=window->server->readFromServer();
    if(response.compare("Error: Wrong login or password.\n")==0){     //kontrola odpovedi
        QMessageBox::warning(this, tr("Connect error"), tr("Wrong login or password"), QMessageBox::Ok, QMessageBox::Ok);
        window->showLoginDialog();
        window->server->sendToServer(QString("Login.\n"));      //pri chybnem hesle se bude znovu prihlasovat
        window->server->checkOk();
        return;
        }

    if(response.compare("Ok.\n")==0){
        window->connected=true;
        window->setMenuEnable(true);    //nastavi se menu(jako pripojen)
        }

}

/*! Spoji dialog s hlavnim oknem*/
void myLoginDialog::connectWindow(MainWindow *win){
    this->window=win;
    }

/////////////////////////////////////////////////////////////////////

/*! Konstruktor*/
myRegisterDialog::myRegisterDialog(QWidget *parent):
        QDialog(parent),
        ui(new Ui::registerDialog){
            ui->setupUi(this);
            window=NULL;
            }

/*! Destruktor */
myRegisterDialog::~myRegisterDialog(){
    delete ui;
    }

/*! Spoji dialog s hlavnim oknem*/
void myRegisterDialog::connectWindow(MainWindow *win){
    this->window=win;
    }

/*! Stisk potvrzeni registrace*/
void myRegisterDialog::on_registerButton_clicked()
{
    this->hide();
    if(ui->password->text().compare(ui->repPassword->text())!=0){
        QMessageBox::warning(this, tr("Wrong password"), tr("Passwords are not same"), QMessageBox::Ok, QMessageBox::Ok);
        window->showRegisterDialog();
        }
    else{
        if(!window->server->sendToServer(ui->login->text()+"\n")){
            QMessageBox::warning(this, tr("Connect error"), tr("Error while sending data"), QMessageBox::Ok, QMessageBox::Ok);
            window->showLoginDialog();
            return;
            }
        window->server->checkOk();
        if(!window->server->sendToServer(ui->password->text()+"\n")){
            QMessageBox::warning(this, tr("Connect error"), tr("Error while sending data"), QMessageBox::Ok, QMessageBox::Ok);
            window->showLoginDialog();
            return;
            }
        QString response=window->server->readFromServer();
        if(response.compare("Error: Could not add user.\n")){
            QMessageBox::warning(this, tr("Connect error"), tr("Could not add user"), QMessageBox::Ok, QMessageBox::Ok);
            window->showRegisterDialog();
            window->server->sendToServer(QString("Register.\n"));      //pri chybnem hesle se bude znovu prihlasovat
            window->server->checkOk();
            return;
            }
        if(response.compare("Ok.\n")==0){
            window->setMenuEnable(true);    //okno jako pripojen
            window->connected=true;
            }
        }
}

/*! Zruseni registrace*/
void myRegisterDialog::on_registerCancelButton_clicked()
{
    window->server->sendToServer(QString("admin\n"));
    window->server->checkOk();
    window->server->sendToServer(QString("XXXXX\n"));
    window->server->checkOk();
    this->hide();
}

/////////////////////////////////////////////////////////////////////

/*! Konstruktor*/
myRunDialog::myRunDialog(QWidget *parent):
        QDialog(parent),
        ui(new Ui::runDialog){
            ui->setupUi(this);
            window=NULL;
            }

/*! Destruktor */
myRunDialog::~myRunDialog(){
    delete ui;
    }

/*! Spoji dialog s hlavnim oknem*/
void myRunDialog::connectWindow(MainWindow *win){
    this->window=win;
    }

/*! Zruseni run dialogu*/
void myRunDialog::on_runCancelButton_clicked()
{
    this->hide();
}

/*! Potvrzeni run*/
void myRunDialog::on_runButton_clicked()
{
    this->hide();
    QRegExp re("\\d+");
    if(!re.exactMatch(ui->steps->text())){
        QMessageBox::warning(this, tr("Wrong step"), tr("Steps must be number"), QMessageBox::Ok, QMessageBox::Ok);
        window->showRunDialog();
        }
    QString net;
    if(!window->myNetName.empty())
        net=window->scene->createXMLrun(ui->steps->text().toInt(), QString(window->myNetName.c_str()), "");     //XML ze site
    else net=window->scene->createXMLrun(ui->steps->text().toInt(), "", "");
    if(!window->server->sendToServer(QString("Run simulation.\n"))){        //posle se ze se ma simulovat
        QMessageBox::warning(this, tr("Connect error"), tr("Error while sending data"), QMessageBox::Ok, QMessageBox::Ok);
        window->showLoginDialog();
        return;
        }
    window->server->checkOk();
    if(!window->server->sendToServer(net+"\n")){        //posle se sit
        window->showLoginDialog();
        return;
        }
    QString result=window->server->readFromServer();        //nacte se vysledek
    window->scene->readXML(result);         //a zpracuje se
}

/////////////////////////////////////////////////////////////////////

/*! Konstruktor*/
myPropertiesDialog::myPropertiesDialog(QWidget *parent):
        QDialog(parent),
        ui(new Ui::propertiesDialog){
            ui->setupUi(this);
            window=NULL;
            }

/*!< Destruktor */
myPropertiesDialog::~myPropertiesDialog(){
    delete ui;
    }

/*! Spoji dialog s hlavnim oknem*/
void myPropertiesDialog::connectWindow(MainWindow *win){
    this->window=win;
    }

/*! Stisk potvrzovaciho tlacitka*/
void myPropertiesDialog::on_propertiesButton_clicked()
{
    window->myNetName=ui->name->text().toStdString();
    window->myNetDesc=ui->description->toPlainText().toStdString();
    this->hide();
}

//! Vraci hodnotu zapsanou jako jmeno site
/*!
  \return Jmeno site
  */
QString myPropertiesDialog::getName(){
    return ui->name->text();
    }

//! Vraci hodnotu zapsanou jako popis site
/*!
  \return Popis site
  */
QString myPropertiesDialog::getDesc(){
    return ui->description->toPlainText();
    }

/*! Zobrazeni dialogu*/
void myPropertiesDialog::show(){
    ui->name->setText(QString(window->myNetName.c_str()));
    ui->description->setText(QString(window->myNetDesc.c_str()));
    QDialog::show();
    }
/////////////////////////////////////////////////////////////////////

/*! Konstruktor*/
myOpenDialog::myOpenDialog(QWidget *parent):
        QDialog(parent),
        ui(new Ui::openDialog){
            ui->setupUi(this);
            window=NULL;
            }

/*! Destruktor */
myOpenDialog::~myOpenDialog(){
    delete ui;
    }

/*! Spoji dialog s hlavnim oknem*/
void myOpenDialog::connectWindow(MainWindow *win){
    window=win;
    }

/*! Otevreni site ze serveru*/
void myOpenDialog::on_openButton_clicked()
{
    QDomDocument doc("");
    QDomElement root=doc.createElement("net");    //korenovy element
    doc.appendChild(root);
    QDomElement name=doc.createElement("name");
    root.appendChild(name);
    QDomText netName=doc.createTextNode(ui->name->itemText(ui->name->currentIndex()));
    name.appendChild(netName);
    QDomElement version=doc.createElement("version");
    root.appendChild(version);
    QDomText verText=doc.createTextNode(ui->version->itemText(ui->version->currentIndex()));
    version.appendChild(verText);
    QString result=doc.toString(-1);
    if(!window->server->sendToServer(result+"\n")){
        QMessageBox::warning(this, tr("Connect error"), tr("Error while sending data"), QMessageBox::Ok, QMessageBox::Ok);
        return;
        }
    QString loadedNet=window->server->readFromServer();
    window->scene->readXML(loadedNet);
    window->scene->simulationEnd();
    window->myNetName=ui->name->currentText().toStdString();
    window->myNetDesc=ui->dscription->toPlainText().toStdString();
    this->hide();
}


void myOpenDialog::on_findButton_clicked()
{
    window->showFindDialog();
}

void myOpenDialog::on_cancelButton_clicked()
{
    window->server->sendToServer(QString("Nejaka blbost.\n"));
    window->server->checkOk();
    this->hide();
}

void myOpenDialog::on_simulationsButton_clicked()
{
    window->showSimulationsDialog();
    window->simDialog->hide();
    readSimulations(myNets, ui->name->itemText(ui->name->currentIndex()));
    window->showSimulationsDialog();
}

//! Nacteni informaci o simulacich dane site
/*!
  \param nets Popis vsech siti ze serveru
  \param netName Jmeno site pro kterou chci vypsat simulace
  */
void myOpenDialog::readSimulations(QString nets, QString netName){
    QDomDocument doc("");
    doc.setContent(nets);
    QString myNetName;
    QString result;
    QDomElement docElem = doc.documentElement();
    QDomNodeList netList=docElem.elementsByTagName("Net");
    for(int i=0; i<netList.size(); i++){
        QDomElement netNode=netList.at(i).toElement();
        QDomNode elemNode=netNode.firstChild();
        while(!elemNode.isNull()){
            QDomElement elem=elemNode.toElement();
            QString elemText=elem.tagName();
            if(elemText.toStdString().compare("name")==0){
                myNetName=elem.text();
                }
            if(elemText.toStdString().compare("Simulation")==0){
                QDomNode vers=elem.firstChild();
                while(!vers.isNull()){
                    QDomElement versElem=vers.toElement();
                    QString versStr=versElem.tagName();
                    if(versStr.toStdString().compare("user")==0 && netName.compare(myNetName)==0)
                        result.append(versElem.text()+"    ");
                    if(versStr.toStdString().compare("date")==0 && netName.compare(myNetName)==0){
                        result.append(versElem.text()+"\n");
                        }
                    vers=vers.nextSibling();
                    }
            }
            elemNode=elemNode.nextSibling();
            }
        if(netName.compare(myNetName)==0){
            break;
            }
        }
    window->simDialog->ui->simulations->setText(result);
    }

//! Nacteni informeci o sitich ze serveru
/*!
  \param nets Popis vsech siti ze serveru
  \param netName Nazev site pro kterou hledam informace
  \param version Verze site pro kterou hledam informace
  */
void myOpenDialog::readInfo(QString nets, QString netName, QString version){
    if(netName.compare("")==0 && version.compare("-1")==0)
        ui->name->clear();
    myNets=nets;
    QDomDocument doc("");
    doc.setContent(nets);
    QString myNetName="neco tu musi byt";
    QString myVersion;
    bool read=true;
    QDomElement docElem = doc.documentElement();
    QDomNodeList netList=docElem.elementsByTagName("Net");
    for(int i=0; i<netList.size(); i++){
        ui->version->clear();
        QDomElement netNode=netList.at(i).toElement();
        QDomNode elemNode=netNode.firstChild();
        while(!elemNode.isNull()){
            QDomElement elem=elemNode.toElement();
            QString elemText=elem.tagName();
            if(elemText.toStdString().compare("name")==0 && netName.compare("")==0){
                ui->name->addItem(elem.text());
                }
            else if(elemText.toStdString().compare("name")==0){
                myNetName=elem.text();
                }
            if(elemText.toStdString().compare("version")==0){
                QDomNode vers=elem.firstChild();
                while(!vers.isNull()){
                    QDomElement versElem=vers.toElement();
                    QString versStr=versElem.tagName();
                    if(versStr.toStdString().compare("user")==0 && read)
                        ui->author->setText(versElem.text());
                    if(versStr.toStdString().compare("id")==0){
                        ui->version->addItem(versElem.text());
                        myVersion=versElem.text();
                        }
                    if(versStr.toStdString().compare("description")==0 && read)
                        ui->dscription->setText(versElem.text());
                    vers=vers.nextSibling();
                    }
                if(myVersion.compare(version)==0 && myNetName.compare(netName)==0)
                    read=false;
                }
            elemNode=elemNode.nextSibling();
            }
        if(netName.compare(myNetName)==0){
            ui->version->setCurrentIndex(ui->version->count()-1);
            break;
            }
        }
    ui->version->setCurrentIndex(ui->version->count()-1);
    if(read) ui->name->setCurrentIndex(ui->name->count()-1);

    }


//! Nacteni siti pri vyhledavani
/*!
  \param pattern Klic pro vyhledani siti
  */
void myOpenDialog::readFind(QString pattern){
    ui->name->clear();
    ui->version->clear();
    QDomDocument doc("");
    doc.setContent(myNets);
    bool read=false;
    QDomElement docElem = doc.documentElement();
    QDomNodeList netList=docElem.elementsByTagName("Net");
    for(int i=0; i<netList.size(); i++){
        QDomElement netNode=netList.at(i).toElement();
        QDomNode elemNode=netNode.firstChild();
        while(!elemNode.isNull()){
            QDomElement elem=elemNode.toElement();
            QString elemText=elem.tagName();
            if(elemText.toStdString().compare("name")==0 && elem.text().contains(pattern, Qt::CaseInsensitive)){
                ui->version->clear();
                ui->name->addItem(elem.text());
                read=true;
                }
            if(elemText.toStdString().compare("version")==0 && read){
                QDomNode vers=elem.firstChild();
                while(!vers.isNull()){
                    QDomElement versElem=vers.toElement();
                    QString versStr=versElem.tagName();
                    if(versStr.toStdString().compare("user")==0 && read)
                        ui->author->setText(versElem.text());
                    if(versStr.toStdString().compare("id")==0 && read){
                        ui->version->addItem(versElem.text());
                        }
                    if(versStr.toStdString().compare("description")==0 && read)
                        ui->dscription->setText(versElem.text());
                    vers=vers.nextSibling();
                    }
                }
            elemNode=elemNode.nextSibling();
            }
        read=false;
        }
    ui->version->setCurrentIndex(ui->version->count()-1);
    ui->name->setCurrentIndex(ui->name->count()-1);
    }


void myOpenDialog::on_name_activated(int index)
{
    readInfo(myNets, ui->name->itemText(index), QString("-1"));
    ui->name->setCurrentIndex(index);
}



void myOpenDialog::on_version_activated(int index)
{
    readInfo(myNets, ui->name->itemText(ui->name->currentIndex()), ui->version->itemText(index));
    ui->version->setCurrentIndex(index);
}


/////////////////////////////////////////////////////////////////////

/*! Konstruktor*/
myFindDialog::myFindDialog(QWidget *parent):
        QDialog(parent),
        ui(new Ui::findDialog){
            ui->setupUi(this);
            window=NULL;
            }

/*! Destruktor */
myFindDialog::~myFindDialog(){
    delete ui;
    }

/*! Spoji dialog s hlavnim oknem */
void myFindDialog::connectWindow(MainWindow *win){
    window=win;
    }

void myFindDialog::on_findButton_clicked()
{
    window->findNets(ui->find->text());
    this->hide();
}


void myFindDialog::on_cancelButton_clicked()
{
    this->hide();
}


/////////////////////////////////////////////////////////////////////////

/*! Konstruktor*/
mySimulationsDialog::mySimulationsDialog(QWidget *parent):
        QDialog(parent),
        ui(new Ui::simulationsDialog){
            ui->setupUi(this);
            window=NULL;
            }

/*! Destruktor */
mySimulationsDialog::~mySimulationsDialog(){
    delete ui;
    }

/*! Spoji dialog s hlavnim oknem */
void mySimulationsDialog::connectWindow(MainWindow *win){
    window=win;
    }



void mySimulationsDialog::on_okButton_clicked()
{
    this->hide();
}


////////////////////////////////////////////////////////////////////
/*! Konstruktor */
myHelpDialog::myHelpDialog(QWidget *parent):
        QDialog(parent),
        ui(new Ui::helpDialog){
            ui->setupUi(this);
            window=NULL;
            ui->textEdit->setText(QString("Napoveda k programu simulator Petriho siti\n"
                                          "Autori: Vojtech Bartl (xbartl03), Pavel Fryz (xfryzp00)\n\n"
                                          "Editace siti:\n"
                                          "Jsou dostupne ctyri mody. Mod pro vkladani mist, pro vkladani prechodu, pro pohyb s komponentami a pro odstranovani. Pri "
                                          "modu pro vkladani se po stisknuti tlacitka mysi vlozi do sceny bud misto, nebo prechod dle vybraneho modu. Hrany"
                                          "se vytvori spojenim dvou jiz existujicich prvku, nebo pretazenim mysi z jednoho prvku do volneho prostoru, kde se vytvori novy"
                                          "prvek a hrana do nej. Pri modu pro pohyb je mozne libovolne hybat s misty a prechody v ramci sceny. Pri modu odstranovani"
                                          "je mozne odstranit libovolne misto, prechod ci hranu. Pri odstraneni mista, nebo prechodu se odstrani vsechny hrany, ktere"
                                          "nalezi k dane komponenete. Pro vkladani tokenu mistu se na nem stiskne prave tlacitko mysi a objevi se dialog, kde se "
                                          "pridavaji tokeny, ktere musi byt oddeleny carkou. Tokny je mozne v dialogu i odstranovat odmazanim z dialogu. Na hranu se"
                                          "label vlozi take stiskem praveho tlacitka mysi na hrane. Objevi se dialog pro vlozeni labelu. Pro odstraneni labelu se opet muze odmazat"
                                          "z dialogu. Pro vkladani straze a vyrazu na prechod se na nem opet stiskne prave tlacitko mysi. Do dialogu se"
                                          "zadavaji straze oddelene znakem & a vyraz. Pro odstraneni se musi dana"
                                          "straz nebo vyraz odstranit z dialogu. Vsechny popisy site je mozno sledovat po najeti mysi na prislusnou komponentu site.\n\n"
                                          "Ukladani a nacitani site:\n"
                                          "Site je mozne ukladat a nacitat lokalne v menu (Save a Load), nebo ukladat a nacitat site z/na server. Pro praci se sitemi na"
                                          "severu je potreba se nejprve na server pripojit. Pri nacitani site je vyber ze vsech dostupnych verzi dane site. Je mozne vyhledat"
                                          "site dle nazvu. Pro zobrazeni vsech siti po vyhledani konkretnich, je treba nastavit vyhledavaci retezec na prazdny. Rovnez"
                                          "je dostupny i popis site. Nazev a popis site je potreba nastavit pri jejim ukladani na server v polozce menu Set ties.\n\n"
                                          "Prihlaseni na server:\n"
                                          "Prihlaseni na server se provadi v zalozce menu Server. Je mozne se prihlasit, nebo si zaregistrovat novy ucet. Pri kazdem"
                                          "prihlasovani je potreba zadat host a port serveru.\n\n"
                                          "Simulace:\n"
                                          "Simulace se provadi v zalozce munu Simulation->Start. V modu simulace se pocita s krokovanim simulace, kde se po kliknuti"
                                          "na prechod provede dany krok simulace. Cervene jsou zvyrazneny prechody, ktere neni aktualne mozne provest. Pro spusteni"
                                          "cele simulace je mozne stisknout tlacitko run, kde se pote zada maximalni pocet kroku simulace. Simulace se zastavi v "
                                          "zalozce menu Simulation->Stop. Dokud se simulace nezastavi, neni mozne sit upravovat."));
            }

/*! Destruktor */
myHelpDialog::~myHelpDialog(){
    delete ui;
    }

/*! Spoji dialog s hlavnim oknem*/
void myHelpDialog::connectWindow(MainWindow * win){
    window=win;
    }


void myHelpDialog::on_button_clicked()
{
    this->hide();
}
