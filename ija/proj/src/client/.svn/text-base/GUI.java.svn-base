package client;

import petrinet.*;

import javax.swing.*;
import java.awt.*;
import java.awt.event.*;
import java.util.*;
import java.awt.geom.Line2D;
import java.io.File;
import javax.swing.event.*;
import java.net.*;
import java.io.*;

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
 * Trida implementujici GUI.
 * @author Vojtech Bartl - xbartl03, Pavel Fryz - xfryzp00
 */
public class GUI{
  /** Tlacitko transition */
  private JButton btnTrans = new JButton("Transition");
  /** Tlacitko place */
  private JButton btnPlace = new JButton("Place");
  /** Tlacitko remove */
  private JButton btnRemove = new JButton("Remove");
  /** Tlacitko move */
  private JButton btnMove = new JButton("Move");
  /** Tlacitko run */
  private JButton btnRun = new JButton("Run");
  /** Hlavni okno */
  private JFrame window = new JFrame("Petri-NET Simulator");
  /** Okno s napovedou */
  private JFrame helpW = new JFrame("Help");
  /** Lista se stavem */
  private JLabel status = new JLabel("Disconnected");
  /** Polozka menu start */
  private JMenuItem startMI = new JMenuItem("Start");
  /** Polozka menu stop */
  private JMenuItem stopMI = new JMenuItem("Stop");
  /** Polozka menu new */
  private JMenuItem newMI = new JMenuItem("New");
  /** Polozka menu open */
  private JMenuItem openMI = new JMenuItem("Open");
  /** Polozka menu open from server */
  private JMenuItem openFromServerMI = new JMenuItem("Open from server");
  /** Polozka menu save */
  private JMenuItem saveMI = new JMenuItem("Save");
  /** Polozka menu save to server */
  private JMenuItem saveToServerMI = new JMenuItem("Save to server");
  /** Polozka menu exit */
  private JMenuItem exitMI = new JMenuItem("Exit");
  /** Polozka menu set properties */
  private JMenuItem setPropertiesMI = new JMenuItem("Set properties");
  /** Polozka menu clear */
  private JMenuItem clearMI = new JMenuItem("Clear");
  /** Polozka menu register */
  private JMenuItem registerMI = new JMenuItem("Register");
  /** Polozka menu login */
  private JMenuItem loginMI = new JMenuItem("Login");
  /** Polozka menu help */
  private JMenuItem helpMI = new JMenuItem("Help");
  /** Polozka menu show tokens */
  private JCheckBoxMenuItem showTokenMI = new JCheckBoxMenuItem("Show tokens", false);
  /** Panel s tlacitky pro editaci */
  private JPanel toolEdit = new JPanel(new FlowLayout(FlowLayout.LEFT));
  /** Panel s tlacitky pro simulaci */
  private JPanel toolSimul = new JPanel(new FlowLayout(FlowLayout.LEFT));
  /** Pole se vsemi okny aplikace */
  private static ArrayList<GUI> gui=new ArrayList<GUI>();
  /** Panel okna s reprezentaci site */
  private GPanel net;
  /** XML element s nactenymi styly */
  private Element styles;
  /** Promena se soucasnym stylem */
  private static String curStyle="default";
  /** Promena se stavem jestli se maji zobrazovat tokeny */
  private static boolean showToken = false;
  /** Socket pripojeny k serveru */
  private Socket socket=new Socket();
  /** Login prihlaseneho uzivatele */
  private static String login;
  /** Adrese serveru */
  private static String host;
  /** Heslo prihlaseneho uzivatele */
  private static String password;
  /** Port serveru */
  private static int port;

  /** Sirka okna */
  private final int FRAME_W=800;
  /** Vyska okna */
  private final int FRAME_H=500;
  /** Soucasny mod */
  private int mode = GPanel.ADD_PLACE;


  /**
  * Konstruktor inicializuje promenne
  */
  public GUI(){
    window.setLayout(new BorderLayout());
    window.setBounds(550, 100, FRAME_W, FRAME_H);
    window.setJMenuBar(createMenu());

    createButtons();
    toolEdit.add(btnPlace);
    toolEdit.add(btnTrans);
    toolEdit.add(btnRemove);
    toolEdit.add(btnMove);
    toolSimul.add(btnRun);

    JPanel toolbar=new JPanel(new CardLayout());
    toolbar.add(toolEdit, "");
    toolbar.add(toolSimul, "");
    toolSimul.setVisible(false);
    window.add(toolbar,BorderLayout.NORTH);

    net=new GPanel();
    gui.add(this);
    window.add(net,BorderLayout.CENTER);

    window.add(status,BorderLayout.SOUTH);

    if(curStyle=="default"){
      SAXReader reader=new SAXReader();
      Document document = null;
      try{
        document=reader.read(new File("config.xml"));
        changeStyle(document.getRootElement().elementText("Style"));
        if(document.getRootElement().elementText("ShowToken").equals("true")){
          GPlace.showToken=true;
          showToken=true;
        }else{
          GPlace.showToken=false;
          showToken=false;
        }
      }
      catch(DocumentException de){}
    }else{changeStyle(curStyle);}

    showTokenMI.setState(showToken);
    window.setResizable(false);
    window.setDefaultCloseOperation(JFrame.DO_NOTHING_ON_CLOSE);
    window.addWindowListener(new WindowAdapter() {
      public void windowClosing(WindowEvent evt) {
        gui.remove(this);
        window.dispose();
      }
    });
    window.setLocationRelativeTo(null);
    window.setVisible(true);
    
    helpW.setBounds(550, 100, FRAME_W, FRAME_H);
    helpW.setResizable(false);
    helpW.setDefaultCloseOperation(JFrame.DISPOSE_ON_CLOSE);
    JTextArea helpArea=new JTextArea(helpText);
    helpArea.setEditable(false);
    helpW.add(helpArea);
  }

  /**
  * Funkce pro vytvoreni a vykresleni GUI
  */
  public static void createAndShowGUI(){
    new GUI();
  }

  /**
  * Funkce pro vytvoreni menu
  */
  protected JMenuBar createMenu(){
    JMenuBar menu;
    JMenu file, server, edit, style, simulation, help;
    menu=new JMenuBar();
    file=new JMenu("File");
    edit=new JMenu("Edit");
    server=new JMenu("Server");
    simulation=new JMenu("Simulation");
    style=new JMenu("Style");
    help=new JMenu("Help");
    menu.add(file);
    menu.add(edit);
    menu.add(server);
    menu.add(simulation);
    menu.add(style);
    menu.add(help);

    menuActions action = new menuActions();
    startMI.addActionListener(action);
    stopMI.addActionListener(action);
    newMI.addActionListener(action);
    openMI.addActionListener(action);
    openFromServerMI.addActionListener(action);
    saveMI.addActionListener(action);
    saveToServerMI.addActionListener(action);
    exitMI.addActionListener(action);
    clearMI.addActionListener(action);
    registerMI.addActionListener(action);
    loginMI.addActionListener(action);
    setPropertiesMI.addActionListener(action);
    showTokenMI.addActionListener(action);
    helpMI.addActionListener(action);

    startMI.setEnabled(false);
    stopMI.setEnabled(false);
    openFromServerMI.setEnabled(false);
    saveToServerMI.setEnabled(false);

    file.add(newMI);
    file.addSeparator();
    file.add(openMI);
    file.add(openFromServerMI);
    file.addSeparator();
    file.add(saveMI);
    file.add(saveToServerMI);
    file.addSeparator();
    file.add(exitMI);
    edit.add(setPropertiesMI);
    edit.add(clearMI);
    server.add(loginMI);
    server.add(registerMI);
    simulation.add(startMI);
    simulation.add(stopMI);
    style.add(showTokenMI);
    help.add(helpMI);

    try{
      SAXReader reader=new SAXReader();
      Document document = reader.read(new File("styles.xml"));
      styles = document.getRootElement();
      Iterator i=styles.elementIterator("Style");
      style.addSeparator();
      while(i.hasNext()){
        JMenuItem styleMI=new JMenuItem(((Element)i.next()).elementText("name"));
        styleMI.addActionListener(action);
        style.add(styleMI);
      }
    }catch(Exception e){}

    return menu;
  }


  /**
  * Funkce pro vytvoreni tlacitek pro nabidku
  */
  protected void createButtons(){
    buttonPress press = new buttonPress();
    btnTrans.setBackground(Color.white);
    btnPlace.setBackground(Color.white);
    btnRemove.setBackground(Color.white);
    btnMove.setBackground(Color.white);
    btnRun.setBackground(Color.white);

    btnPlace.setEnabled(false);
    btnTrans.addActionListener(press);
    btnPlace.addActionListener(press);
    btnRemove.addActionListener(press);
    btnMove.addActionListener(press);
    btnRun.addActionListener(press);
  }




  /**
  * Trida obsluhujici akce v menu
  */
  public class menuActions implements ActionListener{
    public void actionPerformed(ActionEvent e){
      if(e.getSource() == openMI){
        JFileChooser fc=new JFileChooser("nets");
        fc.showOpenDialog(window);
        File openFile=fc.getSelectedFile();
        if(openFile!=null)
          net.open(openFile);
        toolEdit.setVisible(true);
        toolSimul.setVisible(false);
        if(startMI.isEnabled()||stopMI.isEnabled()){
          startMI.setEnabled(true);
          stopMI.setEnabled(false);
        }
        if(!btnMove.isEnabled()) net.setMode(GPanel.MOVE);
        else if(!btnRemove.isEnabled()) net.setMode(GPanel.REMOVE);
        else if(!btnPlace.isEnabled())net.setMode(GPanel.PLACE);
        else net.setMode(GPanel.TRANSITION);
        window.repaint();
      }else if(e.getSource()== saveMI){
        JFileChooser fc=new JFileChooser("nets");
        fc.showSaveDialog(window);
        File saveFile=fc.getSelectedFile();
        if(saveFile!=null)
        net.save(saveFile);
      }else if(e.getSource()==exitMI){
        if(socket.isConnected()){
          try{
            PrintWriter out = new PrintWriter(socket.getOutputStream(), true);
            out.println("Bye.");
            socket.close();
          }catch(Exception ee){}
        }
        window.dispose();
      }else if(e.getSource()==clearMI){
        net.clear();
      }else if(e.getSource()==helpMI){      ///////////////////////////////////
        helpW.setVisible(true);
      }else if(e.getSource()==newMI){
        javax.swing.SwingUtilities.invokeLater(new Runnable() {
            public void run() {
                createAndShowGUI();
            }
        });
      }else if(e.getSource()==startMI){
        try{
          PrintWriter out = new PrintWriter(socket.getOutputStream(), true);
          BufferedReader in = new BufferedReader(new InputStreamReader(socket.getInputStream()));
          out.println("Run simulation.");
          in.readLine();
          Document document=DocumentFactory.getInstance().createDocument();
          net.toXML(document);
          document.getRootElement().addElement("steps").addText("0");
          ByteArrayOutputStream baos=new ByteArrayOutputStream();
          OutputFormat format=OutputFormat.createCompactFormat();
          format.setSuppressDeclaration(true);
          XMLWriter writer=new XMLWriter(baos, format);
          writer.write(document);
          writer.flush();
          out.println(baos.toString());
          String input = in.readLine();
          if(input.matches("^Error.*")){
            JOptionPane.showMessageDialog(window, input, "Error", JOptionPane.ERROR_MESSAGE);
            return;
          }
          toolEdit.setVisible(false);
          toolSimul.setVisible(true);
          startMI.setEnabled(false);
          stopMI.setEnabled(true);
          net.setMode(GPanel.SIMULATION);
          document=DocumentHelper.parseText(input);
          net.fromXML(document.getRootElement());
          net.setMode(GPanel.SIMULATION);
        }
        catch(Exception eee){}
        window.repaint();
      }else if(e.getSource()==stopMI){
        toolEdit.setVisible(true);
        toolSimul.setVisible(false);
        startMI.setEnabled(true);
        stopMI.setEnabled(false);
        if(!btnMove.isEnabled()) net.setMode(GPanel.MOVE);
        else if(!btnRemove.isEnabled()) net.setMode(GPanel.REMOVE);
        else if(!btnPlace.isEnabled())net.setMode(GPanel.PLACE);
        else net.setMode(GPanel.TRANSITION);
        window.repaint();
      }else if(e.getSource()==openFromServerMI){
        try{
          PrintWriter out = new PrintWriter(socket.getOutputStream(), true);
          BufferedReader in = new BufferedReader(new InputStreamReader(socket.getInputStream()));
          out.println("Load net.");
          String input = in.readLine();
          if(input.equals("Error")){
            JOptionPane.showMessageDialog(window, "Error during loading nets", "Error", JOptionPane.ERROR_MESSAGE);
            return;
          }else{
            LoadDialog d = new LoadDialog(window,input);
            out.println(d.output());
            input = in.readLine();
            if(input.equals("Error")){
              if(!d.output().equals("None"))
                JOptionPane.showMessageDialog(window, "Could not load net", "Error", JOptionPane.ERROR_MESSAGE);
              return;
            }
            try{
              Document document=DocumentHelper.parseText(input);
              net.fromXML(document.getRootElement());
            }catch(Exception exce){}
            toolEdit.setVisible(true);
            toolSimul.setVisible(false);
            startMI.setEnabled(true);
            stopMI.setEnabled(false);
            if(!btnMove.isEnabled()) net.setMode(GPanel.MOVE);
            else if(!btnRemove.isEnabled()) net.setMode(GPanel.REMOVE);
            else if(!btnPlace.isEnabled())net.setMode(GPanel.PLACE);
            else net.setMode(GPanel.TRANSITION);
            window.repaint();
          }
        }catch(Exception ioe){}
      }else if(e.getSource()==saveToServerMI){
        if("".equals(net.getName())){
          JOptionPane.showMessageDialog(window, "Unnamed net(menu edit->set properties)", "Error", JOptionPane.WARNING_MESSAGE);
          return;
        }
        try{
          PrintWriter out = new PrintWriter(socket.getOutputStream(), true);
          BufferedReader in = new BufferedReader(new InputStreamReader(socket.getInputStream()));
          out.println("Save net.");
          in.readLine();
          out.println(net.toCompactXML());
          if(in.readLine().equals("Saved"))
            JOptionPane.showMessageDialog(window, "Net saved", "Saved", JOptionPane.INFORMATION_MESSAGE);
          else
            JOptionPane.showMessageDialog(window, "Error during saving the net", "Error", JOptionPane.WARNING_MESSAGE);
        }catch(Exception ioe){}
      }else if(e.getSource()==setPropertiesMI){
        propertyDialog();
      }else if(e.getSource()==loginMI){
        ConnectDialog d=new ConnectDialog(window,"Login.",status);
        if(d.getStatus()){
          login = d.getLogin();
          password = d.getPassword();
          host = d.getHost();
          port = d.getPort();
          try{
            for(GUI g:gui) {
              PrintWriter out = null;
              BufferedReader in = null;
              g.socket = new Socket(host,port);
              g.net.setSocket(g.socket);
              out = new PrintWriter(g.socket.getOutputStream(), true);
              in = new BufferedReader(new InputStreamReader(g.socket.getInputStream()));
              in.readLine();
              out.println("Login.");
              in.readLine();
              out.println(login);
              in.readLine();
              out.println(password);
              in.readLine();
            }
          }catch(IOException ioe){}
        }
        if(status.getText().equals("Connected")){
           for(GUI g:gui) g.startMI.setEnabled(true);
           for(GUI g:gui) g.openFromServerMI.setEnabled(true);
           for(GUI g:gui) g.saveToServerMI.setEnabled(true);
        }
      }else if(e.getSource()==registerMI){
        new ConnectDialog(window,"Register.",status);
      }else if(e.getSource()==showTokenMI){
        showToken=((JCheckBoxMenuItem)(e.getSource())).getState();
        for(GUI g:gui){
          g.showTokenMI.setState(showToken);
          g.net.updateGraphics();
          GPlace.showToken = showToken;
        }
        Document document=DocumentFactory.getInstance().createDocument();
        Element config=document.addElement("Configuration");
        config.addElement("Style").addText(curStyle);
        if(showToken) config.addElement("ShowToken").addText("true");
        else config.addElement("ShowToken").addText("false");
        try{
          FileOutputStream fos=new FileOutputStream(new File("config.xml"));
          OutputFormat format=OutputFormat.createPrettyPrint();
          XMLWriter writer=new XMLWriter(fos, format);
          writer.write(document);
          writer.flush();
        }
        catch(IOException ex){}
      }else{
        curStyle=((JMenuItem)(e.getSource())).getText();
        changeStyle(curStyle);
        for(GUI g:gui) g.net.updateGraphics();
        Document document=DocumentFactory.getInstance().createDocument();
        Element config=document.addElement("Configuration");
        config.addElement("Style").addText(curStyle);
        if(showToken) config.addElement("ShowToken").addText("true");
        else config.addElement("ShowToken").addText("false");
        try{
          FileOutputStream fos=new FileOutputStream(new File("config.xml"));
          OutputFormat format=OutputFormat.createPrettyPrint();
          XMLWriter writer=new XMLWriter(fos, format);
          writer.write(document);
          writer.flush();
        }
        catch(IOException ex){}
      }
    }
  }

  /**
   * Funkce pro zobrazeni okna pro editaci vlastnosti site
   */
  public void propertyDialog(){
    class PropertyDialog extends JDialog implements ActionListener
    {
      JTextField name = new JTextField(20);
      JTextField description = new JTextField(20);
      JButton btnOK = new JButton("Ok");
      public PropertyDialog(Component parent)
      {
        setModal(true);
        setTitle("Properties");

        name.setText(net.getName());
        description.setText(net.getDescription());

        getContentPane().setLayout(new BorderLayout());
        setDefaultCloseOperation(DISPOSE_ON_CLOSE);

        JPanel top = new JPanel();
        top.setLayout(new BorderLayout());
        top.add(new JLabel("Name:"),BorderLayout.NORTH);
        top.add(name,BorderLayout.CENTER);

        JPanel center = new JPanel();
        center.setLayout(new BorderLayout());
        center.add(new JLabel("Description"),BorderLayout.NORTH);
        center.add(description,BorderLayout.CENTER);

        getContentPane().add(top,BorderLayout.NORTH);
        getContentPane().add(center,BorderLayout.CENTER);
        getContentPane().add(btnOK,BorderLayout.SOUTH);

        btnOK.addActionListener(this);
        setResizable(false);
        pack();
        setLocationRelativeTo(parent);
        setVisible(true);
      }
      public void actionPerformed(ActionEvent ae)
      {
        net.setName(name.getText());
        net.setDescription(description.getText());
        dispose();
      }
    }
    new PropertyDialog(window);
  }

  /**
  * Trida obsluhujici stisk tlacitek
  */
  public class buttonPress implements ActionListener{
    public void actionPerformed(ActionEvent e){
      if((((JButton)(e.getSource())).getText()).equals("Transition")){
        clearButtons();
        net.setMode(GPanel.ADD_TRANSITION);
        btnTrans.setEnabled(false);
      }
      else if((((JButton)(e.getSource())).getText()).equals("Place")){
        clearButtons();
        net.setMode(GPanel.ADD_PLACE);
        btnPlace.setEnabled(false);
      }
      else if((((JButton)(e.getSource())).getText()).equals("Remove")){
        clearButtons();
        net.setMode(GPanel.REMOVE);
        btnRemove.setEnabled(false);
      }
      else if((((JButton)(e.getSource())).getText()).equals("Move")){
        clearButtons();
        net.setMode(GPanel.MOVE);
        btnMove.setEnabled(false);
      }else if(e.getSource()==btnRun){
        String label = JOptionPane.showInputDialog(window, "Max steps","Run",JOptionPane.PLAIN_MESSAGE);
        if(label==null) return;
        int steps = 0;
        try{
          steps=Integer.parseInt(label);
        }catch(NumberFormatException nfe){
          JOptionPane.showMessageDialog(window, nfe.getMessage(), "Wrong limit", JOptionPane.ERROR_MESSAGE);
          return;
        }
        if(steps<=0){
          JOptionPane.showMessageDialog(window, "Limit must be > 0", "Wrong limit", JOptionPane.ERROR_MESSAGE);
          return;
        }
        try{
          PrintWriter out = new PrintWriter(socket.getOutputStream(), true);
          BufferedReader in = new BufferedReader(new InputStreamReader(socket.getInputStream()));
          out.println("Run simulation.");
          in.readLine();
          Document document=DocumentFactory.getInstance().createDocument();
          net.toXML(document);
          document.getRootElement().addElement("steps").addText(label);
          ByteArrayOutputStream baos=new ByteArrayOutputStream();
          OutputFormat format=OutputFormat.createCompactFormat();
          format.setSuppressDeclaration(true);
          XMLWriter writer=new XMLWriter(baos, format);
          writer.write(document);
          writer.flush();
          out.println(baos.toString());
          String input = in.readLine();
          if(input.matches("^Error.*")){
            JOptionPane.showMessageDialog(window, input, "Error", JOptionPane.ERROR_MESSAGE);
            return;
          }
          document=DocumentHelper.parseText(input);
          net.fromXML(document.getRootElement());
          net.setMode(GPanel.SIMULATION);
        }
        catch(Exception eee){}
      }
    }
  }

  /**
   * Funkce pro zmenu stylu
   * @param styleName Nazev nastavovaneho stylu
   */
  private void changeStyle(String styleName){
    Iterator i=styles.elementIterator("Style");
    while(i.hasNext()){
      Element style = (Element)i.next();
      if(style.elementText("name").equals(styleName)){
        Element color = style.element("arclinecolor");
        GArc.lineColor = new Color(Integer.parseInt(color.elementText("red")),
                                   Integer.parseInt(color.elementText("green")),
                                   Integer.parseInt(color.elementText("blue")));
        color = style.element("arcfillcolor");
        GArc.fillColor = new Color(Integer.parseInt(color.elementText("red")),
                                   Integer.parseInt(color.elementText("green")),
                                   Integer.parseInt(color.elementText("blue")));
        color = style.element("arcselectcolor");
        GArc.selectColor = new Color(Integer.parseInt(color.elementText("red")),
                                     Integer.parseInt(color.elementText("green")),
                                     Integer.parseInt(color.elementText("blue")));
        color = style.element("placelinecolor");
        GPlace.lineColor = new Color(Integer.parseInt(color.elementText("red")),
                                     Integer.parseInt(color.elementText("green")),
                                     Integer.parseInt(color.elementText("blue")));
        color = style.element("placefillcolor");
        GPlace.fillColor = new Color(Integer.parseInt(color.elementText("red")),
                                     Integer.parseInt(color.elementText("green")),
                                     Integer.parseInt(color.elementText("blue")));
        color = style.element("placeselectcolor");
        GPlace.selectColor = new Color(Integer.parseInt(color.elementText("red")),
                                       Integer.parseInt(color.elementText("green")),
                                       Integer.parseInt(color.elementText("blue")));
        color = style.element("transitionlinecolor");
        GTransition.lineColor = new Color(Integer.parseInt(color.elementText("red")),
                                   Integer.parseInt(color.elementText("green")),
                                   Integer.parseInt(color.elementText("blue")));
        color = style.element("transitionfillcolor");
        GTransition.fillColor = new Color(Integer.parseInt(color.elementText("red")),
                                          Integer.parseInt(color.elementText("green")),
                                          Integer.parseInt(color.elementText("blue")));
        color = style.element("transitionselectcolor");
        GTransition.selectColor = new Color(Integer.parseInt(color.elementText("red")),
                                             Integer.parseInt(color.elementText("green")),
                                             Integer.parseInt(color.elementText("blue")));
        GArc.ENDPOINT = Integer.parseInt(style.elementText("arcendpoint"));
        GArc.MOVEPOINT = Integer.parseInt(style.elementText("arcmovepoint"));
        GPlace.RADIUS = Integer.parseInt(style.elementText("placeradius"));
        GTransition.WIDTH = Integer.parseInt(style.elementText("transitionwidth"));
        GTransition.HEIGHT = Integer.parseInt(style.elementText("transitionheight"));

        Font f = new Font(style.elementText("font"),Font.BOLD,Integer.parseInt(style.elementText("fontsize")));
        btnPlace.setFont(f);
        btnTrans.setFont(f);
        btnRemove.setFont(f);
        btnMove.setFont(f);
        btnRun.setFont(f);

        net.updateGraphics();
        return;
      }
    }
  }


  /**
  * Funkce nastavi barvu vsech tlacitek na puvodni
  */
  private void clearButtons(){
   btnPlace.setEnabled(true);
   btnTrans.setEnabled(true);
   btnRemove.setEnabled(true);
   btnMove.setEnabled(true);
  }
  
  private final String helpText="Napoveda k programu simulator Petriho siti\n"+
   "Autori: Vojtech Bartl (xbartl03), Pavel Fryz (xfryzp00)\n\n"+
   "Editace siti:\nJsou dostupne ctyri mody. Mod pro vkladani mist, pro vkladani "+
   "prechodu, pro pohyb s komponentami a pro odstranovani. Pri \nmodu pro vkladani "+
   "se po stisknuti leveho tlacitka mysi vlozi do sceny bud misto, nebo prechod "+
   "dle vybraneho modu. Hrany \nse vytvori spojenim dvou jiz existujicich prvku, "+
   "nebo pretazenim mysi z jednoho prvku do volneho prostoru, kde se vytvori novy\n"+
   "prvek a hrana do nej. Pri modu pro pohyb je mozne libovolne hybat s misty a "+
   "prechody v ramci sceny. Pri modu odstranovani\nje mozne odstranit libovolne "+
   "misto, prechod ci hranu. Pri odstraneni mista, nebo prechodu se odstrani vsechny "+
   "hrany, ktere\nnalezi k dane komponenete. Pro vkladani tokenu mistu se na nem stiskne "+
   "leve tlacitko mysi a objevi se dialog, kde se pomoci\nAdd prida zadany token. "+
   "Tokny je mozne v dialogu i odstranovat pomoci zapsani tokenu a stisku Remove. "+
   "Na hranu se\nlabel vlozi take stiskem leveho tlacitka mysi na hrane. Objevi se "+
   "dialog pro vlozeni labelu. Pro odstraneni labelu se musi otevrit\ndialog a nechat "+
   "prazdny. Pro vkladani straze a vyrazu na prechod se na nem opet stiskne leve tlacitko "+
   "mysi. Do dialogu se\nzadavaji straze oddelene znakem & a vyraz. Korektni zadani se "+
   "kontroluje az behem simulace. Pro odstraneni se musi dana\nstraz nebo vyraz odstranit "+
   "z dialogu.\n\nUkladani a nacitani site:\n"+
   "Site je mozne ukladat a nacitat lokalne v menu (Save a Load), nebo ukladat "+
   "a nacitat site z/na server. Pro praci se sitemi na\nseveru je potreba se nejprve "+
   "na server pripojit. Pri nacitani site je vyber ze vsech dostupnych verzi dane site. "+
   "Rovnez\nje dostupny i popis site. Nazev a popis site je potreba nastavit pri jejim "+
   "ukladani na server v polozce menu Set properties.\n\nPrihlaseni na server:\n"+
   "Prihlaseni na server se provadi v zalozce menu Server. Je mozne se prihlasit, nebo "+
   "si zaregistrovat novy ucet. Pri kazdem\nprihlasovani je potreba zadat host a port "+
   "serveru.\n\nSimulace:\nSimulace se provadi v zalozce munu Simulation->Start. V modu "+
   "simulace se pocita s krokovanim simulace, kde se po kliknuti\nna prechod provede dany "+
   "krok simulace. Cervene jsou zvyrazneny prechody, ktere neni aktualne mozne provest. "+
   "Pro spusteni\ncele simulace je mozne stisknout tlacitko run, "+
    "kde se pote zada maximalni pocet kroku simulace." ;
}
