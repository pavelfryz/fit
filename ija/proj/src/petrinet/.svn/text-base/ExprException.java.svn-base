package petrinet;

/**
 * Trida pro vyjimku, ktera muze nastat
 * pro zpracovani vyrazu a straze prechodu
 * @author Vojtech Bartl - xbartl03, Pavel Fryz - xfryzp00
 */
public class ExprException extends Exception
{
  /** Chybova zprava */
  String errmsg;

  /** Vytvori vyjimku */
  public ExprException()
  {
    super();
    errmsg = "unknown";
  }

  /**
   * Vytvori vyjimku
   * @param err Chybova zprava
   */
  public ExprException(String err)
  {
    super("Error: "+err);
    errmsg = "Error: "+err;
  }

  /**
   * Vrati chybovou zpravu
   * @return Chybova zprava
   */
  public String getError()
  {
    return errmsg;
  }
}
