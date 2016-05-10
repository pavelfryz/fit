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
  protected ArrayList<String> myExpression;
  /** Proveditelnost prechodu, nastavuje se volanim funkce testGuard */
  protected boolean myExecutable = false;

  /**
   * Vytvori prechod
   */
  public BaseTransition() {
    myInArc = new ArrayList<Arc>();
    myOutArc = new ArrayList<Arc>();
    myGuard = "";
    myExpression = new ArrayList<String>();
  }

  /**
   * Vrati pole vstupnich hran
   * @return Vstupni hrany
   */
  public ArrayList<Arc> getInputArcs(){
    return myInArc;
  }

  /**
   * Vrati pole vystupnich hran
   * @return Vystupni hrany
   */
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
    Arc arc = new Arc(this,place);
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
   * Vrati vyraz na zadane pozici
   * @param position Pozice vyrazu
   * @return Vyraz na zadane pozici
   */
  public String getExpression(int position){
    if(position>=myExpression.size()) return "";
    else return myExpression.get(position);
  }

  /**
   * Vrati strazni vyraz
   * @return Straz
   */
  public String getGuard(){
    return myGuard;
  }

  /**
   * Otestuje podminku prechodu
   * @return Proveditelnost prechodu
   */
  public abstract boolean testGuard ()throws ExprException;

  /**
   * Prida vyraz do prechodu
   * @param expr Vyraz
   */
  public void addExpression (String expr) {
    myExpression.add(expr);
  }

  /**
   * Odstrani vsechny vyrazy
   */
  public void clearExpressios(){
    myExpression.clear();
  }

  /**
   * Vrati proveditelnost prechodu, po zavolani funkce testGuard
   * @return Proveditelnost prechodu
   */
  public boolean executable(){
    return myExecutable;
  }

  /**
   * Provede prechod
   */
  public abstract void execute ()throws ExprException;
}