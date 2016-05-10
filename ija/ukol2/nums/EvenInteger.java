package ija.homework2.nums;

import ija.homework2.nums.AbstractInteger;

public class EvenInteger extends AbstractInteger {

  public EvenInteger(int x){
    super(x);
    this.x=validate(x);
  }

  protected int validate(int x){
    return (x%2==1)?x+1:x;
  }

  public Num plus(Num x){
    return new EvenInteger(this.x+x.intValue());
  }
}