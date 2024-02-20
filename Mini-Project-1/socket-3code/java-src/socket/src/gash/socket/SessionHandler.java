package gash.socket;

import java.io.BufferedInputStream;
import java.io.ByteArrayOutputStream;
// import java.io.InterruptedIOException;
import java.net.Socket;

import gash.payload.BasicBuilder;
import gash.payload.Message;


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
			var in = new BufferedInputStream(connection.getInputStream());

			if (in == null)
				throw new RuntimeException("Unable to get input streams");

			BasicBuilder builder = new BasicBuilder();
			while (forever) {
				ByteArrayOutputStream buf = new ByteArrayOutputStream();
				int nextByte;

				while ((nextByte = in.read())!= -1) {
					if (nextByte == 0) {
						break;
					}
					buf.write(nextByte);
				}

				if(nextByte == -1) {
					stopSession();
					break;
				}

				if (buf.size() == 0) {
					continue;
				}

				byte[] messageBytes = buf.toByteArray();

			
				// byte[] raw = new byte[2048];
				// try {
				// 	int len = in.read(raw);
				// 	if (len == 0 )
				// 		continue;
				// 	else if (raw[0] == 0) {
				// 		continue;
				// 	} else if(len == -1) {
				// 		break;
				// 	}

				Message msg = builder.decode(messageBytes);
				if (msg != null) {
					String modifiedName = msg.getName() + " (Session " + sessionId + ")";
					String modifiedMsg = "from " + modifiedName + ", to group: " + msg.getGroup() + ", text: " + msg.getText();
					System.out.println(modifiedMsg + "\n");
				}
					// System.out.println(msg + "\n");
					
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