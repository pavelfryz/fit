package client;
import java.awt.*;

/**
 * Rozhrani komponent umistovanych
 * do grafu petriho site
 */
public interface GComponent {
  /** Tolerance u kliknuti misi */
  static final int HIT_BOX = 4;

  /**
   * Vykresli komponentu
   */
  void draw(Graphics g);

  /**
   * Zjisti prunik komponenty s bodem
   * @param x Souradnice na ose x
   * @param y Souradnice na ose y
   * @return Jestli je bod uvnitr komponenty
   */
  boolean intersects(int x, int y);

  /**
   * Posune danou komponentu
   * na nove souradnice
   * @param x Souradnice na ose x
   * @param y Souradnice na ose y
   */
  void move(int x,int y);
}
