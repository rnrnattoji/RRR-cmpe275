package gash.socket;

import java.io.BufferedReader;
import java.io.IOException;
import java.io.InputStreamReader;
// import java.io.OutputStream;
import java.net.Socket;

import gash.payload.BasicBuilder;
import gash.payload.Message;

public class BasicClient {
	private String name;
	private String ipaddr;
	private int port;
	private String group = "public";
	private Socket clt;

	// public BasicClient() {
	// 	this("JAVA_CLIENT","127.0.0.1", 2000);
	// }

	public BasicClient(String name, String ipaddr, int port) {
		this.name = name;
		this.ipaddr = ipaddr;
		this.port = port;
	}

	public void stop() {
		if (this.clt != null) {
			try {
				this.clt.close();
			} catch (IOException e) {
				// TODO better error handling? yes!
				e.printStackTrace();
			}
		}
		this.clt = null;
	}

	public void join(String group) {
		this.group = group;
	}

	public void connect() {
		if (this.clt != null) {
			return;
		}

		try {
			this.clt = new Socket(this.ipaddr, this.port);
			System.out.println("\nConnected to Server with Address:" + this.ipaddr + " and port:" + this.port);
		}  catch (Exception e) {
				// System.out.println("An error occurred connecting to server: "+ e.getMessage());
				throw new  RuntimeException("Connection refused");
		}
	}

	public void sendMessage(String message) {

		if (isServerAlive()) {
            try {
				System.out.println("sending to group " + this.group + " from " + this.name + ": " + message);
	
				BasicBuilder builder = new BasicBuilder();
				byte[] msg = builder.encode(new Message(name, group, message)).getBytes();
				this.clt.getOutputStream().write(msg);
			
			} catch (IOException e) {
				System.out.println("IO error occured");
				stop();
			} catch (Exception e) {
				System.out.println("Unexpected error occured");
				stop();
			}
        } else {
			BufferedReader reader = new BufferedReader(new InputStreamReader(System.in));
			System.out.println("Server is Down");
			this.stop();
			System.out.print("Do you want to reconnect? ('Y' or 'N'): ");
			try {
				String answer = reader.readLine();
				if ("Y".equalsIgnoreCase(answer)) {
					while(true) {
						try {
							this.connect();
							break;
						} catch (Exception e) {
							System.out.println("\nNot able to connect? Do you want to retry? ('Y' or 'N') ");
							this.stop();
							answer = reader.readLine();
							if (!"Y".equalsIgnoreCase(answer)) {
								throw new  RuntimeException("User does not want to retry");
							}
						}
					}
				} else {
					throw new  RuntimeException("User does not want to retry");
				}
			} catch (IOException readException) {
				System.out.println("An error occurred reading input. Exiting...");
			}
		}
		}

// 	private boolean isServerAlive() {
//         if (this.clt == null) return false;

//         try {
//             this.clt.getOutputStream().write(0);
//             this.clt.getOutputStream().flush();
//             return true; 
//         } catch (IOException e) {
//             return false;
//         }  
//    }

//    private boolean isServerAlive() {
//         try {
//             this.clt.getOutputStream().write(0);
// 			this.clt.getOutputStream().flush();
// 			this.clt.getOutputStream().write(0);
// 			this.clt.getOutputStream().flush();
//             return true;
//         } catch (IOException e) {
//             return false;
//         }
//     }

	private boolean isServerAlive() {
		try {
			String dataToSend = "0000,\0";
			this.clt.getOutputStream().write(dataToSend.getBytes("UTF-8"));
			this.clt.getOutputStream().write(dataToSend.getBytes("UTF-8"));
			return true;
		} catch (IOException e) {
			return false;
		}
	}
}
