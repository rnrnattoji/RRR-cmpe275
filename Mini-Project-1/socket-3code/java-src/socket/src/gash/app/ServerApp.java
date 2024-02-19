package gash.app;

import java.util.InputMismatchException;
import java.util.Scanner; 
import gash.socket.BasicServer;

/**
 * server application
 * 
 * @author gash
 * 
 */
class ServerApp {
	public ServerApp() {
	}

	public static void main(String[] args) {
		var host = "0.0.0.0";
		int port = 2001;

		Scanner scanner = new Scanner(System.in); 

        System.out.print("Please Enter the Server Address (DEFAULT: 0.0.0.0): "); 
        String hostInput = scanner.nextLine().trim();
        if (!hostInput.isEmpty()) {
            host = hostInput;
        }
	    
		while (true) {
            System.out.print("Please Enter the Server Port Number (DEFAULT: 2001): "); 
            String portInput = scanner.nextLine().trim();
            if (portInput.isEmpty()) {
                break;
            }
            
            try {
                int inputPort = Integer.parseInt(portInput);

				if (inputPort < 1024 || inputPort > 65535) {
                    throw new IllegalArgumentException("Port number must be between 1024 and 65535.");
                }
                port = inputPort;
                break;

            } catch (InputMismatchException e) {
                System.out.println("ERROR! Please enter a valid number."); 
                scanner.next();

            } catch (IllegalArgumentException e) {
                System.out.println(e.getMessage());
				
            } catch (Exception e) {
                System.out.println("An unexpected error occurred: " + e.getMessage());
            }
        }

		var server = new BasicServer(host,port);
		server.start();
		scanner.close();
	}
}
