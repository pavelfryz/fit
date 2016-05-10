package petrinet;
import java.util.ArrayList;

/**
 * Zakladni trida pro prechod petriho
 * site, od ktere jsou odvozeny vsechny
 * ostatni prechody.
 * @author Vojtech Bartl - xbartl03, Pavel Fryz - xfryzp00
 */
abstract public class BaseTransition{
  /** Seznam vstupnich hran */
  protected ArrayList<Arc> myInArc;
  /** Seznam vystupnich hran */
  protected ArrayList<Arc> myOutArc;
  /** Retezec s podminkou pro provedeni prechodu */
  protected String myGuard;
  /** Seznam vyrazu, ktere se provedou pri prechodu */
  protected ArrayList<String> myExpresion;

  /**
   * Vytvori prechod
   */
  public BaseTransition() {
    myInArc = new ArrayList<Arc>();
    myOutArc = new ArrayList<Arc>();
    myGuard = "";
    myExpresion = new ArrayList<String>();
  }

  public ArrayList<Arc> getInputArcs(){
    return myInArc;
  }

  public ArrayList<Arc> getOutputArcs(){
    return myOutArc;
  }

  /**
   * Prida vstupni hranu
   * @param arc Vstupni hrana
   */
  public void addInput (Arc arc) {
    myInArc.add(arc);
  }

  /**
   * Spoji prechod s mistem
   * @param place Spojovane misto
   * @return Nova hrana
   */
  public Arc connectTo (Place place) {
    Arc arc = new Arc(place,this);
    place.addInput(arc);
    myOutArc.add(arc);
    return arc;
  }

  /**
   * Odstarni prechod a vsechny jeho hrany
   */
  public void remove(){
    for(Arc arc : myInArc)
      arc.remove(Arc.RM_P);
    for(Arc arc : myOutArc)
      arc.remove(Arc.RM_P);
  }

  /**
   * Odstrani hranu prechodu
   * @param arc Odstranovana hrana
   */
  public void removeArc(Arc arc){
    myInArc.remove(arc);
    myOutArc.remove(arc);
  }

  /**
   * Nastavi podminku prechodu
   * @param guard Podminka prechodu
   */
  public void setGuard (String guard) {
    myGuard = guard;
  }

  /**
   * Otestuje podminku prechodu
   * @return Proveditelnost prechodu
   */
  public abstract boolean testGuard ()throws ExprException;

  /**
   * Prida vyraz do prechodu
   * @param Vyraz
   */
  public void addExpresion (String expr) {
    myExpresion.add(expr);
  }

  /**
   * Provede prechod
   */
  public abstract void execute ()throws ExprException;
}
