package client;

import petrinet.Arc;
import petrinet.Place;
import petrinet.BaseTransition;
import petrinet.XMLParse;

import org.dom4j.Branch;
import org.dom4j.Element;
import java.awt.geom.Line2D;
import java.awt.*;

/**
 * Trida GArc slouzi pro grafickou
 * reprezentaci hrany.
 * @author Vojtech Bartl - xbartl03, Pavel Fryz - xfryzp00
 */
public class GArc extends Arc implements GComponent, XMLParse {
  /** Cara reprezentujici hranu */
  protected Line2D myLine;
  /** Velikost konce urcujici smer hrany */
  public static int ENDPOINT = 10;
  /** Posun koncu od okraje mista a prechodu */
  public static int MOVEPOINT = 2;
  /** Jesli je zadana komponenta vybrana */
  private boolean selected = false;
  /** Barva cary */
  public static Color lineColor = Color.gray;
  /** Barva vyplne */
  public static Color fillColor = Color.black;
  /** Barva cary pokud je komponenta vybrana */
  public static Color selectColor = Color.green;
  /** Barva cary pokud neni komponenta spravne nastavena */
  public static Color errorColor = Color.red;

  /**
   * Vytvori novou hranu orientovanou z mista do prechodu
   */
  public GArc(Place place, BaseTransition transition){
    super(place,transition);
    updateGraphics();
  }

  /**
   * Vytvori novou hranu orientovanou z prechodu do mista
   */
  public GArc(BaseTransition transition, Place place){
    super(transition,place);
    updateGraphics();
  }

  /**
   * Aktualizuje pozici hrany, na zaklade
   * pozice mista a prechodu
   */
  public void updateGraphics(){
    Point result[] = new Point[2];
    GPlace place = (GPlace) myPlace;
    GTransition transition = (GTransition) myTransition;

    Point p1 = place.getPosition();
    Point p2 = transition.getPosition();
    if(p1.equals(p2)){
      result[0]=new Point(p1.x,p1.y+GPlace.RADIUS);
      result[1]=new Point(p2.x,p2.y+GTransition.HEIGHT/2);
    }else{
      Point dir = new Point(p2.x-p1.x,p2.y-p1.y);
      double invDist = (GPlace.RADIUS+MOVEPOINT)/p1.distance(p2);
      result[0]=new Point((int)(p1.x+dir.x*invDist),(int)(p1.y+dir.y*invDist));

      int t1 = dir.x*GTransition.HEIGHT;
      int t2 = dir.y*GTransition.WIDTH;
      int h1 =  t1 - t2;
      int h2 = -t1 - t2;
      if ((h1 > 0 && h2 > 0) || (h1 < 0 && h2 < 0)){
        int y = p2.y;
        if(h1 > 0) y += GTransition.HEIGHT/2+MOVEPOINT;
        else       y -= GTransition.HEIGHT/2+MOVEPOINT;
        result[1]=new Point(p2.x+(dir.x/dir.y)*(y-p2.y),y);
      }else{
        int x = p2.x;
        if(h1 > 0) x -= GTransition.WIDTH/2+MOVEPOINT;
        else       x += GTransition.WIDTH/2+MOVEPOINT;
        result[1]=new Point(x,p2.y+(dir.y/dir.x)*(x-p2.x));
      }
    }
    if(myOrientation==P2T)
      myLine = new Line2D.Double(result[0],result[1]);
    else
      myLine = new Line2D.Double(result[1],result[0]);
  }

  /**
   * Zmeni stav promene, ktera udava jestli je hrana vybrana.
   * Z vybrane na nevybranou a naopak.
   */
  public void changeSelected(){
    selected = !selected;
  }

  /**
   * Vykresli hranu
   */
  public void draw (Graphics g){
    Graphics2D g2d = (Graphics2D) g;
    if(selected)
      g2d.setPaint(selectColor);
    else
      g2d.setPaint(lineColor);
    g2d.draw(myLine);
    g2d.setPaint(fillColor);
    g2d.fillOval((int)(myLine.getX2()-ENDPOINT/2),(int)(myLine.getY2()-ENDPOINT/2),ENDPOINT,ENDPOINT);
    g2d.setPaint(Color.red);
    g2d.drawString(myLabel,(int)myLine.getBounds().getCenterX(),(int)myLine.getBounds().getCenterY());
  }

  /**
   * Implementace z rozhrani component, nedela nic, hranou nelze samostatne pohybovat
   */
  public void move(int x,int y){};

  /**
   * Zjisti prunik hrany s bodem(s jistou rezervou)
   */
  public boolean intersects(int x, int y){
    return myLine.intersects(x-HIT_BOX/2,y-HIT_BOX/2,HIT_BOX,HIT_BOX);
  }

  /**
   * Nastavi hranu z XML elementu
   * @param element XML element
   */
  public void fromXML(Element element){
    myLabel = element.elementText("label");
  }

  public void toXML(Branch branch){
    Element arc = branch.addElement("GArc");
    arc.addElement("place")
      .addAttribute("id",String.valueOf(((GPlace)myPlace).getId()));
    arc.addElement("transition")
      .addAttribute("id",String.valueOf(((GTransition)myTransition).getId()));
    if(P2T == myOrientation)
      arc.addAttribute("orientation","toTransition");
    else
      arc.addAttribute("orientation","toPlace");
    arc.addElement("label")
      .addText(myLabel);
  }
}