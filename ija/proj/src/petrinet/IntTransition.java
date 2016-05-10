package petrinet;

import java.util.StringTokenizer;

/**
 * Trida IntTransiton slouzi pro prechod
 * pracujici pouze s celociselnymi tokeny.
 * @author Vojtech Bartl - xbartl03, Pavel Fryz - xfryzp00
 */
public class IntTransition extends BaseTransition {

  public IntTransition() {}

  public boolean testGuard () throws ExprException {
    boolean result = testGuard(0);
    for(Arc arc:myInArc)
      arc.sendToken();
    myExecutable = result;
    return result;
  }

  /**
   * Zjisti proveditelnost prechodu.
   * Otestuje strazni podminku se vsemi tokeny
   * pomoci rekurzivniho backtrackingu
   * @param uroven rekurze
   * @return Proveditelnost prechodu
   */
  private boolean testGuard(int arcId) throws ExprException {
    // koncova podminka rekurze
    if(arcId == myInArc.size())
      return test();
    Arc arc = myInArc.get(arcId);
    for(int i=0;i<arc.getTokenCount();++i){
      arc.readToken(); // navazani tokenu
      try{
        int x = Integer.parseInt(arc.getLabel());
        if(x!=Integer.parseInt(arc.getToken().getValue())){
          arc.sendToken();
          continue;
        }
      }catch(NumberFormatException e){}
      // test strazni podminky
      if(testGuard(arcId+1))
        return true;
      arc.sendToken(); // odvazani tokenu
    }
    return false;
  }

  /**
   * Otestuje jestli navazane tokeny vyhovuji
   * strazni podmince
   * @return Vyhodnoceni straze
   */
  private boolean test() throws ExprException
  {
    if(myGuard=="")
      return true;
    String []expr = myGuard.replaceAll("\\s","").split("&",-1);
    for(String e:expr){
      StringTokenizer st = new StringTokenizer(e,"<>=!",true);
      String token;
      int op1=0;
      int op2=0;
      final int NOOP = 0;
      final int OP = 1;
      final int LT = 2;
      final int GT = 3;
      final int EQ = 4;
      final int LTEQ = 5;
      final int GTEQ = 6;
      final int NOT = 7;
      final int NEQ = 8;
      final int EQEQ = 9;
      final int EOK = 10;
      int state = NOOP;
      while(st.hasMoreTokens()){
        token=st.nextToken();
        if(token.equals("<")){
          if(state == OP) state = LT;
          else throw new ExprException("Missing operand: "+myGuard);
        }else if(token.equals(">")){
          if(state == OP) state = GT;
          else throw new ExprException("Missing operand: "+myGuard);
        }else if(token.equals("=")){
          if(state == OP)      state = EQ;
          else if(state == EQ) state = EQEQ;
          else if(state == LT) state = LTEQ;
          else if(state == GT) state = GTEQ;
          else if(state == NOT)state = NEQ;
          else throw new ExprException("Missing operand: "+myGuard);
        }else if(token.equals("!")){
          if(state == OP) state = NOT;
          else throw new ExprException("Missing operand: "+myGuard);
        }else{
          if(state==NOOP){
            op1=getValueOf(token);
            state=OP;
          }else if(state==EQ||state==NOT)
            throw new ExprException("Missing '=': "+myGuard);
          else{
            op2=getValueOf(token);
            if(state==LTEQ)     { if(op1 >  op2) return false;}
            else if(state==LT)  { if(op1 >= op2) return false;}
            else if(state==GTEQ){ if(op1 <  op2) return false;}
            else if(state==GT)  { if(op1 <= op2) return false;}
            else if(state==EQEQ){ if(op1 != op2) return false;}
            else if(state==NEQ) { if(op1 == op2) return false;}
            state = EOK;
          }
        }
      }
      if(state!=EOK) throw new ExprException("Wrong Guard expression: "+myGuard);
    }
    return true;
  }

  /**
   * Provede prechod, vyhodnoti podminku prechodu,
   * provede vyrazy prechodu a posle vysledek na vystupni hrany
   */
  public void execute () throws ExprException{
    if(!testGuard(0)) return;
    for(Arc outArc:myOutArc){
      try{
        int i=Integer.parseInt(outArc.getLabel());
        outArc.setToken(new IntToken(i));
      }catch(NumberFormatException ex){
        boolean br = false;
        for(String expr:myExpression){
          String []eparts = expr.replaceAll("\\s","").split("=",-1);
          if(eparts.length!=2) throw new ExprException("Wrong expression: "+expr);
          if(eparts[0].equals(outArc.getLabel())){
            int i=eval(eparts[1]);
            outArc.setToken(new IntToken(i));
            br=true;
            break;
          }
        }
        if(br) continue;
        for(Arc inArc:myInArc){
          if(inArc.getLabel().equals(outArc.getLabel())){
            outArc.setToken(new IntToken(inArc.getToken().getValue()));
            br=true;
            break;
          }
        }
        if(br) continue;
        throw new ExprException("Undefined value of Arc: "+outArc.getLabel());
      }
    }
    for(Arc inArc:myInArc) inArc.setToken(null);
    for(Arc outArc:myOutArc) outArc.sendToken();
  }

  /**
   * Vyhodnoti vyraz
   * @param expr Vyraz
   * @return Hodnota vyrazu
   */
  private int eval(String expr) throws ExprException{
    StringTokenizer st=new  StringTokenizer(expr,"+-",true);
    String token;
    int result = 0;
    final int NOOP = 0;
    final int MINUS = 1;
    final int PLUS = 2;
    int operator = NOOP;
    while(st.hasMoreTokens()){
      token = st.nextToken();
      if(token.equals("-")){
        if(operator!=NOOP)
          break;
        operator=MINUS;
      }else if(token.equals("+")){
        if(operator!=NOOP)
          break;
        operator=PLUS;
      }else{
        int operand = getValueOf(token);
        if(operator==NOOP||operator==PLUS)
          result+=operand;
        else
          result-=operand;
        operator=NOOP;
      }
    }
    if(operator!=NOOP)
      throw new ExprException("Missing operand");
    return result;
  }

  /**
   * Prevede retezec na cislo, pokud retezec neni
   * cislo pokusi se ziskat hodnotu ze vstupnich hran
   * s danym jmenem
   * @param s Retezec(Jmeno hrany)
   * @return Hodnota retezce
   */
  private int getValueOf(String s) throws ExprException{
    try{
      int i=Integer.parseInt(s);
      return i;
    }catch(NumberFormatException ex){
      for(Arc arc:myInArc){
        if(s.equals(arc.getLabel())){
          return Integer.parseInt(arc.getToken().getValue());
        }
      }
    }
    throw new ExprException("Undefined operand: "+s);
  }

}

