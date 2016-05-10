package petrinet;

import java.util.ArrayList;
import java.util.Iterator;
import java.util.Collections;
import java.util.Random;

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
  protected ArrayList<Integer> index;

  /**
   * Vytvori petriho sit
   */
  public PetriNet() {
    myTransitions = new ArrayList<BaseTransition>();
    myPlaces = new ArrayList<Place>();
    myArcs = new ArrayList<Arc>();
    index = new ArrayList<Integer>();
  }

  /**
   * Vrati iterator pres prechody site
   * @return Iterator pres prechody
   */
  public Iterator getTransIter(){
    return myTransitions.iterator();
  }

  /**
   * Vrati iterator pres hrany site
   * @return Iterator pres hrany
   */
  public Iterator getArcsIter(){
    return myArcs.iterator();
  }

  /**
   * Vrati iterator pres mista site
   * @return Iterator pres mista
   */
  public Iterator getPlaceIter(){
    return myPlaces.iterator();
  }

  /**
   * Provede simulaci daneho prechodu
   * @param transition Simulovany prechod
   * @return Zda se prechod provedl
   */
  public boolean step (BaseTransition transition) throws ExprException{
    if(transition.executable()){
      transition.execute();
      for(BaseTransition t:myTransitions)
        t.testGuard();
      return true;
    }
    return false;
  }

  /**
   * Provede simulaci site.
   * @param steps Maximalni pocet kroku
   */
  public void run (int steps) throws ExprException{
    Random generator = new Random();
    for(int i=0;i<index.size();i++)
      Collections.swap(index,i,generator.nextInt(index.size()));
    for(BaseTransition t:myTransitions)
      t.testGuard();
    boolean executed = true;
    while(executed&&steps>0){
      executed = false;
      for(Integer i:index){
        if(step(myTransitions.get(i))){
          executed = true;
          steps--;
        }
        if(steps==0) break;
      }
    }

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
    index.add(myTransitions.size());
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

  public ArrayList<Arc> getArcs(Place place, BaseTransition transition){
    ArrayList<Arc> result = new ArrayList<Arc>();
    for(Arc arc:place.getInputArcs()){
      if(arc.getTransition() == transition){
        result.add(arc);
      }
    }
    for(Arc arc:place.getOutputArcs()){
      if(arc.getTransition() == transition){
        result.add(arc);
      }
    }
    return result;
  }

  /**
   * Odstrani misto a jeho hrany
   * @param place Odstanovane misto
   */
  public void remove(Place place) {
    myPlaces.remove(place);
    place.remove();
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
    index.remove(myTransitions.size());
    transition.remove();
    Iterator<Arc> iter = myArcs.iterator();
    while(iter.hasNext()){
      Arc a=(Arc) iter.next();
      if(a.removed())
        iter.remove();
    }
  }

  /**
   * Vrati prechod na zadane pozici
   * @param id Pozice prechodu
   * @return Prechod na pozici
   */
  public BaseTransition getTransition(int id){
    return myTransitions.get(id);
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
    index.clear();
  }
}
