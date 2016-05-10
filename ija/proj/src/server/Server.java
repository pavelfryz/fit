package server;

import java.net.*;
import java.io.*;

/**
 * Trida Server-obsahuje funkci main, spousti server.
 * @author Vojtech Bartl - xbartl03, Pavel Fryz - xfryzp00
 */
public class Server {
  /**
    * Hlavni funkce serveru
    */
  public static void main(String[] args) throws IOException {
    ServerSocket socket = null;
    boolean listening = true;
    final int PORT = 9354;

    try {
      socket = new ServerSocket(PORT);
    } catch (IOException e) {
      System.err.println("Could not listen on port: "+PORT+".");
      System.exit(-1);
    }

    while (listening)
      new ServerThread(socket.accept()).start();

    socket.close();
  }
}
