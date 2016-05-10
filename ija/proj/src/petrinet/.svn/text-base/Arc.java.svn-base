package petrinet;

/**
 * Trida Arc slouzi pro spojeni mista
 * a prechodu petriho site.
 * @author Vojtech Bartl - xbartl03, Pavel Fryz - xfryzp00
 */
public class Arc {
  /** Spojovane misto */
  protected Place myPlace;
  /** Spojovany prechod */
  protected BaseTransition myTransition;
  /** Nazev hrany */
  protected String myLabel = "";
  /** Token prochazejici pres hranu */
  protected Token myToken = null;
  /** Orientace hrany */
  protected boolean myOrientation;
  /** Z mista do prechodu */
  public static final boolean P2T = true;
  /** Z prechodu do mista */
  public static final boolean T2P = false;
  /** Oznaceni odstranene hrany */
  protected boolean myRemoved = false;
  /** Odstrani hranu z mista i prechodu */
  public static final int RM_ALL = 0;
  /** Odstrani hranu pouze z mista */
  public static final int RM_P = 1;
  /** Odstrani hranu pouze z prechodu */
  public static final int RM_T = 2;

  /**
   * Vytvori novou hranu spojujici zadane misto
   * a prechod smerem k prechodu
   * @param place Spojovane misto
   * @param transition Spojovany prechod
   */
  public Arc (Place place,BaseTransition transition){
    myPlace = place;
    myOrientation = P2T;
    myTransition = transition;
  }

  /**
   * Vytvori novou hranu spojujici zadane misto
   * a prechod smerem k mistu
   * @param place Spojovane misto
   * @param transition Spojovany prechod
   */
  public Arc (BaseTransition transition,Place place){
    myPlace = place;
    myOrientation = T2P;
    myTransition = transition;
  }

  /**
   * Odstrani hranu z mista i prechodu
   */
  public void remove(){remove(RM_ALL);}

  /**
   * Odstrani hranu z mista nebo prechodu
   * podle zadanaho parametru
   * @param type Typ odstraneni
   */
  public void remove(int type){
    if(type == RM_ALL){
      myPlace.removeArc(this);
      myTransition.removeArc(this);
    } else if(type == RM_T){
      myTransition.removeArc(this);
    } else {
      myPlace.removeArc(this);
    }
    myRemoved = true;
  }

  /**
   * Vrati spojovane misto
   * @return Spojovane misto
   */
  public Place getPlace(){
    return myPlace;
  }

  /**
   * Vrati spojovany prechod
   * @return Spojovany prechod
   */
  public BaseTransition getTransition(){
    return myTransition;
  }

  /**
   * Zjisti jestli hrana byla odstranena
   */
  public boolean removed(){
    return myRemoved;
  }

  /**
   * Nastavi nazev hrany
   * @param label Nazev hrany
   */
  public void setLabel(String label){
    myLabel = label;
  }

  /**
   * Zjisti nazev hrany
   * @return Nazev hrany
   */
  public String getLabel(){
    return myLabel;
  }

  /**
   * Zjisti pocet tokenu v napojenem miste
   * @return Pocet tokenu v miste
   */
  public int getTokenCount(){
    return myPlace.getTokenCount();
  }

  /**
   * Vrati token prochazejici pres hranu
   * @return Prochazejici token
   */
  public Token getToken(){
    return myToken;
  }

  /**
   * Nastavi token, prochazejici hranou
   * @param token Token
   */
  public void setToken(Token token){
    myToken = token;
  }

  /**
   * Nacte token z mista.
   * @return Nacteny token
   */
  public Token readToken(){
    return myToken = myPlace.getToken();
  }

  /**
   * Posle token z hrany do mista
   */
  public void sendToken(){
    if(myToken != null){
      myPlace.addToken(myToken);
      myToken = null;
    }
  }
}