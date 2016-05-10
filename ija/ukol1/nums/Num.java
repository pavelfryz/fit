package ija.homework1.nums;

public class Num {
  protected int x;
  public final int min;
  public final int max;

  public Num(int x, int min, int max){
    this.min = min;
    this.max = max;
    this.x = x;
    normalize();
  }

  public Num(int x, int max){
    this(x,0,max);
  }

  public int getValue(){
    return x;
  }

  public Num plus(Num num){
    return new Num(x+num.x,min,max);
  }

  protected void normalize(){
    x=(x-min)%(max-min+1)+min+((min>x)?(max-min+1):0);
  }
}