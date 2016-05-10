package ija.homework2.nums;

import ija.homework2.nums.Num;
import java.lang.String;
import java.lang.Object;

abstract public class AbstractInteger implements Num {
  protected int x;

  public AbstractInteger(int x){
    this.x = x;
  }

  abstract protected int validate(int x);

  abstract public Num plus(Num x);

  public int intValue() {
    return x;
  }

  public String stringValue(){
    return String.valueOf(x);
  }

  public boolean equals(Object obj){
    if (obj instanceof AbstractInteger) {
      AbstractInteger i = (AbstractInteger)obj;
      return x==i.x;
    } else
    return false;
  }

  public int hashCode() {
    return x;
  }
}