package client;

import javax.swing.*;
import java.awt.*;
import java.awt.event.*;
import java.util.*;
import java.awt.geom.Line2D;
import java.io.File;
import petrinet.*;
import javax.swing.event.*;

/**
 * Trida implementujici GUI
 *
 */
public class GUI{

    protected JButton buttonTrans;  //tlacitka
    protected JButton buttonPlace;
    protected JButton buttonRemove;
    protected JButton buttonMove;
    protected JFrame frame;
    protected GPanel mainWindow;
    protected JPanel addingP;
    protected JPanel addingG;
    protected JPanel addingT;
    protected JPanel addingA;
    protected JTextArea tAreaP;
    protected JTextArea tAreaG;
    protected JTextArea tAreaT;
    protected JTextArea tAreaA;
    protected Color buttonPressColor;

    private final int FRAME_W=800;
    private final int FRAME_H=500;


    /**
     * Konstruktor inicializuje promenne
     */
    public GUI(){
      buttonPressColor=new Color(238, 154, 0);

    }

    /**
     * Funkce pro vytvoreni a vykresleni GUI
     */
    public void createGUI(){
        frame=new JFrame("Petri-NET Simulator");     //frame
        frame.setBounds(550, 100, FRAME_W, FRAME_H);
        frame.setLayout(null);
        frame.setBackground(Color.white);
        createMenu(frame);
        JPanel options=new JPanel(new GridLayout(1,4));     //panel pro tlacitka
        options.setBounds(0, 0, (int)(FRAME_W*0.6), 50);
        options.setBackground(Color.white);
        createButtons();
        options.add(buttonTrans);
        options.add(buttonPlace);
        options.add(buttonRemove);
        options.add(buttonMove);
        frame.add(options);
        mainWindow=new GPanel();
        mainWindow.setLayout(null);
        mainWindow.setBounds(0, 50, FRAME_W, FRAME_H-50);      //hlavni panel pro PN
        mainWindow.setBackground(Color.white);
        frame.add(mainWindow);
        frame.setResizable(false);
        frame.setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);
        frame.setVisible(true);
        }


    /**
     * Funkce pro vytvoreni menu
     * @param Hlavni okno programu do ktereho se menu vklada
     */
    protected void createMenu(JFrame frame){
        JMenuBar menu;
        JMenu file, edit, style;
        JMenuItem fileMenu, editMenu, styleMenu;
        menu=new JMenuBar();
        file=new JMenu("File");
        menu.add(file);
        edit=new JMenu("Edit");
        menu.add(edit);
        style=new JMenu("Style");
        menu.add(style);
        fileMenu=new JMenuItem("Open");
        fileMenu.addActionListener(new menuActions());
        file.add(fileMenu);
        fileMenu=new JMenuItem("Save");
        fileMenu.addActionListener(new menuActions());
        file.add(fileMenu);
        file.addSeparator();
        fileMenu=new JMenuItem("Exit");
        fileMenu.addActionListener(new menuActions());
        file.add(fileMenu);
        editMenu=new JMenuItem("Clear");
        editMenu.addActionListener(new menuActions());
        edit.add(editMenu);
        styleMenu=new JMenuItem("puvodni");
        styleMenu.addActionListener(new menuActions());
        style.add(styleMenu);
        styleMenu=new JMenuItem("dalsi");
        styleMenu.addActionListener(new menuActions());
        style.add(styleMenu);
        frame.setJMenuBar(menu);
        }


    /**
     * Funkce pro vytvoreni tlacitek pro nabidku
     */
    protected void createButtons(){
        //prechody
        buttonTrans=new JButton("Transition");
        buttonTrans.setBackground(Color.white);
        buttonTrans.addActionListener(new buttonPress());
        //mista
        buttonPlace=new JButton("Place");
        buttonPlace.setBackground(Color.white);
        buttonPlace.addActionListener(new buttonPress());
        //mazani
        buttonRemove=new JButton("Remove");
        buttonRemove.setBackground(Color.white);
        buttonRemove.addActionListener(new buttonPress());
        //posuvani
        buttonMove=new JButton("Move");
        buttonMove.setBackground(Color.white);
        buttonMove.addActionListener(new buttonPress());
        }




    /**
     * Trida obsluhujici akce v menu
     */
    public class menuActions implements ActionListener{
        public void actionPerformed(ActionEvent e){
            if((((JMenuItem)(e.getSource())).getText()).equals("Open")){
                JFileChooser fc=new JFileChooser(".");
                fc.showOpenDialog(frame);
                File openFile=fc.getSelectedFile();
                System.out.println("je tu open s "+openFile.getName());
            }
            else if((((JMenuItem)(e.getSource())).getText()).equals("Save")){
                JFileChooser fc=new JFileChooser(".");
                fc.showSaveDialog(frame);
                File saveFile=fc.getSelectedFile();
                System.out.println("je tu save s "+saveFile.getName());
            }
            else if((((JMenuItem)(e.getSource())).getText()).equals("Exit")){
                System.exit(0);
            }
            else if((((JMenuItem)(e.getSource())).getText()).equals("Clear")){
                //vymazani seznamu a prekresleni prazdneho okna

                mainWindow.clear();
                frame.repaint();

            }
            else if((((JMenuItem)(e.getSource())).getText()).equals("puvodni")){
                System.out.println("je tu puvodni");
            }
            else if((((JMenuItem)(e.getSource())).getText()).equals("dalsi")){
                System.out.println("je tu dalsi");
            }
        }
    }

    /**
     * Trida obsluhujici stisk tlacitek
     */
    public class buttonPress implements ActionListener{
        public void actionPerformed(ActionEvent e){
            //zjisteni, ktere tlacitko bylo stisknuto-nastaveni modu a podbarveni tlacitka
            if((((JButton)(e.getSource())).getText()).equals("Transition")){
                mainWindow.setMode(GPanel.ADD_TRANSITION);
                buttonTrans.setBackground(buttonPressColor);
                }
            else if((((JButton)(e.getSource())).getText()).equals("Place")){
                mainWindow.setMode(GPanel.ADD_PLACE);
                buttonPlace.setBackground(buttonPressColor);
                }
            else if((((JButton)(e.getSource())).getText()).equals("Remove")){
                mainWindow.setMode(GPanel.REMOVE);
                buttonRemove.setBackground(buttonPressColor);
                }
            else if((((JButton)(e.getSource())).getText()).equals("Move")){
                mainWindow.setMode(GPanel.MOVE);
                buttonMove.setBackground(buttonPressColor);
                }

            }
        }

    /**
     * Funkce nastavi barvu vsech tlacitek na puvodni
     */
    private void clearButtons(){
        buttonPlace.setBackground(Color.white);
        buttonTrans.setBackground(Color.white);
        buttonRemove.setBackground(Color.white);
        buttonMove.setBackground(Color.white);
        }
}



