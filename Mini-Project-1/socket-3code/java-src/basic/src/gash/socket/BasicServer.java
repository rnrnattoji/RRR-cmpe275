package gash.socket;

import java.net.ServerSocket;
import java.net.Socket;

/**
 * server to manage incoming clients
 * 
 * @author gash
 * 
 */
public class BasicServer {
	private String host;
	private int port;
	private ServerSocket socket;
	private boolean forever = true;
	
	public BasicServer(String host, int port) {
		this.host = host;
		this.port = port;
	}

	/**
	 * start monitoring socket for new connections
	 */
	public void start() {
		try {
			socket = new ServerSocket(port);

			System.out.println("Server Host: " + socket.getInetAddress().getHostAddress());

			while (forever) {
				Socket s = socket.accept();
				if (!forever) {
					break;
				}

				System.out.println("--> server got a client connection");
				var sh = new SessionHandler(s);
				sh.start();
			}
		} catch (Exception e) {
			e.printStackTrace();
		}
	}
}
