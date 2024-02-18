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
		var host = "127.0.0.1";
		int port = -1;

		Scanner scanner = new Scanner(System.in); 
	    
		while (true) {
            System.out.println("Enter port number which the server will run on: "); 
            
            try {
                port = scanner.nextInt(); 

				if (port < 1024 || port > 65535) {
                    throw new IllegalArgumentException("Port number must be between 1024 and 65535.");
                }
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
