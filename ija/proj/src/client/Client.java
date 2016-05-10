package client;

/**
 * Trida Client-obsahuje funkci main, spousti klienta
 * @author Vojtech Bartl - xbartl03, Pavel Fryz - xfryzp00
 */
public class Client{

    /**
     * Hlavni funkce klienta - vytvori a spusti hlavni okno
     */
    public static void main(String[] args){
        javax.swing.SwingUtilities.invokeLater(new Runnable() {
            public void run() {
                GUI.createAndShowGUI();
            }
        });
    }
}