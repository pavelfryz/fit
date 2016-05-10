package ija.homework2.nums;

import ija.homework2.nums.EvenInteger;
import ija.homework2.nums.Num;
import java.lang.String;
import java.lang.Integer;

public class RomanEvenInteger implements Num {
  private EvenInteger x;

  public RomanEvenInteger(String x){
    this.x = new EvenInteger(Integer.parseInt(x));
  }

  private RomanEvenInteger(int x){
    this.x = new EvenInteger(x);
  }

  public Num plus(Num x){
    return new RomanEvenInteger(this.x.intValue()+x.intValue());
  }

  public int intValue(){
    return x.intValue();
  }

  public String stringValue(){
    return x.stringValue();
  }

  public boolean equals(Object obj){
    if (obj instanceof RomanEvenInteger) {
      RomanEvenInteger i = (RomanEvenInteger)obj;
      return x.equals(i.x);
    } else
    return false;
  }

  public int hashCode() {
    return x.hashCode();
  }
}