package server;

import java.net.*;
import java.io.*;

/**
 * Trida ServerThread slouzi pro obsluhu jednoho klienta.
 * @author Vojtech Bartl - xbartl03, Pavel Fryz - xfryzp00
 */
public class ServerThread extends Thread {
  /** Socket spojujici klienta a server */
  private Socket socket = null;

  /**
   * Vytvori nove vlakno
   */
  public ServerThread(Socket socket) {
    super();
    this.socket = socket;
  }

  /**
   * Obsluhuje klienta
   */
  public void run() {
    try {
      PrintWriter out = new PrintWriter(socket.getOutputStream(), true);
      BufferedReader in = new BufferedReader(
      new InputStreamReader(socket.getInputStream()));

      String inputLine, outputLine;
      Protocol p = new Protocol();
      outputLine = p.processInput(null);
      out.println(outputLine);

      while ((inputLine = in.readLine()) != null) {
        outputLine = p.processInput(inputLine);
        out.println(outputLine);

        if (outputLine.equals("Bye."))
          break;
      }
      out.close();
      in.close();
      socket.close();

    } catch (IOException e) {
      e.printStackTrace();
    }
  }
}
