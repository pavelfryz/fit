package client;

import petrinet.*;
import java.awt.geom.Ellipse2D;
import java.awt.*;

public class GPlace extends Place implements GComponent {
  /** Kruh reprezenrujici misto */
  Ellipse2D circle;
  /** Polomer kruhu mista */
  public static final int RADIUS = 12;

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
    super();
    circle = new Ellipse2D.Double();
    move(position);
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
      arc.actualizeLine();
    }
    for(Arc a:myOutArc){
      GArc arc=(GArc)a;
      arc.actualizeLine();
    }
  }

  public void draw (Graphics g){
    Graphics2D g2d = (Graphics2D) g;
    g2d.draw(circle);
  }

  public boolean intersects(int x, int y){
    return circle.intersects(x-HIT_BOX/2,y-HIT_BOX/2,HIT_BOX,HIT_BOX);
  }
}
