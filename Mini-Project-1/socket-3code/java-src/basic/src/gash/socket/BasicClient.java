package gash.socket;

import java.io.IOException;
import java.net.Socket;

import gash.payload.BasicBuilder;
import gash.payload.Message;

public class BasicClient {
	private String name;
	private String ipaddr;
	private int port;
	private String group = "public";

	private Socket clt;

	public BasicClient(String name) {
		this(name, "127.0.0.1", 2000);
	}

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
			System.out.println("Connected to " + clt.getInetAddress().getHostAddress());
		} catch (Exception e) {
			e.printStackTrace();
		}
	}

	public void sendMessage(String message) {
		if (this.clt == null) {
			System.out.println("no connection, text not sent");
			return;
		}

		try {
			BasicBuilder builder = new BasicBuilder();
			byte[] msg = builder.encode(new Message(name, group, message)).getBytes();
			this.clt.getOutputStream().write(msg);
		} catch (Exception e) {
			e.printStackTrace();
		}
	}
}
