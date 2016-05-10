package petrinet;

class Test{
  public static void main(String []argv) throws ExprException{
    PetriNet net = new PetriNet();
    Place p = new Place();
    Arc a;
    net.add(p);
    BaseTransition t=new IntTransition();
    net.add(t);
    a=net.connect(p,t);
    a.setLabel("x");
    p.addToken(new IntToken(5));
    p.addToken(new IntToken(6));
    p.addToken(new IntToken(7));
    p.addToken(new IntToken(22));
    p=new Place();
    net.add(p);
    a=net.connect(p,t);
    a.setLabel("y");
    p.addToken(new IntToken(12));
    p.addToken(new IntToken(44));
    p.addToken(new IntToken(22));
    p.addToken(new IntToken(20));
    t.setGuard("x!=y&x==7&y>12&y<=22");
    if(t.testGuard())
      System.out.println("OK");
    p=new Place();
    net.add(p);
    a=net.connect(t,p);
    a.setLabel("a");
    t.addExpresion("       a  =  x+y ");
    t.execute();
    a.sendToken();
    Token tok=p.getToken();
    System.out.println("Token: "+tok.getValue());

  }
}
