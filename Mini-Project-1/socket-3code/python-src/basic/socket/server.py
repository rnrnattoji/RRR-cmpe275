import socket
import threading

from ..payload import builder


class BasicServer(object):
    def __init__(self, ipaddr, port=2000):
        self.ipaddr = ipaddr
        self.port = port
        self._svr = None
        self.good = True

        if self.ipaddr is None:
            raise ValueError("IP address is missing or empty")
        elif self.port is None:
            raise ValueError("port number is missing")
        elif self.port <= 1024:
            raise ValueError(f"port number ({port}) must be above 1024")

    def __del__(self):
        # closs socket
        self.stop()

    def stop(self):
        self.good = False
        if self._svr is not None:
            self._svr.close()
            self._svr = None

    def run(self):
        addr = (self.ipaddr, self.port)
        # if socket.has_dualstack_ipv6():
        #    self._svr = socket.create_server(addr, family=socket.AF_INET6, dualstack_ipv6=True)
        # else:
        #    self._svr = socket.create_server(addr)
        
        self._svr = socket.create_server(addr)
        self._svr.listen(10)

        print(f"Server Host: {self.ipaddr} is listening on PORT: {self.port}")

        cltnumber = 0
        while self.good:
            cltnumber += 1
            cltconn, cltaddr = self._svr.accept()
            print(f"\n---> Server got a Client Connection from {cltaddr[0]}")
            csession = SessionHandler(cltconn, cltaddr, cltnumber)
            csession.start()
            print(f"Session {cltnumber} started")

# ----------------------------------------------


class SessionHandler(threading.Thread):
    def __init__(self, client_connection, client_addr, client_number):
        threading.Thread.__init__(self)
        self.daemon = False
        self._cltconn = client_connection
        self._cltaddr = client_addr
        self._cltnumber = client_number
        self.good = True

    def __del__(self):
        self.close()

    def close(self):
        if self._cltconn is None:
            return
        else:
            self._cltconn.close()
            self._cltconn = None
            self.good = False

    def process(self, raw):
        try:
            bldr = builder.BasicBuilder()
            name, group, text = bldr.decode(raw)
            print(f"\nFrom {name} (Session {self._cltnumber}), to group: {group}, text: {text}")
        except Exception as e:
            pass

    def run(self):
        while self.good:
            try:
                # initial_chunk = self._cltconn.recv(2)
                # print(len(initial_chunk))
                # MSGLEN = int(initial_chunk.decode("utf-8").split(",", 1)[0])

                # chunks = [initial_chunk]
                # bytes_recd = len(initial_chunk)
                # while bytes_recd < MSGLEN:
                #     chunk = self._cltconn.recv(2048)
                #     if chunk == b'':
                #         raise RuntimeError("socket connection broken")
                #     chunks.append(chunk)
                #     bytes_recd = bytes_recd + len(chunk)

                # buf = b''.join(chunks)

                # chunks = []
                # while True:
                #     chunk = self._cltconn.recv(2048)
                #     if chunk is None:
                #         break
                #     chunks.append(chunk)
               
                # buf = b''.join(chunks)

                buf = self._cltconn.recv(2048)
                if len(buf) <= 0:
                    self.good = False
                else:
                    self.process(buf.decode("utf-8"))
            except Exception as e:
                self.good = False

        print(f"\nSession {self._cltnumber} ending")


if __name__ == '__main__':
    address = str(input("Please Enter the Server Address (DEFAULT: 0.0.0.0): ").strip() or "0.0.0.0")
    port = int(input("Please Enter the Server Port Number (DEFAULT: 2000): ").strip() or "2000")
    svr = BasicServer(address, port)
    svr.run()
