package petrinet;
import java.util.ArrayList;
import java.util.Iterator;

/**
 * Trida pro petriho sit.
 * @author Vojtech Bartl - xbartl03, Pavel Fryz - xfryzp00
 */
public class PetriNet {
  /** Seznam prechodu site */
  protected ArrayList<BaseTransition> myTransitions;
  /** Seznam hran site */
  protected ArrayList<Arc> myArcs;
  /** Seznam mist site */
  protected ArrayList<Place> myPlaces;

  /**
   * Vytvori petriho sit
   */
  public PetriNet() {
    myTransitions = new ArrayList<BaseTransition>();
    myPlaces = new ArrayList<Place>();
    myArcs = new ArrayList<Arc>();
  }

  public void step () {

  }

  public void run () {

  }

  /**
   * Prida do site misto
   * @param place Misto
   */
  public void add (Place place) {
    myPlaces.add(place);
  }

  /**
   * Prida do site prechod
   * @param transition Prechod
   */
  public void add (BaseTransition transition) {
    myTransitions.add(transition);
  }

  /**
   * Spoji vystup mista se vstupem hrany
   * @param place Misto
   * @param transition Prechod
   * @return Vytvorenou hranu
   */
  public Arc connect (Place place, BaseTransition transition) {
    Arc arc = place.connectTo(transition);
    myArcs.add(arc);
    return arc;
  }

  /**
   * Spoji vystup hrany se vstupem mista
   * @param transition Prechod
   * @param place Misto
   * @return Vytvorenou hranu
   */
  public Arc connect (BaseTransition transition, Place place) {
    Arc arc = transition.connectTo(place);
    myArcs.add(arc);
    return arc;
  }

  /**
   * Odstrani misto a jeho hrany
   * @param place Odstanovane misto
   */
  public void remove(Place place) {
    myPlaces.remove(place);
    Iterator<Arc> iter = myArcs.iterator();
    while(iter.hasNext()){
      Arc a=(Arc) iter.next();
      if(a.removed())
        iter.remove();
    }
  }

  /**
   * Odstrani prechod a jeho hrany
   * @param transition Odstranovany prechod
   */
  public void remove(BaseTransition transition){
    myTransitions.remove(transition);
    Iterator<Arc> iter = myArcs.iterator();
    while(iter.hasNext()){
      Arc a=(Arc) iter.next();
      if(a.removed())
        iter.remove();
    }
  }

  /**
   * Odstrani hranu
   * @param arc Odstranovan hrana
   */
  public void remove(Arc arc){
    myArcs.remove(arc);
    arc.remove(Arc.RM_ALL);
  }

  /**
   * Odstrani vsechny mista, prechody a hrany site.
   */
  public void removeAll(){
    myArcs.clear();
    myPlaces.clear();
    myTransitions.clear();
  }
}
