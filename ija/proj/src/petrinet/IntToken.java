package petrinet;

import org.dom4j.Branch;
import org.dom4j.Element;

/**
 * Trida IntToken slouzi pro celociselny
 * token.
 * @author Vojtech Bartl - xbartl03, Pavel Fryz - xfryzp00
 */
public class IntToken implements Token, XMLParse {
  /**
   * Ciselna hodnota tokenu
   */
  protected int myValue;

  /**
   * Vytvori celociselny token
   * @param value Hodnota tokenu
   */
  public IntToken(int value){
    myValue = value;
  }

  /**
   * Vytvori celociselny token
   * @param value Hodnota tokenu
   */
  public IntToken(String value){
    setValue(value);
  }

  /**
   * Vytvori celociselny token z xml Elementu
   * @param element XML element
   */
  public IntToken(Element element){
    super();
    fromXML(element);
  }

  /**
   * Nastavi hodnotu tokenu z XML Elementu
   * @param element XML element
   */
  public void fromXML(Element element){
    setValue(element.attributeValue("value"));
  }

  public String getValue(){
    return String.valueOf(myValue);
  }

  public void setValue(String value){
    myValue = Integer.parseInt(value);
  }

  public void toXML(Branch branch){
    Element token = branch.addElement("IntToken")
      .addAttribute("value",String.valueOf(myValue));
  }
}