package client;

import javax.swing.*;
import java.awt.*;
import java.awt.event.*;
import java.util.*;
import java.net.*;
import java.io.*;

/**
 * Trida pro dialog pro pripojeni k serveru.
 * @author Vojtech Bartl - xbartl03, Pavel Fryz - xfryzp00
 */
public class ConnectDialog extends JDialog implements ActionListener{
    /** Panel pro pripojeni k serveru */
    private JPanel connectPanel = new JPanel(new BorderLayout());
    /** Panel pro prihlaseni uzivatele */
    private JPanel loginPanel = new JPanel(new BorderLayout());
    /** Panel pro registraci uzivatele */
    private JPanel registerPanel = new JPanel(new BorderLayout());
    /** Tlacitko pro pripojeni */
    private JButton btnConnect = new JButton("Connect");
    /** Tlacitko pro zruseni pripojeni */
    private JButton btnCancelC = new JButton("Cancel");
    /** Tlacitko pro prihlaseni */
    private JButton btnLogIn = new JButton("Log in");
    /** Tlacitko pro zruseni prihlaseni */
    private JButton btnCancelL = new JButton("Cancel");
    /** Tlacitko pro registraci */
    private JButton btnRegister = new JButton("Register");
    /** Tlacitko pro zruseni registrace */
    private JButton btnCancelR = new JButton("Cancel");
    /** Pole pro login */
    private JTextField login=new JTextField(10);
    /** Pole pro heslo */
    private JPasswordField password=new JPasswordField(10);
    /** Pole pro adresu serveru */
    private JTextField server=new JTextField("localhost",10);
    /** Pole pro port serveru */
    private JTextField port=new JTextField("9354",10);
    /** Pole pro login */
    private JTextField loginR=new JTextField(10);
    /** Pole pro heslo */
    private JPasswordField passwordR1=new JPasswordField(10);
    /** Pole pro heslo */
    private JPasswordField passwordR2=new JPasswordField(10);
    /** Vytvoreny socket */
    private Socket socket;
    /** Vystupni stream k serveru */
    private PrintWriter out = null;
    /** Vstupni stream ze serveru */
    private BufferedReader in = null;
    /** Stav pripojeni*/
    private JLabel status;
    /** Retezec s typem dialogu(registracni nebo prihlasovaci)*/
    private String type;
    /** Stav spojeni*/
    private boolean connected = false;

    /**
     * Vytvori dialog pro pripojeni k serveru
     * @param parent Rodicovska komponenta
     * @param type Typ-registace nebo prohlaseni
     * @param status Status pripojeni
     */
    public ConnectDialog(Component parent,String type,JLabel status){
      setModal(true);
      setTitle("Connect");
      setDefaultCloseOperation(DISPOSE_ON_CLOSE);
      getContentPane().setLayout(new CardLayout());
      getContentPane().add(connectPanel, "");
      getContentPane().add(loginPanel, "");
      getContentPane().add(registerPanel, "");
      createConnectPanel();
      loginPanel.setVisible(false);
      registerPanel.setVisible(false);
      createLoginPanel();
      createRegisterPanel();
      this.status=status;
      this.type=type;
      btnConnect.addActionListener(this);
      btnCancelC.addActionListener(this);
      btnLogIn.addActionListener(this);
      btnCancelL.addActionListener(this);
      btnRegister.addActionListener(this);
      btnCancelR.addActionListener(this);

      pack();
      setLocationRelativeTo(parent);
      setVisible(true);
    }

    /**
     * Vrati jestli je klient pripojen
     * @return Pripojenost klienta
     */
    public boolean getStatus(){
      return connected;
    }

    /**
     * Vrati login uzivatele
     * @return Login
     */
    public String getLogin(){
      return login.getText();
    }

    /**
     * Vrati heslo uzivatele
     * @return Heslo
     */
    public String getPassword(){
      return new String(password.getPassword());
    }

    /**
     * Vrati adresu serveru
     * @return Adresa serveru
     */
    public String getHost(){
      return server.getText();
    }

    /**
     * Vrati port serveru
     * @return Port
     */
    public int getPort(){
      return Integer.parseInt(port.getText());
    }

    /**
     * Ovladani stisku tlacitek
     */
    public void actionPerformed(ActionEvent e){
      if(e.getSource()==btnConnect){
        int iport;
        try{
          iport=Integer.parseInt(port.getText());
        }catch(NumberFormatException nfe){
          JOptionPane.showMessageDialog(this, nfe.getMessage(), "Wrong port number", JOptionPane.ERROR_MESSAGE);
          return;
        }
        try{
          socket = new Socket(server.getText(),iport);
          out = new PrintWriter(socket.getOutputStream(), true);
          in = new BufferedReader(new InputStreamReader(socket.getInputStream()));
          in.readLine();
        }catch(IOException ioe){
          JOptionPane.showMessageDialog(this, ioe.getMessage(), "Could not connect", JOptionPane.ERROR_MESSAGE);
          return;
        }
        if(type.equals("Login."))
          loginPanel.setVisible(true);
        else
          registerPanel.setVisible(true);
        connectPanel.setVisible(false);

      }else if(e.getSource()==btnLogIn){
        try{
          out.println("Login.");
          in.readLine();
          out.println(login.getText());
          in.readLine();
          out.println(password.getPassword());
          if(in.readLine().equals("It is not secret.")){
            JOptionPane.showMessageDialog(this, "Wrong login or password", "Warning", JOptionPane.WARNING_MESSAGE);
          }else{
            status.setText("Connected");
            connected = true;
            out.println("Bye.");
            dispose();
          }
        }catch(Exception ee){}
      }else if(e.getSource()==btnRegister){
        if(loginR.getText().isEmpty()){
          JOptionPane.showMessageDialog(this, "Missing login", "Warning", JOptionPane.WARNING_MESSAGE);
        }else if(new String(passwordR1.getPassword()).length()<5){
          JOptionPane.showMessageDialog(this, "Password too short(at least 5 characters)", "Warning", JOptionPane.WARNING_MESSAGE);
        }else if(!(new String(passwordR1.getPassword()).equals(new String(passwordR2.getPassword())))){
          JOptionPane.showMessageDialog(this, "Differnet passwords", "Warning", JOptionPane.WARNING_MESSAGE);
        }else{
          try{
            out.println("Register.");
            in.readLine();
            out.println(loginR.getText());
            in.readLine();
            out.println(new String(passwordR1.getPassword()));
            if(!in.readLine().equals("OK")){
              JOptionPane.showMessageDialog(this, "Could not register, user already exists", "Warning", JOptionPane.WARNING_MESSAGE);
              return;
            }
            JOptionPane.showMessageDialog(this, "Registered. You can now login","Registration successful",JOptionPane.INFORMATION_MESSAGE);
            out.println("Bye.");
            out.close();
            in.close();
            socket.close();
            dispose();
          }catch(Exception ee){dispose();}
        }
      }else if(e.getSource()==btnCancelC){
        dispose();
      }else{
        try{
          out.println("Bye.");
          out.close();
          in.close();
          socket.close();
        }catch(Exception ee){}
        dispose();
      }
    }


    /**
     * Funkce pro vytvoreni okna pro prihlaseni uzivatele
     */
    public void createConnectPanel(){
      JLabel logLabel=new JLabel("Server:");
      JLabel passLabel=new JLabel("Port:");

      JPanel west = new JPanel(new GridLayout(2,1));
      west.add(logLabel);
      west.add(passLabel);

      JPanel center = new JPanel(new GridLayout(2,1));
      center.add(server);
      center.add(port);

      JPanel south = new JPanel();
      south.add(btnConnect);
      south.add(btnCancelC);

      connectPanel.add(west,BorderLayout.WEST);
      connectPanel.add(center,BorderLayout.CENTER);
      connectPanel.add(south,BorderLayout.SOUTH);
    }

    /**
     * Funkce pro vytvoreni okna pro prihlaseni uzivatele
     */
    public void createLoginPanel(){
      JLabel logLabel=new JLabel("Login:");
      JLabel passLabel=new JLabel("Password:");

      JPanel west = new JPanel(new GridLayout(2,1));
      west.add(logLabel);
      west.add(passLabel);

      JPanel center = new JPanel(new GridLayout(2,1));
      center.add(login);
      center.add(password);

      JPanel south = new JPanel();
      south.add(btnLogIn);
      south.add(btnCancelL);

      loginPanel.add(west,BorderLayout.WEST);
      loginPanel.add(center,BorderLayout.CENTER);
      loginPanel.add(south,BorderLayout.SOUTH);
    }

    /**
     * Funkce pro vytvoreni okna pro prihlaseni uzivatele
     */
    public void createRegisterPanel(){
      JLabel logLabel=new JLabel("Login:");
      JLabel passLabel=new JLabel("Password:");
      JLabel passLabel2 = new JLabel("Repeat password:");

      JPanel west = new JPanel(new GridLayout(3,1));
      west.add(logLabel);
      west.add(passLabel);
      west.add(passLabel2);

      JPanel center = new JPanel(new GridLayout(3,1));
      center.add(loginR);
      center.add(passwordR1);
      center.add(passwordR2);

      JPanel south = new JPanel();
      south.add(btnRegister);
      south.add(btnCancelR);

      registerPanel.add(west,BorderLayout.WEST);
      registerPanel.add(center,BorderLayout.CENTER);
      registerPanel.add(south,BorderLayout.SOUTH);
    }
}
