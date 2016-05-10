package ija.homework1.nums.bounded;

import ija.homework1.nums.Num;

public class OddNum extends Num {
  public OddNum(int x, int min, int max){
    super(x,min,max);
    normalize();
  }

  public OddNum(int x, int max){
    this(x,0,max);
  }

  public OddNum plus(Num num){
    return new OddNum(x+num.getValue(),min,max);
  }

  protected void normalize(){
    super.normalize();
    x+=(x%2==0)?1:0;
  }
}