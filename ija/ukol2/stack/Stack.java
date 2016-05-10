package ija.homework2.stack;

import ija.homework2.nums.Num;

public class Stack{
  private int size;
  private int iter;
  private Num array[];

  public Stack(int x){
    array = new Num[x];
    size = x;
    iter = -1;
  }

  public boolean push(Num x){
    if (iter != size - 1){
      array[++iter]=x;
      return true;
    }
    return false;
  }

  public boolean pop(){
    if (iter > -1){
      iter--;
      return true;
    }
    return false;
  }

  public boolean contains(Num x){
    return x.equals(array[iter]);
  }
}