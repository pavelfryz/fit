package client;

import petrinet.*;

import org.dom4j.Branch;
import org.dom4j.Element;
import java.awt.geom.Rectangle2D;
import java.awt.*;
import java.util.Iterator;

/**
 * Trida GTransiton slouzi pro grafickou
 * reprezentaci prechodu s celosicelnymi tokeny.
 * @author Vojtech Bartl - xbartl03, Pavel Fryz - xfryzp00
 */
public class GTransition extends IntTransition implements GComponent, XMLParse {
  /** Obdelnik reprezentujici prechod */
  protected Rectangle2D rectangle;
  /** Sirka obdelniku */
  public static int WIDTH = 30;
  /** Vyska obdelniku */
  public static int HEIGHT = 16;
  /** Id v ramci site, nastavuje sit */
  protected int id=-1;
  /** Jesli je zadana komponenta vybrana */
  private boolean selected = false;
  /** Barva cary */
  public static Color lineColor = Color.black;
  /** Barva vyplne */
  public static Color fillColor = Color.white;
  /** Barva cary pokud je komponenta vybrana */
  public static Color selectColor = Color.green;
  /** Barva cary pokud neni komponenta spravne nastavena */
  public static Color errorColor = Color.red;
  /** Jestli je hrana ve stavu simulace */
  public boolean simulation = false;


  /**
   * Vytvori novy prechod
   * @param x Souradnice na ose x
   * @param y Souradnice na ose y
   */
  public GTransition(int x,int y){
    super();
    rectangle = new Rectangle2D.Double();
    move(x,y);
  }

  /**
   * Vytvori novy prechod
   * @param position Souradnice stredu prechodu
   */
  public GTransition(Point position){
    this(position.x,position.y);
  }

  /**
   * Vytvori novy prechod z XML elementu
   * @param element XML element
   */
  public GTransition(Element element){
    this(0,0);
    fromXML(element);
  }

  /**
   * Nastavi prechod z XML elementu.
   * @param element XML element
   */
  public void fromXML(Element element){
    id=Integer.parseInt(element.attributeValue("id"));
    int x=Integer.parseInt(element.attributeValue("x"));
    int y=Integer.parseInt(element.attributeValue("y"));
    move(x,y);
    if(element.elementText("executable").equals("true"))
      myExecutable=true;
    myGuard = element.elementText("Guard");
    Iterator i=element.elementIterator("Expr");
    while(i.hasNext())
      addExpression(((Element)i.next()).getText());
  }

  /**
   * Zmeni stav vybranosti prechodu, z vybraneho na nevybrany a naopak
   */
  public void changeSelected(){
    selected = !selected;
  }

  /**
   * Nastavi id prechodu, mela by byt volana pred metodou toXML
   * @param identifier Id prechodu
   */
  public void setId(int identifier){
    id = identifier;
  }

  /**
   * Vrati id prechodu
   * @return Id prechodu
   */
  public int getId(){
    return id;
  }

  public Arc connectTo (Place place) {
    Arc arc = new GArc(this,place);
    place.addInput(arc);
    myOutArc.add(arc);
    return arc;
  }

  public void updateGraphics(){
    move(getPosition());
  }

  /**
   * Vrati pozici stredu prechodu
   * @return Stred mista
   */
  public Point getPosition(){
    return new Point((int)rectangle.getCenterX(),(int)rectangle.getCenterY());
  }

  /**
   * Presune prechod na zadanou pozici
   * @param position Nova pozice
   */
  public void move(Point position){
    move(position.x,position.y);
  }

  public void move(int x, int y){
    Rectangle2D.Double rectangle = (Rectangle2D.Double) this.rectangle;
    rectangle.x=x-WIDTH/2;
    rectangle.y=y-HEIGHT/2;
    rectangle.width=WIDTH;
    rectangle.height=HEIGHT;
    for(Arc a:myInArc){
      GArc arc=(GArc)a;
      arc.updateGraphics();
    }
    for(Arc a:myOutArc){
      GArc arc=(GArc)a;
      arc.updateGraphics();
    }
  }

  public void draw (Graphics g){
    Graphics2D g2d = (Graphics2D) g;
    g2d.setPaint(fillColor);
    g2d.fill(rectangle);
    if(simulation){
      g2d.setPaint(errorColor);
      if(myExecutable)
        g2d.setPaint(selectColor);
    }
    else if(selected)
      g2d.setPaint(selectColor);
    else
      g2d.setPaint(lineColor);
    g2d.draw(rectangle);
    g2d.setPaint(Color.red);
    g2d.drawString(myGuard,(int)rectangle.getMinX(),(int)rectangle.getMinY());
    if(myExpression.size()>0)
    g2d.drawString(myExpression.get(0),(int)rectangle.getMinX(),(int)rectangle.getMaxY()+12);
  }

  public boolean intersects(int x, int y){
    return rectangle.intersects(x-HIT_BOX/2,y-HIT_BOX/2,HIT_BOX,HIT_BOX);
  }

  public void toXML(Branch branch){
    Point pos = getPosition();
    Element transition = branch.addElement("GTransition")
      .addAttribute("id",String.valueOf(id))
      .addAttribute("x",String.valueOf(pos.x))
      .addAttribute("y",String.valueOf(pos.y));
    String executable = (myExecutable)?"true":"false";
    transition.addElement("executable").addText(executable);
    Element guard = transition.addElement("Guard")
      .addText(myGuard);
    for(String e:myExpression){
      Element ex = transition.addElement("Expr")
        .addText(e);
    }
  }
}