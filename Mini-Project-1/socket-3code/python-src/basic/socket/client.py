import socket
from basic.payload import builder

class BasicClient(object):
    def __init__(self, name, ipaddr="127.0.0.1", port=2000):
        self._clt = None
        self.name = name
        self.ipaddr = ipaddr
        self.port = port

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

        addr = (self.ipaddr,self.port)
        self._clt = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        self._clt.connect(addr)
        #self._clt.setblocking(False)

    def join(self, group):
        self.group = group

    def sendMsg(self, text):
        if self._clt is None:
            raise RuntimeError("No connection to server exists")

        print(f"sending to group {self.group} from {self.name}: {text}")
        bldr = builder.BasicBuilder()
        m = bldr.encode(self.name,self.group,text)
        self._clt.send(bytes(m, "utf-8"))

    def groups(self):
        # return list of groups
        pass

    def getMsgs(self):
        # get the latest messages from a group
        pass


if __name__ == '__main__':
    clt = BasicClient("frida_kahlo","127.0.0.1",2000)
    while True:
        m = input("enter message: ")
        if m == '' or m == 'exit':
            break
        else:
            clt.sendMsg(m)
            clt.sendMsg(m)
