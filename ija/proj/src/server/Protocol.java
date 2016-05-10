package server;

import java.net.*;
import java.io.*;
import java.util.Date;

import petrinet.*;
import client.*;


import org.dom4j.Document;
import org.dom4j.DocumentFactory;
import org.dom4j.io.OutputFormat;
import org.dom4j.DocumentHelper;
import org.dom4j.io.XMLWriter;
import java.io.File;
import java.io.FileOutputStream;
import java.io.IOException;
import org.dom4j.io.SAXReader;
import org.dom4j.DocumentException;
import org.dom4j.Element;
import org.dom4j.Document;
import java.util.Iterator;

/**
 * Trida Protocol slouzi pro zpracovani pozadavku klienta,
 * uchovava si soucasny stav komunikace a generuje reakci na
 * prichozi zpravy.
 * @author Vojtech Bartl - xbartl03, Pavel Fryz - xfryzp00
 */
public class Protocol {
  private static final int WAITING = 0;
  private static final int SENTLOGORREG = 1;
  private static final int ASKFORPASSWORD = 2;
  private static final int ASKFORACTION = 3;
  private static final int RUN = 4;
  private static final int LOADNET = 5;
  private static final int SAVENET = 6;
  private static final int LOGIN = 7;
  private static final int REGISTER = 8;
  private static final int ASKFORREGISTER = 9;
  private static final int STEP = 10;
  /** stav komunikace */
  private int state = WAITING;
  /** login klieta */
  private String login = "";
  /** heslo klienta */
  private String password = "";

  /**
   * Zpracuje vstup klienta
   * @param theInput vstup klienta
   * @return Odpoved na vstup
   */
  public String processInput(String theInput) {
    String theOutput = null;
    if("Bye.".equals(theInput))
      return "Bye.";
    switch(state){
      case WAITING:
        theOutput = "Hello, do you want to login or register?";
        state = SENTLOGORREG;
        break;
      case SENTLOGORREG:
        if(theInput.equals("Login.")){
          state = LOGIN;
        }else if(theInput.equals("Register.")){
          state = REGISTER;
        }
        theOutput = "OK";
        break;
      case LOGIN:
        login = theInput;
        theOutput = "Tell me secret.";
        state = ASKFORPASSWORD;
        break;
      case ASKFORPASSWORD:
        password = theInput;
        /* autentizuje klienta */
        if(checkPassword(login,password)){
          theOutput = "What do you want?";
          state = ASKFORACTION;
        }else{
          theOutput = "It is not secret.";
          state = SENTLOGORREG;
        }
        break;
      case ASKFORACTION:
        theOutput = "Ok.";
        if (theInput.equals("Save net."))
          state = SAVENET;
        else if(theInput.equals("Run simulation."))
          state = RUN;
        else if(theInput.equals("Load net.")){
          state = LOADNET;
          /* odesle seznam siti */
          theOutput = getNets();
          if(theOutput.equals("Error"))
            state = ASKFORACTION;
        }
        else if(theInput.equals("Step.")){
          state = STEP;
        }else
          theOutput = "Wrong action.";
        break;
      case SAVENET:
        /* ulozi prijatou sit */
        theOutput = saveNet(theInput);
        state = ASKFORACTION;
        break;
      case RUN:
        /* simuluje prijatou sit */
        theOutput = runSimulation(theInput);
        state = ASKFORACTION;
        break;
      case STEP:
        /* simuluje prijatou sit */
        theOutput = stepSimulation(theInput);
        state = ASKFORACTION;
        break;
      case LOADNET:
        /* odesle pozadovanou sit */
        theOutput = loadNet(theInput);
        state = ASKFORACTION;
        break;
      case REGISTER:
        login=theInput;
        theOutput="Tell me secret";
        state=ASKFORREGISTER;
        break;
      case ASKFORREGISTER:
        password=theInput;
        theOutput=addUser(login,password);
        break;
    }
    return theOutput;
  }

  /**
   * Prida uzivatel
   * @param login Login uzivatele
   * @param password Heslo uzivatele
   * @return Odpoved na pridani uzivatele
   */
  private String addUser(String login, String password){
    try{
      Document document = null;
      File file = new File("users.xml");
      Element root = null;
      if(file.exists()){
        SAXReader reader=new SAXReader();
        document=reader.read(file);
        root = document.getRootElement();
      }else{
        document=DocumentFactory.getInstance().createDocument();
        root=document.addElement("Users");
      }

      Iterator i=root.elementIterator("User");
      while(i.hasNext()){
        Element user = (Element)i.next();
        if(user.elementText("login").equals(login)){
          return "Defined.";
        }
      }
      Element user = root.addElement("User");
      user.addElement("login").addText(login);
      user.addElement("password").addText(password);

      FileOutputStream fos=new FileOutputStream(file);
      OutputFormat format=OutputFormat.createPrettyPrint();
      XMLWriter writer=new XMLWriter(fos, format);
      writer.write(document);
      writer.flush();
    }catch(Exception e){return "Defined.";}
    return "OK";
  }

  /**
   * Autentizuje uzivatele
   * @param login Login uzivatele
   * @param password Heslo uzivatele
   * @return Stav autentizace
   */
  private boolean checkPassword(String login,String password){
    try{
      SAXReader reader=new SAXReader();
      Document document = reader.read(new File("users.xml"));
      Element root = document.getRootElement();
      Iterator i=root.elementIterator("User");
      while(i.hasNext()){
        Element user = (Element)i.next();
        if(user.elementText("login").equals(login)){
          if(user.elementText("password").equals(password))
            return true;
          else
            return false;
        }
      }
    }catch(Exception e){}
    return false;
  }

  private String getNets(){
    try{
      Document document=DocumentFactory.getInstance().createDocument();
      Element root=document.addElement("Nets");
      File folder = new File("nets");
      File[] listOfFiles = folder.listFiles();

      for (int i = 0; i < listOfFiles.length; i++)
      {
        SAXReader reader=new SAXReader();
        Document net=reader.read(listOfFiles[i]);
        Element netel = root.addElement("Net");
        netel.addElement("name").addText(listOfFiles[i].getName().replaceAll("\\.xml$",""));
        Iterator it=net.getRootElement().elementIterator("GNet");
        while(it.hasNext()){
          Element version = (Element)it.next();
          Element ver = netel.addElement("version");
          ver.addElement("user").addText(version.elementText("user"));
          ver.addElement("id").addText(version.elementText("version"));
          ver.addElement("description").addText(version.elementText("description"));
        }
        it=net.getRootElement().elementIterator("Simulation");
        while(it.hasNext()){
          Element simul = (Element)it.next();
          Element sim=netel.addElement("Simulation");
          sim.addElement("user").addText(simul.elementText("user"));
          sim.addElement("date").addText(simul.elementText("date"));
        }
      }

      ByteArrayOutputStream baos=new ByteArrayOutputStream();
      OutputFormat format=OutputFormat.createCompactFormat();
      format.setSuppressDeclaration(true);
      XMLWriter writer=new XMLWriter(baos, format);
      writer.write(document);
      writer.flush();
      return baos.toString();
    }catch(Exception e){}
    return "Error";
  }

  /**
   * Ulozi sit na server
   * @param net Sit
   * @param Ospoved na ulozeni
   */
  private String saveNet(String net){
    try{
      Document document=DocumentHelper.parseText(net);
      Element root = document.getRootElement();
      String name = root.elementText("name");
      File file = new File("nets" + File.separator + name + ".xml");
      Element netr=null;
      int version = 1;
      Document netdoc=null;
      if(file.exists()){
        SAXReader reader=new SAXReader();
        netdoc=reader.read(file);
        netr = netdoc.getRootElement();
        version+=netr.elements("GNet").size();
      }else{
        netdoc=DocumentFactory.getInstance().createDocument();
        netr=netdoc.addElement("PetriNet");
      }
      root.addElement("user").addText(login);
      root.addElement("version").addText(String.valueOf(version));
      netr.add(root);
      FileOutputStream fos=new FileOutputStream(file);
      OutputFormat format=OutputFormat.createPrettyPrint();
      XMLWriter writer=new XMLWriter(fos, format);
      writer.write(netdoc);
      writer.flush();
    }catch(Exception e){return "Error";}
    return "Saved";
  }

  /**
   * Vrati pozadovanou sit
   * @param netid Id site
   * @return Sit
   */
  private String loadNet(String netid){
    try{
      Document document=DocumentHelper.parseText(netid);
      Element root = document.getRootElement();
      String name = root.elementText("name");
      String version = root.elementText("version");
      SAXReader reader=new SAXReader();
      document = reader.read(new File("nets" + File.separator + name +".xml"));
      root = document.getRootElement();
      Iterator i=root.elementIterator("GNet");
      while(i.hasNext()){
        Element net = (Element)i.next();
        if(net.elementText("version").equals(version)){
          ByteArrayOutputStream baos=new ByteArrayOutputStream();
          OutputFormat format=OutputFormat.createCompactFormat();
          format.setSuppressDeclaration(true);
          XMLWriter writer=new XMLWriter(baos, format);
          writer.write(net);
          writer.flush();
          return baos.toString();
        }
      }
    }catch(Exception e){}
    return "Error";
  }

  /**
   * Provede simulaci site
   * @param net Sit
   * @return Odsimulovana sit
   */
  private String runSimulation(String net){
    try{
      Document document=DocumentHelper.parseText(net);
      String name=document.getRootElement().elementText("name");
      Element root = document.getRootElement();
      int steps = Integer.parseInt(root.elementText("steps"));
      GNet gnet = new GNet(root);
      gnet.run(steps);
      document=DocumentFactory.getInstance().createDocument();
      gnet.toXML(document);
      ByteArrayOutputStream baos=new ByteArrayOutputStream();
      OutputFormat format=OutputFormat.createCompactFormat();
      format.setSuppressDeclaration(true);
      XMLWriter writer=new XMLWriter(baos, format);
      writer.write(document);
      writer.flush();

      File file = new File("nets" + File.separator + name + ".xml");
      if(file.exists()&&steps>0){
        SAXReader reader=new SAXReader();
        document=reader.read(file);
        Element sim=document.getRootElement().addElement("Simulation");
        sim.addElement("user").addText(login);
        sim.addElement("date").addText(new Date().toString());
        FileOutputStream fos=new FileOutputStream(file);
        format=OutputFormat.createPrettyPrint();
        writer=new XMLWriter(fos, format);
        writer.write(document);
        writer.flush();
      }

      return baos.toString();
    }catch(ExprException e){
      return e.getError();
    }
    catch(Exception e){}
    return "Error.";
  }

  /**
   * Provede krok simulace site
   * @param net Sit
   * @return Odsimulovana sit
   */
  private String stepSimulation(String net){
    try{
      Document document=DocumentHelper.parseText(net);
      String name=document.getRootElement().elementText("name");
      Element root = document.getRootElement();
      int id = Integer.parseInt(root.elementText("step"));
      GNet gnet = new GNet(root);
      gnet.step(gnet.getTransition(id));
      document=DocumentFactory.getInstance().createDocument();
      gnet.toXML(document);
      ByteArrayOutputStream baos=new ByteArrayOutputStream();
      OutputFormat format=OutputFormat.createCompactFormat();
      format.setSuppressDeclaration(true);
      XMLWriter writer=new XMLWriter(baos, format);
      writer.write(document);
      writer.flush();

      File file = new File("nets" + File.separator + name + ".xml");
      if(file.exists()){
        SAXReader reader=new SAXReader();
        document=reader.read(file);
        Element sim=document.getRootElement().addElement("Simulation");
        sim.addElement("user").addText(login);
        sim.addElement("date").addText(new Date().toString());
        FileOutputStream fos=new FileOutputStream(file);
        format=OutputFormat.createPrettyPrint();
        writer=new XMLWriter(fos, format);
        writer.write(document);
        writer.flush();
      }

      return baos.toString();
    }catch(ExprException e){
      return e.getError();
    }
    catch(Exception e){}
    return "Error.";
  }

}
