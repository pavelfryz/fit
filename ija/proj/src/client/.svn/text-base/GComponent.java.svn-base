package client;

import java.awt.Graphics;
import java.awt.Color;

/**
 * Rozhrani komponent umistovanych
 * do grafu petriho site
 * @author Vojtech Bartl - xbartl03, Pavel Fryz - xfryzp00
 */
public interface GComponent {
  /** Tolerance u kliknuti misi */
  static final int HIT_BOX = 10;

  /**
   * Zmeni stav komponenty
   */
  void changeSelected();

  /**
   * Aktualizuje grafiku komponenty
   */
  void updateGraphics();

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