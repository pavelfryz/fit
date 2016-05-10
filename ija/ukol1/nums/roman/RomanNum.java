package ija.homework1.nums.roman;

import ija.homework1.nums.Num;

import java.lang.String;
import java.lang.Integer;

public class RomanNum {
  private Num x;

  public RomanNum(String x, int min, int max){
    this.x = new Num(Integer.parseInt(x),min,max);
  }

  public RomanNum(String x, int max){
    this(x,0,max);
  }

  public String getValue(){
    return String.valueOf(x.getValue());
  }

  public RomanNum plus(RomanNum num){
    int result = x.getValue()+num.x.getValue();
    return new RomanNum(String.valueOf(result),x.min,x.max);
  }
}