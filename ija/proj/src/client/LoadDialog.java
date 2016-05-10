package client;

import javax.swing.*;
import java.awt.*;
import java.awt.event.*;
import java.util.*;
import java.net.*;
import java.io.*;
import java.lang.Integer;

import org.dom4j.Document;
import org.dom4j.DocumentHelper;
import org.dom4j.Element;

/**
 * Trida pro dialog pro prohlizeni a nacitani siti ze serveru.
 * @author Vojtech Bartl - xbartl03, Pavel Fryz - xfryzp00
 */
public class LoadDialog extends JDialog implements ActionListener{
    private JLabel description = new JLabel("");
    private JLabel author = new JLabel("");
    private Document document;
    private JComboBox box= new JComboBox();
    private JComboBox versions= new JComboBox();
    private String wanted = "None";
    private JButton open = new JButton("open");
    private JButton simulations = new JButton("simulations");
    private JButton find = new JButton("find");
    private JButton close = new JButton("close");

    /**
     * Vytvori dialog pro pripojeni k serveru
     * @param parent Rodicovska komponenta
     * @param type Typ-registace nebo prohlaseni
     * @param status Status pripojeni
     */
    public LoadDialog(Component parent,String input){
      setModal(true);
      setTitle("Load net");
      try{
        document=DocumentHelper.parseText(input);
      }catch(Exception e){}

      Element root = document.getRootElement();
      Iterator i=root.elementIterator("Net");
      while(i.hasNext()){
        Element net = (Element)i.next();
        box.addItem(net.elementText("name"));
      }

      setDefaultCloseOperation(DISPOSE_ON_CLOSE);

      getContentPane().setLayout(new GridLayout(3,4));
      getContentPane().add(new JLabel("Net"));
      getContentPane().add(new JLabel("Description"));
      getContentPane().add(new JLabel("Author"));
      getContentPane().add(new JLabel("Version"));
      getContentPane().add(box);
      getContentPane().add(description);
      getContentPane().add(author);
      getContentPane().add(versions);
      getContentPane().add(open);
      getContentPane().add(find);
      getContentPane().add(simulations);
      getContentPane().add(close);

      updateVersion();

      box.addActionListener(this);
      versions.addActionListener(this);
      open.addActionListener(this);
      close.addActionListener(this);
      find.addActionListener(this);
      simulations.addActionListener(this);


      pack();
      setLocationRelativeTo(parent);
      setVisible(true);
    }

    /**
     * Aktualizuje pocet verzi u zvolene site
     */
    private void updateVersion(){
      String selected=(String)box.getSelectedItem();
      versions.removeAllItems();
      if((selected)==null){
        description.setText("");
        author.setText("");
        return;
      }
      Element root = document.getRootElement();
      Iterator i=root.elementIterator("Net");
      while(i.hasNext()){
        Element net = (Element)i.next();
        if(selected.equals(net.elementText("name"))){
          Iterator it=net.elementIterator("version");
          Integer max = new Integer(0);
          while(it.hasNext()){
            Integer version = new Integer(((Element)it.next()).elementText("id"));
            versions.addItem(version);
            if(version.compareTo(max)>0) max=version;
          }
          versions.setSelectedItem(max);
          updateAuthorAndDescription();
          return;
        }
      }
    }

    /**
     * Aktualizuje jmeno autora a popis site podle
     * vybrane verze a site
     */
    void updateAuthorAndDescription(){
      String selected=(String)box.getSelectedItem();
      Integer version=(Integer)versions.getSelectedItem();
      if(version==null) return;
      Element root = document.getRootElement();
      Iterator i=root.elementIterator("Net");
      while(i.hasNext()){
        Element net = (Element)i.next();
        if(selected.equals(net.elementText("name"))){
          Iterator it=net.elementIterator("version");
          while(it.hasNext()){
            Element el=(Element)it.next();
            if(new Integer(el.elementText("id")).equals(version)){
              author.setText(el.elementText("user"));
              description.setText(el.elementText("description"));
              return;
            }
          }
        }
      }
    }

    public String output(){
      return wanted;
    }

    /**
     * Ovladani stisku tlacitek
     */
    public void actionPerformed(ActionEvent e){
      if(e.getSource()==box){
        updateVersion();
      }
      else if(e.getSource()==versions){
        updateAuthorAndDescription();
      }
      else if(e.getSource()==open){
        wanted = "<net><name>"+(String)box.getSelectedItem()+"</name><version>"+
        versions.getSelectedItem().toString()+"</version></net>";
        dispose();
      }else if(e.getSource()==simulations){
        Vector <String>data = new Vector<String>();
        String selected=(String)box.getSelectedItem();

        Element root = document.getRootElement();
        Iterator i=root.elementIterator("Net");
        while(i.hasNext()){
          Element net = (Element)i.next();
          if(selected.equals(net.elementText("name"))){
            Iterator it=net.elementIterator("Simulation");
            while(it.hasNext()){
              Element el=(Element)it.next();
              data.add(new String(el.elementText("user")+": "+el.elementText("date")));
            }
            break;
          }
        }

        JList sim = new JList(data);
        class SimDialog extends JDialog{
          public SimDialog(Component parent,JList list){
            setModal(true);
            setTitle("Net simulations");
            setDefaultCloseOperation(DISPOSE_ON_CLOSE);
            JScrollPane pane = new JScrollPane(list);
            setSize(300,400);
            getContentPane().setLayout(new BorderLayout());
            getContentPane().add(new JLabel("Simulations"),BorderLayout.NORTH);
            getContentPane().add(pane,BorderLayout.CENTER);
            pack();
            setLocationRelativeTo(parent);
            setVisible(true);
          }
        }
        new SimDialog(this,sim);
      }else if(e.getSource()==close){
        dispose();
      }else if(e.getSource()==find){
        class FindDialog extends JDialog implements ActionListener{
          private JTextField name=new JTextField(10);
          public FindDialog(Component parent){
            setModal(true);
            setTitle("Find");
            setDefaultCloseOperation(DISPOSE_ON_CLOSE);
            getContentPane().setLayout(new BorderLayout());
            getContentPane().add(new JLabel("Write part of nets name:"),BorderLayout.NORTH);
            getContentPane().add(name,BorderLayout.CENTER);
            JButton ok = new JButton("Ok");
            ok.addActionListener(this);
            getContentPane().add(ok,BorderLayout.SOUTH);
            pack();
            setLocationRelativeTo(parent);
            setVisible(true);
          }
          public void actionPerformed(ActionEvent e){
            dispose();
          }
          public String getName(){
            return name.getText();
          }
        }
        FindDialog d = new FindDialog(this);
        String name=d.getName();

        box.removeAllItems();
        Element root = document.getRootElement();
        Iterator i=root.elementIterator("Net");
        while(i.hasNext()){
          Element net = (Element)i.next();
          if(net.elementText("name").toLowerCase().contains(name.toLowerCase()))
            box.addItem(net.elementText("name"));
        }
        updateVersion();

      }
    }
}