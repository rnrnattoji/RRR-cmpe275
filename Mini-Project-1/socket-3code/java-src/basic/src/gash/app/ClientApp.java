package gash.app;

import java.io.BufferedReader;
import java.io.InputStreamReader;

import gash.socket.BasicClient;

/**
 * client - basic chat construct. This varies from our Python and C++ versions
 * as it prompts the use for messages.
 * 
 * @author gash
 * 
 */
public class ClientApp {
	private BasicClient myClient;

	public ClientApp() {
	}

	public static void main(String[] args) {
		var myClient = new BasicClient("app", "127.0.0.1", 2000);
		myClient.connect();
		myClient.join("pets/dogs");

		var br = new BufferedReader(new InputStreamReader(System.in));
		while (true) {
			try {
				System.out.print("\nenter message ('exit' to quit): ");
				var m = br.readLine();
				if (m.length() == 0 || "exit".equalsIgnoreCase(m))
					break;

				myClient.sendMessage(m);
			} catch (Exception ex) {
				break;
			}
		}
	}
}