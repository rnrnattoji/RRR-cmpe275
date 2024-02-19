import socket

from ..payload import builder


class BasicClient(object):
    def __init__(self, name, ipaddr="127.0.0.1", port=2000):
        self._clt = None
        self.name = name
        self.ipaddr = ipaddr
        self.port = port
        self.msgs = []

        self.group = "public"

        if self.ipaddr is None:
            raise ValueError("IP address is missing or empty")
        elif self.port is None:
            raise ValueError("port number is missing")

        self.connect()

    def __del__(self):
        self.stop()

    def stop(self):
        if self._clt is not None:
            self._clt.close()
        self._clt = None

    def connect(self):
        if self._clt is not None:
            return

        addr = (self.ipaddr, self.port)
        self._clt = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        self._clt.connect(addr)
        print(f"\nConnected to Server with Address:{self.ipaddr} and port:{self.port}")
        # self._clt.setblocking(False)

    def is_connection_alive(self):
        try:
            # Attempt to send a minimal message to check the connection
            self._clt.sendall(b'\0')
            # Poll the socket to check for errors or closed connection
            self._clt.getpeername()
            return True
        except (socket.error, AttributeError):
            return False
            
    def join(self, group):
        self.group = group

    def sendMsg(self, text) -> str:
        if self._clt is None:
            raise RuntimeError("No connection to server exists")
        
        if self.is_connection_alive():
            print(f"sending to group {self.group} from {self.name}: {text}")
            bldr = builder.BasicBuilder()
            m = bytes(bldr.encode(self.name, self.group, text), "utf-8")
            self._clt.sendall(m)
        else:
            print("\nServer is Down")
            self.__del__()
            inp = str(input("\nDo you want to reconnect? ('Y' or 'N') "))
            if inp == "Y":
                while True:
                    try:
                        self.connect()
                        break
                    except ConnectionRefusedError:
                        inp_retry = str(input("\nNot able to connect! Do you want to retry? ('Y' or 'N') "))
                        self.__del__()
                        if inp_retry != "Y":
                            raise RuntimeError("\nNo connection to server exists")
            else:
                raise RuntimeError("\nNo connection to server exists")


    def storeMsg(self, msg):
        self.msgs.append(msg)

    def groups(self):
        # return list of groups
        pass

    def getMsgs(self):
        # get the latest messages from a group
        pass


if __name__ == '__main__':
    name = str(input("Please Enter you Name (DEFAULT: PYTHON_CLIENT): ").strip() or "PYTHON_CLIENT")
    address = str(input("Please Enter the Server Address that you want to connect (DEFAULT: 0.0.0.0): ").strip() or "0.0.0.0")
    port = int(input("Please Enter the Server Port Number (DEFAULT: 2000): ").strip() or "2000")

    try:
        clt = BasicClient(name, address, port)
        while True:
            m = str(input("\nEnter message ('exit' to quit): "))
            if m == '' or m == 'exit':
                break
            else:
                try:
                    for i in range(0, 5000):
                        m += 'a'
                    m += 'b'
                    clt.sendMsg(m)
                except RuntimeError as e:
                    break
    except ConnectionRefusedError:
        print("\nUnable to connect to the server")

