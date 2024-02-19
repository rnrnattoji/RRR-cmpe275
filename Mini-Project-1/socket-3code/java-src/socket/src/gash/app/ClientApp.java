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
	// private BasicClient myClient;

	public ClientApp() {
	}

	public static void main(String[] args) {
		var br = new BufferedReader(new InputStreamReader(System.in));
        String serverAddress = "0.0.0.0";
        int port = 2001;
		String name = "JAVA_CLIENT";

        try {
			System.out.print("Please Enter client Name (DEFAULT: JAVA_CLIENT): ");
            String nameInput = br.readLine().trim();
            if (!nameInput.isEmpty()) {
                name = nameInput;
            }

            System.out.print("Please Enter the Server Address that you want to connect (DEFAULT: 0.0.0.0): ");
            String addressInput = br.readLine().trim();
            if (!addressInput.isEmpty()) {
                serverAddress = addressInput;
            }

            System.out.print("Please Enter the Server Port Number (DEFAULT: 2001): ");
            String portInput = br.readLine().trim();
            if (!portInput.isEmpty()) {
                port = Integer.parseInt(portInput);
            }
        } catch (Exception e) {
            System.out.println("An error occurred while reading client name, server address or port: " + e.getMessage());
            return;
        }

		var myClient = new BasicClient(name, serverAddress, port);
		myClient.connect();
		// myClient.join("pets/dogs");

		while (true) {
			try {
				System.out.print("\nEnter message ('exit' to quit): ");
				var m = br.readLine();
				if (m.length() == 0 || "exit".equalsIgnoreCase(m))
					break;

				try {
					myClient.sendMessage(m);
				} catch (Exception e) {
					break;
				}
			} catch (Exception e) {
				System.out.println("An error occurred: " + e.getMessage());
				break;
			}
		}

		myClient.stop();
        try {
			br.close();
		} catch (Exception e) {
			System.out.println("An unexpected error occurred: " + e.getMessage());
		}
	}
}