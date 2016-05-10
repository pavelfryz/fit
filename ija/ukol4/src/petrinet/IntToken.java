package petrinet;
/**
 * Trida IntToken slouzi pro celociselny
 * token.
 * @author Vojtech Bartl - xbartl03, Pavel Fryz - xfryzp00
 */
public class IntToken implements Token {
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

  public String getValue(){
    return String.valueOf(myValue);
  }

  public void setValue(String value){
    myValue = Integer.parseInt(value);
  }

}
