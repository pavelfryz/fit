package client;

import petrinet.*;

import org.dom4j.Branch;
import org.dom4j.Element;
import java.awt.geom.Ellipse2D;
import java.awt.*;
import java.util.Iterator;

/**
 * Trida GPlace slouzi pro grafickou
 * reprezentaci mista.
 * @author Vojtech Bartl - xbartl03, Pavel Fryz - xfryzp00
 */
public class GPlace extends Place implements GComponent, XMLParse {
  /** Kruh reprezenrujici misto */
  Ellipse2D circle;
  /** Polomer kruhu mista */
  public static int RADIUS = 12;
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
  /** Jestli se ma vykreslovat retezec s tokeny*/
  public static boolean showToken = false;

  /**
   * Vytvori nove misto
   * @param x Souradnice na ose x
   * @param y Souradnice na ose y
   */
  public GPlace(int x,int y){
    super();
    circle = new Ellipse2D.Double();
    move(x,y);
  }

  /**
   * Vytvori nove misto
   * @param position Souradnice stredu
   */
  public GPlace(Point position){
    this(position.x,position.y);
  }

  /**
   * Vytvori nove misto z XML elementu
   * @param element XML element
   */
  public GPlace(Element element){
    this(0,0);
    fromXML(element);
  }

  public void updateGraphics(){
    move(getPosition());
  }

  /**
   * Zmeni stav vybranosti mista, z vybraneho na nevybrane a naopak.
   */
  public void changeSelected(){
    selected = !selected;
  }

  /**
   * Nastavi hodnotu mista z XML elementu.
   * @param element XML element
   */
  public void fromXML(Element element){
    id=Integer.parseInt(element.attributeValue("id"));
    int x=Integer.parseInt(element.attributeValue("x"));
    int y=Integer.parseInt(element.attributeValue("y"));
    move(x,y);

    Iterator i=element.elementIterator("IntToken");
    while(i.hasNext())
      addToken(new IntToken((Element)i.next()));
  }

  /**
   * Nastavi id mista, mela by byt volana pred metodou toXML
   * @param identifier Id mista
   */
  public void setId(int identifier){
    id = identifier;
  }

  /**
   * Vrati id mista
   * @return Id mista
   */
  public int getId(){
    return id;
  }

  public Arc connectTo (BaseTransition transition) {
    Arc arc = new GArc(this,transition);
    transition.addInput(arc);
    myOutArc.add(arc);
    return arc;
  }

  /**
   * Vrati pozici stredu mista
   * @return Stred mista
   */
  public Point getPosition(){
    return new Point((int)circle.getCenterX(),(int)circle.getCenterY());
  }

  /**
   * Presune misto na zadanou pozici
   * @param position Nova pozice
   */
  public void move(Point position){
    move(position.x,position.y);
  }

  public void move(int x, int y){
    Ellipse2D.Double circle = (Ellipse2D.Double) this.circle;
    circle.x=x-RADIUS;
    circle.y=y-RADIUS;
    circle.height=RADIUS*2;
    circle.width=RADIUS*2;
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
    g2d.fill(circle);
    if(selected)
      g2d.setPaint(selectColor);
    else
      g2d.setPaint(lineColor);
    g2d.draw(circle);
    g2d.setPaint(Color.red);
    if(showToken)
      g2d.drawString(getTokenString(),(int)circle.getMinX(),(int)circle.getMinY());
  }

  public boolean intersects(int x, int y){
    return circle.intersects(x-HIT_BOX/2,y-HIT_BOX/2,HIT_BOX,HIT_BOX);
  }

  public void toXML(Branch branch){
    Point pos = getPosition();
    Element place = branch.addElement("GPlace")
      .addAttribute("id",String.valueOf(id))
      .addAttribute("x",String.valueOf(pos.x))
      .addAttribute("y",String.valueOf(pos.y));
    for(Token token:myToken)
      ((IntToken)token).toXML(place);
  }
}