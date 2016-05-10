/*!
\file serverthread.cpp
Soubor s implementaci tridy ServerThread, ktera slouzi
pro vlakno, ktere obsluhuje jednoho klienta.
\author Pavel Fryz
\author Vojtech Bartl
\date 7. 5. 2012
*/

#include "serverthread.h"
#include "net/net.h"
#include <QtNetwork>
#include <QDataStream>
#include <QString>
#include <QDomDocument>
#include <QTextStream>

//! Vytvori vlakno
/*!
Vytvori nove vlakno pro obsluhu klienta.
\param socketDescriptor Deskriptor soketu pro komunikaci s klientem
\param parent Rodicovsky QObject
*/
ServerThread::ServerThread(int socketDescriptor, QObject *parent) :
    QThread(parent), socketDescriptor(socketDescriptor),state(WaitingForGreeting)
{
}

//! Obsluhuje klienta.
/*!
Zacatek vlakna pro obsluhu klienta.
Prijima a zpracovava zpravy od klienta
a generuje odpovedi.
*/
void ServerThread::run()
{
    QTcpSocket tcpSocket;
    if (!tcpSocket.setSocketDescriptor(socketDescriptor)) {
        emit error(tcpSocket.error());
        return;
    }
    bool stop=false;
    tcpSocket.write("Hello, do you want to login or register?\n");
    while(!stop){
        if(tcpSocket.waitForReadyRead(5000) && tcpSocket.canReadLine()){
            QString input = tcpSocket.readLine().trimmed();
            if(input=="Bye."){
              stop=true;
            }
            QString output = processInput(input);
            tcpSocket.write(output.toAscii().data());
        }
    }
    tcpSocket.disconnectFromHost();
    tcpSocket.waitForDisconnected();
}

//! Vytvari odpoved.
/*!
Zpracovava zpravu od klienta a vytvrari odpoved na tuto zpravu.
\param input Zprava od klienta
\return Odpoved serveru
*/
QString ServerThread::processInput(QString input)
{
    QString output="Ok.\n";
    if(input=="Bye.")
      return "Bye.\n";
    switch(state){
    case WaitingForGreeting:
        if(input=="Login."){
            state=WaitingForLogin;
        }else if(input=="Register."){
            state=WaitingForName;
        }else{
            output="Error: Only login or register.\n";
        }
        break;
    case WaitingForLogin:
        user=input;
        state=WaitingForPassword;
        break;
    case WaitingForName:
        user=input;
        state=WaitingForKey;
        break;
    case WaitingForPassword:
        if(checkPassword(user,input)){
            state=ReadyForUse;
        }else{
            state=WaitingForGreeting;
            output="Error: Wrong login or password.\n";
        }
        break;
    case WaitingForKey:
        state=WaitingForGreeting;
        if(!addUser(user,input)){
            output="Error: Could not add user.\n";
        }
        break;
    case ReadyForUse:
        if(input=="Run simulation."){
            state=RunSimulation;
        }else if(input=="Save net."){
            state=SaveNet;
        }else if(input=="Step."){
            state=Step;
        }else if(input=="Load net."){
            state=LoadNet;
            output=getNets();
        }else{
            output="Error: Unknown action.\n";
        }
        break;
    case RunSimulation:
        state=ReadyForUse;
        output=runSimulation(input);
        break;
    case Step:
        state=ReadyForUse;
        output=step(input);
        break;
    case SaveNet:
        state=ReadyForUse;
        output=saveNet(input);
        break;
    case LoadNet:
        state=ReadyForUse;
        output=loadNet(input);
        break;

    }

    return output;
}

//! Overi klienta.
/*!
Overi login a heslo klienta.
\param user Login klienta
\param password Heslo klienta
\return true Pokud je kombinace spravna, jinak false.
*/
bool ServerThread::checkPassword(QString login, QString password)
{
    QDomDocument doc;
    QFile file(QCoreApplication::applicationDirPath()+"/users.xml");
    if(!file.open(QIODevice::ReadOnly)){
        return false;
    }
    if(!doc.setContent(&file)){
        file.close();
        return false;
    }
    file.close();

    QDomElement docElem = doc.documentElement();
    QDomNodeList userList=docElem.elementsByTagName("User");

    for(int i=0; i<userList.count(); i++){
        QDomElement user=userList.at(i).toElement();
        if(user.firstChildElement("login").text()==login){
            if(user.firstChildElement("password").text()==password){
                return true;
            }else{
                return false;
            }
        }

    }
    return false;
}

//! Prida klienta.
/*!
Prida klienta s danym login a heslem
do databaze klientu.
\param user Login klienta
\param password Heslo klienta
\return true Pokud se podarilo klienta pridat, jinak false.
*/
bool ServerThread::addUser(QString login, QString password)
{
    QDomDocument doc;
    QFile file(QCoreApplication::applicationDirPath()+"/users.xml");
    bool exists = file.exists();
    if(!file.open(QIODevice::ReadWrite)){
        return false;
    }
    if(exists){
        if(!doc.setContent(&file)){
            file.close();
            return false;
        }
    }else{
        QDomNode node=doc.createProcessingInstruction("xml",  QString("version=\"1.0\" encoding=\"utf-8\""));
        doc.insertBefore(node, doc.firstChild());
        doc.appendChild(doc.createElement("Users"));
    }


    QDomElement docElem = doc.documentElement();
    QDomNodeList userList=docElem.elementsByTagName("User");

    for(int i=0; i<userList.count(); i++){
        QDomElement user=userList.at(i).toElement();
        if(user.firstChildElement("login").text()==login){
            file.close();
            return false;
        }

    }

    QDomElement userElem=doc.createElement("User");
    QDomElement loginElem=doc.createElement("login");
    QDomText loginText=doc.createTextNode(login);
    QDomElement passwordElem=doc.createElement("password");
    QDomText passwordText=doc.createTextNode(password);
    docElem.appendChild(userElem);
    userElem.appendChild(loginElem);
    userElem.appendChild(passwordElem);
    loginElem.appendChild(loginText);
    passwordElem.appendChild(passwordText);

    file.resize(0);
    QTextStream out(&file);
    out << doc.toString();
    file.close();
    return true;
}

//! Odsimuluje sit.
/*!
Odsimuluje klientem poslanou sit a sit po simulaci
posle zpet klientovi.
\param input Sit v xml formatu
\return Odsimulovana sit v xml formatu.
*/
QString ServerThread::runSimulation(QString input)
{
    QDomDocument doc;
    if(!doc.setContent(input)){
        return "Error: Could not parse net\n";
    }
    Net net(doc.documentElement());
    if(!net.isValid()){
        return "Error: Net is not correct\n";
    }

    QString name = doc.documentElement().firstChildElement("netName").text();
    QFile file(QCoreApplication::applicationDirPath()+"/nets/"+name+".xml");
    if(file.exists()){
        QDomDocument document;
        if(!file.open(QIODevice::ReadWrite)){
            return "Error: Could not open net\n";
        }
        if(!document.setContent(&file)){
            file.close();
            return "Error: Could nor parse file\n";
        }
        QDomElement simulElem=document.createElement("Simulation");
        QDomElement userElem=document.createElement("user");
        QDomText userText=document.createTextNode(user);
        QDomElement dateElem=document.createElement("date");
        QDomText dateText=document.createTextNode(QDateTime::currentDateTime().toString());
        document.documentElement().appendChild(simulElem);
        simulElem.appendChild(userElem);
        simulElem.appendChild(dateElem);
        userElem.appendChild(userText);
        dateElem.appendChild(dateText);
        file.resize(0);
        QTextStream out(&file);
        out << document.toString();
        file.close();
    }

    int steps=doc.documentElement().firstChildElement("steps").text().toInt();
    net.run(steps);
    QDomDocument result;
    net.toXML(result,result);
    return result.toString(-1)+"\n";
}

//! Provede krok simulace site.
/*!
Provede krok simulace site poslane klientem
a vyslednou sit posle zpet klientovi.
\param input Sit v xml formatu
\return Odsimulovana sit v xml formatu.
*/
QString ServerThread::step(QString input)
{
    QDomDocument doc;
    if(!doc.setContent(input)){
        return "Error: Could not parse net\n";
    }
    Net net(doc.documentElement());
    if(!net.isValid()){
        return "Error: Net is not correct\n";
    }

    QString name = doc.documentElement().firstChildElement("netName").text();

    QFile file(QCoreApplication::applicationDirPath()+"/nets/"+name+".xml");
    if(file.exists()){
        QDomDocument document;
        if(!file.open(QIODevice::ReadWrite)){
            return "Error: Could not open net\n";
        }
        if(!document.setContent(&file)){
            file.close();
            return "Error: Could nor parse file\n";
        }
        QDomElement simulElem=document.createElement("Simulation");
        QDomElement userElem=document.createElement("user");
        QDomText userText=document.createTextNode(user);
        QDomElement dateElem=document.createElement("date");
        QDomText dateText=document.createTextNode(QDateTime::currentDateTime().toString());
        document.documentElement().appendChild(simulElem);
        simulElem.appendChild(userElem);
        simulElem.appendChild(dateElem);
        userElem.appendChild(userText);
        dateElem.appendChild(dateText);
        file.resize(0);
        QTextStream out(&file);
        out << document.toString();
        file.close();
    }

    int transition=doc.documentElement().firstChildElement("steps").text().toInt();
    net.step(transition);
    QDomDocument result;
    net.toXML(result,result);
    return result.toString(-1)+"\n";
}

//! Ulozi sit.
/*!
Ulozi sit poslanou klientem
do databaze siti.
\param input Sit v xml formatu
\return Retezec se stavem ulozeni
*/
QString ServerThread::saveNet(QString input)
{
    QDomDocument net;
    if(!net.setContent(input)){
        return "Error: Could not parse net\n";
    }

    QString name = net.documentElement().firstChildElement("netName").text();

    QDomDocument doc;
    QFile file(QCoreApplication::applicationDirPath()+"/nets/"+name+".xml");
    bool exists = file.exists();
    int version = 1;
    if(!file.open(QIODevice::ReadWrite)){
        return "Error: Could create output file\n";
    }
    if(exists){
        if(!doc.setContent(&file)){
            file.close();
            return "Error: Could not parse file\n";
        }
    }else{
        QDomNode node=doc.createProcessingInstruction("xml",  QString("version=\"1.0\" encoding=\"utf-8\""));
        doc.insertBefore(node, doc.firstChild());
        doc.appendChild(doc.createElement("Nets"));
    }

    QDomElement root = doc.documentElement();
    version += root.elementsByTagName("Petri-NET").size();

    QDomElement userElem=net.createElement("user");
    net.documentElement().appendChild(userElem);
    userElem.appendChild(net.createTextNode(user));

    QDomElement versionElem=net.createElement("version");
    net.documentElement().appendChild(versionElem);
    versionElem.appendChild(net.createTextNode(QString::number(version)));

    doc.documentElement().appendChild(net);

    file.resize(0);
    QTextStream out(&file);
    out << doc.toString();
    file.close();
    return "Ok.\n";
}

//! Vrati seznam siti.
/*!
Vytvori seznam siti, vcetne informaci o verzich
a simulacich.
\return Seznam siti v xml formatu
*/
QString ServerThread::getNets()
{
    QDir myDir(QCoreApplication::applicationDirPath()+"/nets");
    QStringList filters;
    filters << "*.xml";
    myDir.setNameFilters(filters);
    QStringList list = myDir.entryList();

    QDomDocument doc;
    QDomElement root = doc.createElement("Nets");
    doc.appendChild(root);

    for(int i=0;i<list.size();i++){
        QFile file(QCoreApplication::applicationDirPath()+"/nets/"+list.at(i));
        QDomDocument net;
        if(!file.open(QIODevice::ReadOnly)){
            return "Error: Could not open saved net\n";
        }
        if(!net.setContent(&file)){
            file.close();
            return "Error: Could not parse saved net\n";
        }
        QDomElement netElem=doc.createElement("Net");
        root.appendChild(netElem);
        QDomElement nameElem=doc.createElement("name");
        netElem.appendChild(nameElem);
        nameElem.appendChild(doc.createTextNode(list.at(i).mid(0,list.at(i).length()-4)));
        QDomNodeList netList=net.documentElement().elementsByTagName("Petri-NET");
        for(int j=0;j<netList.size();j++){
            QDomElement versionElem=doc.createElement("version");
            netElem.appendChild(versionElem);
            QDomElement userElem=doc.createElement("user");
            QDomElement idElem=doc.createElement("id");
            QDomElement descriptionElem=doc.createElement("description");
            versionElem.appendChild(userElem);
            versionElem.appendChild(idElem);
            versionElem.appendChild(descriptionElem);
            userElem.appendChild(doc.createTextNode(netList.at(j).firstChildElement("user").text()));
            idElem.appendChild(doc.createTextNode(netList.at(j).firstChildElement("version").text()));
            descriptionElem.appendChild(doc.createTextNode(netList.at(j).firstChildElement("description").text()));
        }
        QDomNodeList simulList=net.documentElement().elementsByTagName("Simulation");
        for(int j=0;j<simulList.size();j++){
            QDomElement simulElem=doc.createElement("Simulation");
            netElem.appendChild(simulElem);
            QDomElement userElem=doc.createElement("user");
            QDomElement dateElem=doc.createElement("date");
            simulElem.appendChild(userElem);
            simulElem.appendChild(dateElem);
            userElem.appendChild(doc.createTextNode(simulList.at(j).firstChildElement("user").text()));
            dateElem.appendChild(doc.createTextNode(simulList.at(j).firstChildElement("date").text()));
        }
        file.close();
    }
    return doc.toString(-1)+"\n";
}

//! Nacte sit.
/*!
Nacte sit pozadovanou klientem.
\param input Retezec s nazvem a verzi pozadovane site
\return Pozadovana sit v xml formatu
*/
QString ServerThread::loadNet(QString input)
{
    QDomDocument doc;
    if(!doc.setContent(input)){
        return "Error: Could not parse input\n";
    }
    QString name=doc.documentElement().firstChildElement("name").text();
    QString version=doc.documentElement().firstChildElement("version").text();

    QFile file(QCoreApplication::applicationDirPath()+"/nets/"+name+".xml");
    if(!file.open(QIODevice::ReadOnly)){
        return "Error: Could not open saved net\n";
    }
    if(!doc.setContent(&file)){
        file.close();
        return "Error: Could not parse saved net\n";
    }

    QDomNodeList netList=doc.documentElement().elementsByTagName("Petri-NET");
    for(int i=0;i<netList.size();i++){
        if(netList.at(i).firstChildElement("version").text()==version){
            QString str;
            QTextStream stream(&str);
            netList.at(i).save(stream,-1);
            return str+"\n";
        }
    }
    return "Error: Version not found\n";
}


