package client;

import petrinet.*;
import java.awt.geom.Rectangle2D;
import java.awt.*;

public class GTransition extends IntTransition implements GComponent {
  /** Obdelnik reprezentujici prechod */
  protected Rectangle2D rectangle;
  /** Sirka obdelniku */
  public static final int WIDTH = 30;
  /** Vyska obdelniku */
  public static final int HEIGHT = 16;

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
    super();
    rectangle = new Rectangle2D.Double();
    move(position);
  }

  public Arc connectTo (Place place) {
    Arc arc = new GArc(this,place);
    place.addInput(arc);
    myOutArc.add(arc);
    return arc;
  }

  /**
   * Vrati pozici stredu prechodu
   * @return Stred mista
   */
  public Point getPosition(){
    return new Point((int)rectangle.getCenterX(),(int)rectangle.getCenterY());
  }

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
      arc.actualizeLine();
    }
    for(Arc a:myOutArc){
      GArc arc=(GArc)a;
      arc.actualizeLine();
    }
  }

  public void draw (Graphics g){
    Graphics2D g2d = (Graphics2D) g;
    g2d.draw(rectangle);
  }

  public boolean intersects(int x, int y){
    return rectangle.intersects(x-HIT_BOX/2,y-HIT_BOX/2,HIT_BOX,HIT_BOX);
  }
}
