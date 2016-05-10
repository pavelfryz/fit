package client;

import petrinet.*;

import org.dom4j.Branch;
import org.dom4j.Element;
import java.util.Iterator;

/**
 * Trida GNet slouzi pro praci s grafickymi
 * komponentami petriho site.
 * @author Vojtech Bartl - xbartl03, Pavel Fryz - xfryzp00
 */
public class GNet extends PetriNet implements XMLParse {
  /** Popis site */
  public String description="";
  /** Jmeno site */
  public String name="";

  public GNet() {
    super();
  }

  /**
   * Vytvori sit z XML elementu
   * @param element XML element
   */
  public GNet(Element element){
    this();
    fromXML(element);
  }

  /**
   * Nastavi sit z XML elementu
   * @param element XML element
   */
  public void fromXML(Element element){
    Iterator i=element.elementIterator("GTransition");
    while(i.hasNext())
      add(new GTransition((Element)i.next()));
    i=element.elementIterator("GPlace");
    while(i.hasNext())
      add(new GPlace((Element)i.next()));

    i=element.elementIterator("GArc");
    while(i.hasNext()){
      Element arc = (Element)i.next();
      int placeId = Integer.parseInt(arc.element("place").attributeValue("id"));
      int transId = Integer.parseInt(arc.element("transition").attributeValue("id"));
      String label = arc.elementText("label");
      if("toPlace".equals(arc.attributeValue("orientation")))
        connect(myTransitions.get(transId),myPlaces.get(placeId)).setLabel(label);
      else
        connect(myPlaces.get(placeId),myTransitions.get(transId)).setLabel(label);
    }
    description=element.elementText("description");
    name=element.elementText("name");
    if(name==null) name="";
    if(description==null) description="";
  }

  public void toXML(Branch branch){
    Element net = branch.addElement("GNet");
    net.addElement("name").addText(name);
    net.addElement("description").addText(description);
    for(int i=0;i<myTransitions.size();i++){
      ((GTransition)myTransitions.get(i)).setId(i);
      ((GTransition)myTransitions.get(i)).toXML(net);
    }
    for(int i=0;i<myPlaces.size();i++){
      ((GPlace)myPlaces.get(i)).setId(i);
      ((GPlace)myPlaces.get(i)).toXML(net);
    }
    for(Arc a:myArcs)
      ((GArc)a).toXML(net);
  }
}