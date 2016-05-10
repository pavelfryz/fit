package client;
import java.util.ArrayList;
import petrinet.*;
import java.awt.Graphics;
import java.awt.MouseInfo;
import java.awt.event.MouseEvent;
import java.awt.event.MouseListener;
import java.awt.event.MouseMotionListener;
import java.awt.*;

import javax.swing.JFrame;
import javax.swing.JPanel;

public class GPanel extends JPanel implements MouseListener, MouseMotionListener{
  /** Mode pridavani mist */
  public static int ADD_PLACE = 1;
  /** Mode pridavani prechodu */
  public static int ADD_TRANSITION = 2;
  /** Mode presouvani komponent */
  public static int MOVE = 3;
  /** Mode odstranovani komponent */
  public static int REMOVE = 4;

  /** Typ vybranepolozky - zadna */
  public static int NONE = 0;
  /** Typ vybranepolozky - misto */
  public static int PLACE = 1;
  /** Typ vybranepolozky - prechod */
  public static int TRANSITION = 2;
  /** Typ vybranepolozky - hrana */
  public static int ARC = 3;

  /** Pole s grafickymi komponentami */
  protected ArrayList<GComponent> myComponents;
  /** Petriho sit */
  protected PetriNet myNet;
  /** Mod */
  protected int mode = 1;

  /** Typ vybrane polozky */
  private int selectedType;
  /** Vybrana polozla */
  private GComponent selected = null;
  /** Pozice vybrane polozky */
  protected Point selectedPosition;

  /**
   * Vytvori panel pro vykreslovani petriho site
   */
  public GPanel() {
    super();
    myComponents = new ArrayList<GComponent>();
    myNet = new PetriNet();
    addMouseListener(this);
    addMouseMotionListener(this);
  }

  /**
   * Odtrani vsechny komponenty
   */
  public void clear(){
    myComponents.clear();
    myNet.removeAll();
  }

  /**
   * Nastavi mod
   * @param mode Nastavovany mod
   */
  public void setMode(int mode){
    this.mode = mode;
  }

  public void mouseClicked(MouseEvent e) {}
  public void mouseEntered(MouseEvent e) {}
  public void mouseExited(MouseEvent e) {}
  public void mouseMoved(MouseEvent e) {}

  public void mouseDragged(MouseEvent e) {
    if(mode == MOVE){
      if(selectedType != NONE)
        selected.move(e.getX(),e.getY());
      repaint();
    }
  }

  public void mousePressed(MouseEvent e) {
    selected = getGComponentAt(e.getX(),e.getY());
    selectedType = getTypeOf(selected);
    if(mode == MOVE){
      myComponents.remove(selected);
      if(selectedType == PLACE){
        selectedPosition = ((GPlace)selected).getPosition();
      }else if(selectedType == TRANSITION){
        selectedPosition = ((GTransition)selected).getPosition();
      }
    }
  }

  public void mouseReleased(MouseEvent e) {
    GComponent atMouse = getGComponentAt(e.getX(),e.getY());
    int typeAtMouse = getTypeOf(atMouse);
    if(mode == REMOVE){
      if(typeAtMouse != NONE){
        myComponents.remove(atMouse);
        if(typeAtMouse == PLACE){
          for(Arc arc:((Place)atMouse).getInputArcs())
            myComponents.remove((GArc)arc);
          for(Arc arc:((Place)atMouse).getOutputArcs())
            myComponents.remove((GArc)arc);
          myNet.remove((Place)atMouse);
        }
        else if(typeAtMouse == TRANSITION){
          for(Arc arc:((BaseTransition)atMouse).getInputArcs())
            myComponents.remove((GArc)arc);
          for(Arc arc:((BaseTransition)atMouse).getOutputArcs())
            myComponents.remove((GArc)arc);
          myNet.remove((BaseTransition)atMouse);
        }
        else
          myNet.remove((Arc)atMouse);
      }
    }else if(mode == MOVE){
      if(selectedType != NONE){
        if(typeAtMouse == ARC || typeAtMouse == NONE){
          if(e.getX()<0 || e.getY()<0 || e.getX()>=getWidth() || e.getY()>=getHeight())
            selected.move(selectedPosition.x,selectedPosition.y);
          else
            selected.move(e.getX(),e.getY());
        }
        myComponents.add(selected);
      }
    }else{
      if(selectedType == NONE || selectedType == ARC){
        if(typeAtMouse == NONE || typeAtMouse == ARC){
          if(mode == ADD_PLACE){
            GPlace place = new GPlace(e.getX(),e.getY());
            myComponents.add(place);
            myNet.add(place);
          }else{
            GTransition transition = new GTransition(e.getX(),e.getY());
            myComponents.add(transition);
            myNet.add(transition);
          }
        }
      }else if(selectedType == PLACE){
        if(typeAtMouse == NONE || typeAtMouse == ARC){
          GTransition transition = new GTransition(e.getX(),e.getY());
          myComponents.add(transition);
          myNet.add(transition);
          GArc arc = (GArc)myNet.connect((Place)selected,transition);
          myComponents.add(arc);
        }else if(typeAtMouse == TRANSITION){
          GArc arc = (GArc)myNet.connect((Place)selected,(BaseTransition)atMouse);
          myComponents.add(arc);
        }
      }else{
        if(typeAtMouse == NONE || typeAtMouse == ARC){
          GPlace place = new GPlace(e.getX(),e.getY());
          myComponents.add(place);
          myNet.add(place);
          GArc arc = (GArc)myNet.connect((BaseTransition)selected,place);
          myComponents.add(arc);
        }else if(typeAtMouse == PLACE){
          GArc arc = (GArc)myNet.connect((BaseTransition)selected,(Place)atMouse);
          myComponents.add(arc);
        }
      }

    }
    selected = null;
    repaint();
  }

  /**
   * Zjisti komponentu na zadane pozici
   * @param x Souradnice na ose x
   * @param y Souradnice na ose y
   * @return Komponenta na souradnicich x,y
   */
  private GComponent getGComponentAt(int x,int y){
    for(GComponent c:myComponents){
      if(c.intersects(x,y)){
        return c;
      }
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
      for(GComponent component: myComponents)
        component.draw(g);
      if(selected != null)
        selected.draw(g);

  }
}
