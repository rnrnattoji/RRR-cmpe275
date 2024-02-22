package gash.socket;

import gash.payload.BasicBuilder;
import gash.payload.Message;
import java.io.BufferedInputStream;
import java.io.ByteArrayOutputStream;
// import java.io.InterruptedIOException;
import java.net.Socket;
import java.util.Arrays;

/**
 *
 * @author gash
 *
 */
class SessionHandler extends Thread {

  private static int sessionCount = 0;
  private final int sessionId;
  private Socket connection;
  // private String name;
  private boolean forever = true;

  public SessionHandler(Socket connection) {
    this.connection = connection;

    // allow server to exit if
    this.setDaemon(true);

    synchronized (SessionHandler.class) {
      sessionCount++;
      sessionId = sessionCount;
    }
  }

  /**
   * stops session on next timeout cycle
   */
  public void stopSession() {
    forever = false;
    if (connection != null) {
      try {
        connection.close();
      } catch (Exception e) {
        e.printStackTrace();
      }
    }
    connection = null;
  }

  /**
   * process incoming data
   */
  public void run() {
    System.out.println("Session " + sessionId + " started\n");
    try {
      connection.setSoTimeout(0);
      BufferedInputStream in = new BufferedInputStream(
        connection.getInputStream()
      );

      while (forever) {
        ByteArrayOutputStream buf = new ByteArrayOutputStream();
        boolean messageComplete = false;

        while (!messageComplete) {
          byte[] dataChunk = new byte[1024]; 
          int bytesRead = in.read(dataChunk, 0, dataChunk.length); 

          if (bytesRead == -1) { 
            forever = false;
            break;
          }

          String chunkAsString = new String(dataChunk, 0, bytesRead, "UTF-8");
          if (chunkAsString.replace("0000,", "").replace("\0", "").isEmpty()) {
              continue; 
          }

          boolean isNullChunk = true;
          for (int i = 0; i < bytesRead; i++) {
            if (dataChunk[i] != 0) {
              isNullChunk = false;
              break;
            }
          }

          if (isNullChunk) {
            continue; 
          }

          if (bytesRead > 0 && dataChunk[bytesRead - 1] == 0) {
            messageComplete = true; 
            bytesRead--;
          }

          buf.write(dataChunk, 0, bytesRead);
        }

        byte[] messageBytes = buf.toByteArray();
        if (messageBytes.length > 0) {
          BasicBuilder builder = new BasicBuilder();
          Message msg = builder.decode(messageBytes);
          if (msg != null) {
            String modifiedName =
              msg.getName() + " (Session " + sessionId + ")";
            String modifiedMsg =
              "from " +
              modifiedName +
              ", to group: " +
              msg.getGroup() +
              ", text: " +
              msg.getText();
            System.out.println(modifiedMsg + "\n");
          }
        }
      }
    } catch (Exception e) {
      e.printStackTrace();
    } finally {
      try {
        System.out.println("Session " + sessionId + " ending");
        System.out.flush();
        stopSession();
      } catch (Exception re) {
        re.printStackTrace();
      }
    }
  }
} // class SessionHandler
