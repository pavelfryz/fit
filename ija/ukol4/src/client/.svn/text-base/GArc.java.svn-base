package client;

import petrinet.*;
import java.awt.geom.Line2D;
import java.awt.*;

public class GArc extends Arc implements GComponent {
  protected Line2D myLine;
  protected boolean myOrientation;
  public static final boolean P2T = true;
  public static final boolean T2P = false;
  private static final int ENDPOINT = 8;

  /**
   * Vytvori novou hranu orientovanou z mista do prechodu
   */
  public GArc(Place place, BaseTransition transition){
    super(place,transition);
    myOrientation = P2T;
    actualizeLine();
  }

  /**
   * Vytvori novou hranu orientovanou z prechodu do mista
   */
  public GArc(BaseTransition transition, Place place){
    super(place,transition);
    myOrientation = T2P;
    actualizeLine();
  }

  /**
   * Aktualizuje pozici hrany, na zaklade
   * pozice mista a prechodu
   */
  public void actualizeLine(){
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
      double invDist = GPlace.RADIUS/p1.distance(p2);
      result[0]=new Point((int)(p1.x+dir.x*invDist),(int)(p1.y+dir.y*invDist));

      int t1 = dir.x*GTransition.HEIGHT;
      int t2 = dir.y*GTransition.WIDTH;
      int h1 =  t1 - t2;
      int h2 = -t1 - t2;
      if ((h1 > 0 && h2 > 0) || (h1 < 0 && h2 < 0)){
        int y = p2.y;
        if(h1 > 0) y += GTransition.HEIGHT/2;
        else       y -= GTransition.HEIGHT/2;
        result[1]=new Point(p2.x+(dir.x/dir.y)*(y-p2.y),y);
      }else{
        int x = p2.x;
        if(h1 > 0) x -= GTransition.WIDTH/2;
        else       x += GTransition.WIDTH/2;
        result[1]=new Point(x,p2.y+(dir.y/dir.x)*(x-p2.x));
      }
    }
    if(myOrientation==P2T)
      myLine = new Line2D.Double(result[0],result[1]);
    else
      myLine = new Line2D.Double(result[1],result[0]);
  }

  /**
   * Vykresli hranu
   */
  public void draw (Graphics g){
    Graphics2D g2d = (Graphics2D) g;
    g2d.draw(myLine);
    g2d.fillOval((int)(myLine.getX2()-ENDPOINT/2),(int)(myLine.getY2()-ENDPOINT/2),ENDPOINT,ENDPOINT);
  }

  public void move(int x,int y){};

  /**
   * Zjisti prunik hrany s bodem(s jistou rezervou)
   */
  public boolean intersects(int x, int y){
    return myLine.intersects(x-HIT_BOX/2,y-HIT_BOX/2,HIT_BOX,HIT_BOX);
  }
}
