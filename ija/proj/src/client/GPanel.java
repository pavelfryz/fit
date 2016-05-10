package client;

import petrinet.*;

import java.util.*;
import java.awt.*;
import java.awt.event.*;
import javax.swing.*;
import javax.swing.text.*;
import java.io.*;
import org.dom4j.Element;
import org.dom4j.Branch;
import java.net.*;
import java.io.*;

import org.dom4j.Document;
import org.dom4j.DocumentFactory;
import org.dom4j.io.OutputFormat;
import org.dom4j.DocumentHelper;
import org.dom4j.io.OutputFormat;
import org.dom4j.io.XMLWriter;
import java.io.File;
import java.io.FileOutputStream;
import java.io.IOException;
import org.dom4j.io.SAXReader;
import org.dom4j.DocumentException;
import org.dom4j.Document;



/**
 * Panel obsahujici petriho sit
 * @author Vojtech Bartl - xbartl03, Pavel Fryz - xfryzp00
 */
public class GPanel extends JPanel implements MouseListener, MouseMotionListener{
  /** Mode pridavani mist */
  public static int ADD_PLACE = 1;
  /** Mode pridavani prechodu */
  public static int ADD_TRANSITION = 2;
  /** Mode presouvani komponent */
  public static int MOVE = 3;
  /** Mode odstranovani komponent */
  public static int REMOVE = 4;
  /** Mode simulace */
  public static int SIMULATION = 5;

  /** Typ vybranepolozky - zadna */
  public static int NONE = 0;
  /** Typ vybranepolozky - misto */
  public static int PLACE = 1;
  /** Typ vybranepolozky - prechod */
  public static int TRANSITION = 2;
  /** Typ vybranepolozky - hrana */
  public static int ARC = 3;

  /** Pole s hranami grafu */
  protected ArrayList<GComponent> myGArcs;
  /** Pole s ostatnimi komponenty grafu */
  protected ArrayList<GComponent> myGOthers;

  /** Petriho sit */
  protected GNet myNet;
  /** Mod */
  protected int mode = 1;

  private GComponent hover = null;
  /** Typ vybrane polozky */
  private int selectedType = NONE;
  /** Vybrana polozla */
  private GComponent selected = null;
  /** Pozice vybrane polozky */
  protected Point selectedPosition;
  /** Socket pripojeny k serveru */
  private Socket socket;

  /**
   * Vytvori panel pro vykreslovani petriho site
   */
  public GPanel() {
    super();
    myGArcs = new ArrayList<GComponent>();
    myGOthers = new ArrayList<GComponent>();
    myNet = new GNet();
    addMouseListener(this);
    addMouseMotionListener(this);
    setBackground(Color.white);
  }

  /**
   * Vrati sit tohoto panelu v kompaktni verzi XML dokumentu.
   * Bez xml deklarace a odsazovani.
   * @return Retezec s reprezentaci site
   */
  public String toCompactXML()throws IOException{
    Document document=DocumentFactory.getInstance().createDocument();
    myNet.toXML(document);
    ByteArrayOutputStream baos=new ByteArrayOutputStream();
    OutputFormat format=OutputFormat.createCompactFormat();
    format.setSuppressDeclaration(true);
    XMLWriter writer=new XMLWriter(baos, format);
    writer.write(document);
    writer.flush();
    return baos.toString();
  }

  /**
   * Nastavi socket
   * @param socket Novy socket
   */
  void setSocket(Socket socket){
    this.socket=socket;
  }

  /**
   * Ulozi sit do souboru
   * @param file Soubor
   */
  public void save(File file){
    Document document=DocumentFactory.getInstance().createDocument();
    myNet.toXML(document);
    try{
      FileOutputStream fos=new FileOutputStream(file);
      OutputFormat format=OutputFormat.createPrettyPrint();
      XMLWriter writer=new XMLWriter(fos, format);
      writer.write(document);
      writer.flush();
    }
    catch(IOException ex){
      JOptionPane.showMessageDialog(this, "Nepodarilo se ulozit XML soubor!", "Pozor", JOptionPane.ERROR_MESSAGE);
    }
  }

  /**
   * Nacte sit ze souvoru
   * @param file Soubor
   */
  public void open(File file){
    clear();
    SAXReader reader=new SAXReader();
    Document document = null;
    try{
      document=reader.read(file);
      myNet = new GNet(document.getRootElement());
      Iterator i = myNet.getArcsIter();
      while(i.hasNext())
        myGArcs.add((GComponent)i.next());
      i = myNet.getTransIter();
      while(i.hasNext())
        myGOthers.add((GComponent)i.next());
      i = myNet.getPlaceIter();
      while(i.hasNext())
        myGOthers.add((GComponent)i.next());
    }
    catch(DocumentException de){
      JOptionPane.showMessageDialog(this, "Nepodarilo se nacist XML soubor!", "Pozor", JOptionPane.ERROR_MESSAGE);
    }
    repaint();
  }

  /**
   * Nastavi jmeno site
   * @param name Jmeno site
   */
  public void setName(String name){
    myNet.name = name;
  }

  /**
   * Nastavi popis site
   * @param description Popis site
   */
  public void setDescription(String description){
    myNet.description = description;
  }

  /**
   * Vrati nazev site
   * @return Nazev site
   */
  public String getName(){
    return myNet.name;
  }

  /**
   * Vrati popis site
   * @return Popis site
   */
  public String getDescription(){
    return myNet.description;
  }


  public void fromXML(Element element){
    myNet = new GNet(element);
    myGArcs.clear();
    myGOthers.clear();
    Iterator i = myNet.getArcsIter();
    while(i.hasNext())
      myGArcs.add((GComponent)i.next());
    i = myNet.getTransIter();
    while(i.hasNext())
      myGOthers.add((GComponent)i.next());
    i = myNet.getPlaceIter();
    while(i.hasNext())
      myGOthers.add((GComponent)i.next());
    repaint();
  }

  public void toXML(Branch branch){
    myNet.toXML(branch);
  }


  /**
   * Odtrani vsechny komponenty
   */
  public void clear(){
    myGArcs.clear();
    myGOthers.clear();
    myNet.removeAll();
    repaint();
  }

  /**
   * Nastavi mod
   * @param mode Nastavovany mod
   */
  public void setMode(int mode){
    boolean simulation = mode==SIMULATION;
    for(GComponent c:myGOthers){
      if(c instanceof GTransition)
        ((GTransition)c).simulation = simulation;
    }
    this.mode = mode;
  }

  /**
   * Funkce na obsluhu kliknuti mysi
   */
  public void mouseClicked(MouseEvent e) {
    GComponent c = getGComponentAt(e.getX(),e.getY());
    int type = getTypeOf(c);
    if(mode == MOVE){
      if(type == ARC)
        arcDialog((GArc)c);
      else if(type == PLACE)
        placeDialog((GPlace)c);
      else if(type == TRANSITION)
        transitionDialog((GTransition)c);
    }
  }
  public void mouseEntered(MouseEvent e) {}
  public void mouseExited(MouseEvent e) {}
  /**
   * Funkce na obsluhu pohybu mysi
   */
  public void mouseMoved(MouseEvent e) {
    GComponent c = getGComponentAt(e.getX(),e.getY());
    if((c==null && hover!=null)||(c!=hover && hover !=null)){
      hover.changeSelected();
      hover = null;
      repaint();
    }
    if(c!=null && hover == null){
      hover = c;
      hover.changeSelected();
      repaint();
    }
  }

  /**
   * Funkce na obsluhu tazeni mysi
   */
  public void mouseDragged(MouseEvent e) {
   if(mode == MOVE){
      if(selectedType != NONE)
        selected.move(e.getX(),e.getY());
      repaint();
    }
  }

  /**
   * Funkce na obsluhu zmacknuti tlacitka mysi
   */
  public void mousePressed(MouseEvent e) {
    if (e.getButton() != MouseEvent.BUTTON1)
      return;
    selected = getGComponentAt(e.getX(),e.getY());
    selectedType = getTypeOf(selected);
    if(mode == MOVE){
      if(selectedType == ARC){
        myGArcs.remove(selected);
      }else if(selectedType == PLACE){
        myGOthers.remove(selected);
        selectedPosition = ((GPlace)selected).getPosition();
      }else if(selectedType == TRANSITION){
        myGOthers.remove(selected);
        selectedPosition = ((GTransition)selected).getPosition();
      }
    }
  }

  /**
   * Funkce na obsluhu pusteni tlacitka mysi
   */
  public void mouseReleased(MouseEvent e) {
    if (e.getButton() != MouseEvent.BUTTON1)
      return;
    GComponent atMouse = getGComponentAt(e.getX(),e.getY());
    int typeAtMouse = getTypeOf(atMouse);
    if(mode == REMOVE){
      if(typeAtMouse != NONE){
        if(typeAtMouse == PLACE){
          myGOthers.remove(atMouse);
          for(Arc arc:((Place)atMouse).getInputArcs())
            myGArcs.remove((GArc)arc);
          for(Arc arc:((Place)atMouse).getOutputArcs())
            myGArcs.remove((GArc)arc);
          myNet.remove((Place)atMouse);
        }
        else if(typeAtMouse == TRANSITION){
          myGOthers.remove(atMouse);
          for(Arc arc:((BaseTransition)atMouse).getInputArcs())
            myGArcs.remove((GArc)arc);
          for(Arc arc:((BaseTransition)atMouse).getOutputArcs())
            myGArcs.remove((GArc)arc);
          myNet.remove((BaseTransition)atMouse);
        }
        else{
          myGArcs.remove(atMouse);
          myNet.remove((Arc)atMouse);
        }
      }
    }else if(mode == MOVE){
      if(selectedType != NONE){
        if(typeAtMouse == ARC || typeAtMouse == NONE){
          if(e.getX()<0 || e.getY()<0 || e.getX()>=getWidth() || e.getY()>=getHeight())
            selected.move(selectedPosition.x,selectedPosition.y);
          else
            selected.move(e.getX(),e.getY());
        }
        if(selectedType == ARC)
          myGArcs.add(selected);
        else
          myGOthers.add(selected);
      }
    }else if(mode==SIMULATION){
      if(typeAtMouse == TRANSITION){
        if(((GTransition)atMouse).executable()){
          try{
            PrintWriter out = new PrintWriter(socket.getOutputStream(), true);
            BufferedReader in = new BufferedReader(new InputStreamReader(socket.getInputStream()));
            out.println("Step.");
            in.readLine();
            Document document=DocumentFactory.getInstance().createDocument();
            toXML(document);
            document.getRootElement().addElement("step").addText(String.valueOf(((GTransition)atMouse).getId()));
            ByteArrayOutputStream baos=new ByteArrayOutputStream();
            OutputFormat format=OutputFormat.createCompactFormat();
            format.setSuppressDeclaration(true);
            XMLWriter writer=new XMLWriter(baos, format);
            writer.write(document);
            writer.flush();
            out.println(baos.toString());
            String input = in.readLine();
            if(input.matches("^Error.*")){
              JOptionPane.showMessageDialog(this, input, "Error", JOptionPane.ERROR_MESSAGE);
              return;
            }
            document=DocumentHelper.parseText(input);
            fromXML(document.getRootElement());
            setMode(GPanel.SIMULATION);
          }
          catch(Exception eee){}
        }
      }
    }else{
      if(e.getX()<0 || e.getY()<0 || e.getX()>=getWidth() || e.getY()>=getHeight()){
        selected = null;
        selectedType = NONE;
        repaint();
        return;
      }
      if(selectedType == NONE || selectedType == ARC){
        if(typeAtMouse == NONE || typeAtMouse == ARC){
          if(selected == atMouse && selectedType == ARC){
            arcDialog((GArc)selected);
          }
          else if(mode == ADD_PLACE){
            GPlace place = new GPlace(e.getX(),e.getY());
            myGOthers.add(place);
            myNet.add(place);
          }else{
            GTransition transition = new GTransition(e.getX(),e.getY());
            myGOthers.add(transition);
            myNet.add(transition);
          }
        }
      }else if(selectedType == PLACE){
        if(typeAtMouse == NONE || typeAtMouse == ARC){
          GTransition transition = new GTransition(e.getX(),e.getY());
          myGOthers.add(transition);
          myNet.add(transition);
          GArc arc = (GArc)myNet.connect((Place)selected,transition);
          myGArcs.add(arc);
        }else if(typeAtMouse == TRANSITION){
          for(Arc a:myNet.getArcs((Place)selected,(BaseTransition)atMouse)){
            myGArcs.remove((GArc)a);
            myNet.remove(a);
          }
          GArc arc = (GArc)myNet.connect((Place)selected,(BaseTransition)atMouse);
          myGArcs.add(arc);
        }else if(selected == atMouse){
          placeDialog((GPlace)selected);
        }
      }else{
        if(typeAtMouse == NONE || typeAtMouse == ARC){
          GPlace place = new GPlace(e.getX(),e.getY());
          myGOthers.add(place);
          myNet.add(place);
          GArc arc = (GArc)myNet.connect((BaseTransition)selected,place);
          myGArcs.add(arc);
        }else if(typeAtMouse == PLACE){
          for(Arc a:myNet.getArcs((Place)atMouse,(BaseTransition)selected)){
            myGArcs.remove((GArc)a);
            myNet.remove(a);
          }
          GArc arc = (GArc)myNet.connect((BaseTransition)selected,(Place)atMouse);
          myGArcs.add(arc);
        }else if(selected == atMouse){
          transitionDialog((GTransition)selected);
        }
      }

    }
    GComponent c = getGComponentAt(e.getX(),e.getY());
    if((c==null && hover!=null)||(c!=hover && hover !=null)){
      hover.changeSelected();
      hover = null;
      repaint();
    }
    if(c!=null && hover == null){
      hover = c;
      hover.changeSelected();
      repaint();
    }
    selected = null;
    selectedType = NONE;
    repaint();
  }

  /**
   * Otevre dialog pro nastaveni hodnot prechodu
   * @param transition Nastavovany prechod
   */
  void transitionDialog(GTransition transition){
    class TransitionDialog extends JDialog implements ActionListener
    {
      JTextField guard = new JTextField(20);
      JTextField expression = new JTextField(20);
      JButton btnADD = new JButton("Add");
      JButton btnRM = new JButton("Remove");
      JButton btnOK = new JButton("Ok");
      GTransition transition;
      public TransitionDialog(Component parent, GTransition transition)
      {
        setModal(true);
        setTitle("Transition");
        this.transition = transition;

        guard.setText(transition.getGuard());
        expression.setText(transition.getExpression(0));

        getContentPane().setLayout(new BorderLayout());
        setDefaultCloseOperation(DISPOSE_ON_CLOSE);

        JPanel top = new JPanel();
        top.setLayout(new BorderLayout());
        top.add(new JLabel("Guard:"),BorderLayout.NORTH);
        top.add(guard,BorderLayout.CENTER);

        JPanel center = new JPanel();
        center.setLayout(new BorderLayout());
        center.add(new JLabel("Expression"),BorderLayout.NORTH);
        center.add(expression,BorderLayout.CENTER);

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
        transition.setGuard(guard.getText());
        transition.clearExpressios();
        if(!(expression.getText().isEmpty()))
          transition.addExpression(expression.getText());
        dispose();
      }
    }
    new TransitionDialog(this,transition);
  }

  /**
   * Otevre dialog pro nastaveni hodnot hrany
   * @param arc Nastavovana hrana
   */
  void arcDialog(GArc arc){
    String label = JOptionPane.showInputDialog(this.getRootPane(), "Arc label","Arc",JOptionPane.PLAIN_MESSAGE);
    if(label != null) arc.setLabel(label);
  }

  /**
   * Otevre dialog pro nastaveni hodnot mista
   * @param place Nastavovane misto
   */
  void placeDialog(GPlace place){
    class PlaceDialog extends JDialog implements ActionListener
    {
      JTextField token = new JTextField();
      JTextArea jta = new JTextArea(5,20);
      JButton btnADD = new JButton("Add");
      JButton btnRM = new JButton("Remove");
      JButton btnOK = new JButton("Ok");
      GPlace place;
      public PlaceDialog(Component parent, GPlace place)
      {
        setModal(true);
        setTitle("Place");
        this.place = place;
        getContentPane().setLayout(new BorderLayout());
        setDefaultCloseOperation(DISPOSE_ON_CLOSE);
        jta.setEditable(false);
        jta.setLineWrap(true);
        JScrollPane scrollPane = new JScrollPane(jta);

        JPanel top = new JPanel();
        top.setLayout(new BorderLayout());
        top.add(new JLabel("Tokens at place:"),BorderLayout.NORTH);
        top.add(scrollPane,BorderLayout.CENTER);

        jta.setText(place.getTokenString());

        JPanel center = new JPanel();
        center.setLayout(new BorderLayout());
        center.add(new JLabel("Token"),BorderLayout.NORTH);
        center.add(token,BorderLayout.CENTER);
        JPanel buttons = new JPanel(new GridLayout(1,2));
        center.add(buttons,BorderLayout.SOUTH);
        token.setColumns(20);
        buttons.add(btnADD); buttons.add(btnRM);

        getContentPane().add(top,BorderLayout.NORTH);
        getContentPane().add(center,BorderLayout.CENTER);
        getContentPane().add(btnOK,BorderLayout.SOUTH);


        ((AbstractDocument)token.getDocument()).setDocumentFilter(new DocumentFilter(){
          public void insertString(DocumentFilter.FilterBypass fb, int offset,
                                   String text, AttributeSet attr) throws BadLocationException {
            fb.insertString(offset, text.replaceAll("[^-\\d]", ""), attr);
          }

          public void replace(DocumentFilter.FilterBypass fb, int offset, int length,
                              String text, AttributeSet attr) throws BadLocationException {
            fb.replace(offset, length, text.replaceAll("[^-\\d]", ""), attr);
          }
        });

        btnADD.addActionListener(this);
        btnOK.addActionListener(this);
        btnRM.addActionListener(this);
        setResizable(false);
        pack();
        setLocationRelativeTo(parent);
        setVisible(true);
      }
      public void actionPerformed(ActionEvent ae)
      {
        if(ae.getSource() == btnADD){
          if(token.getText().isEmpty())
            return;
          try{
            place.addToken(new IntToken(token.getText()));
          }catch(NumberFormatException e){
            JOptionPane.showMessageDialog(this, e.getMessage(), "Wrong format", JOptionPane.ERROR_MESSAGE);
            return;
          }
          jta.setText(place.getTokenString());
          token.setText("");
        }
        else if(ae.getSource() == btnRM){
          if(token.getText().isEmpty())
            return;
          try{
            place.removeToken(new IntToken(token.getText()));
          }catch(NumberFormatException e){
            JOptionPane.showMessageDialog(this, e.getMessage(), "Wrong format", JOptionPane.ERROR_MESSAGE);
            return;
          }
          jta.setText(place.getTokenString());
          token.setText("");
        }
        else
          dispose();
      }
    }
    new PlaceDialog(this,place);
  }

  /**
   * Aktualizuje grafiku vsech polozek
   */
  void updateGraphics(){
    for(GComponent c:myGArcs)
      c.updateGraphics();
    for(GComponent c:myGOthers)
      c.updateGraphics();
    repaint();
  }

  /**
   * Zjisti komponentu na zadane pozici
   * @param x Souradnice na ose x
   * @param y Souradnice na ose y
   * @return Komponenta na souradnicich x,y
   */
  private GComponent getGComponentAt(int x,int y){
    GComponent c = null;

    ListIterator i = myGOthers.listIterator(myGOthers.size());
    while (i.hasPrevious()){
      c=(GComponent)i.previous();
      if(c.intersects(x,y))
        return c;
    }
    i = myGArcs.listIterator(myGArcs.size());
    while (i.hasPrevious()){
      c=(GComponent)i.previous();
      if(c.intersects(x,y))
        return c;
    }
    return null;
  }

  /**
   * Vrati typ koponenty
   * @param component Komponenta
   * @return Typ komponenty
   */
  private int getTypeOf(GComponent component) {
    if(component instanceof Place)
      return PLACE;
    else if(component instanceof BaseTransition)
      return TRANSITION;
    else if(component instanceof Arc)
      return ARC;
    return NONE;
  }

  @Override
  public void paintComponent(Graphics g) {
      super.paintComponent(g);
      Graphics2D g2d = (Graphics2D) g;
      g2d.setRenderingHint(RenderingHints.KEY_ANTIALIASING, RenderingHints.VALUE_ANTIALIAS_ON);
      g2d.setStroke(new BasicStroke(2));
      for(GComponent component: myGArcs)
        component.draw(g);
      for(GComponent component: myGOthers)
        component.draw(g);
      if(selected != null){
        selected.draw(g);
      }

  }

}