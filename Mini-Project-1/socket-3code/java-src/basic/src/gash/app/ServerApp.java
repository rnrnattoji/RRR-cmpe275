package gash.app;

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
		var port = 2000;
		var server = new BasicServer(host,port);
		server.start();
	}
}
