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

			System.out.println("Server Address: " + this.host +" is listening on PORT: " + this.port);

			while (forever) {
				Socket s = socket.accept();
				if (!forever) {
					break;
				}

				String clientAddress = s.getRemoteSocketAddress().toString();
				String clientIp = clientAddress.replaceAll("^/|:\\d+$", "");
				System.out.println("---> Server got a Client Connection from " + clientIp);
				var sh = new SessionHandler(s);
				sh.start();
			}
		} catch (Exception e) {
			e.printStackTrace();
		}
	}
}
