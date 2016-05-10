package petrinet;
import java.util.ArrayList;
import java.util.LinkedList;

/**
 * Trida pro misto petriho site.
 * @author Vojtech Bartl - xbartl03, Pavel Fryz - xfryzp00
 */
public class Place {
  /** Seznam vstupnich hran */
  protected ArrayList<Arc> myInArc;
  /** Seznam vystupnich hran */
  protected ArrayList<Arc> myOutArc;
  /** Seznam tokenu v miste */
  protected LinkedList<Token> myToken;

  /**
   * Vytvori misto
   */
  public Place () {
    myInArc = new ArrayList<Arc>();
    myOutArc = new ArrayList<Arc>();
    myToken = new LinkedList<Token>();
  }

  /**
   * Odstarni misto a vsechny jeho hrany
   */
  public void remove(){
    for(Arc arc : myInArc)
      arc.remove(Arc.RM_T);
    for(Arc arc : myOutArc)
      arc.remove(Arc.RM_T);
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
   * Spoji misto s prechodem
   * @param transition Prechod
   * @return Nova hrana
   */
  public Arc connectTo (BaseTransition transition) {
    Arc arc = new Arc(this,transition);
    transition.addInput(arc);
    myOutArc.add(arc);
    return arc;
  }

  /**
   * Odstrani hranu mista
   * @param arc Odstranovana hrana
   */
  public void removeArc(Arc arc){
    myInArc.remove(arc);
    myOutArc.remove(arc);
  }

  /**
   * Prida token do mista
   * @param token Pridavany token
   */
  public void addToken (Token token) {
    myToken.add(token);
  }

  /**
   * Vrati token z mista
   * null - pokud v miste neni zadny token
   * @return Token
   */
  public Token getToken () {
    return myToken.poll();
  }

  /**
   * Zjisti pocet tokenu v miste
   * @return Pocet tokenu
   */
  public int getTokenCount(){
    return myToken.size();
  }
}
