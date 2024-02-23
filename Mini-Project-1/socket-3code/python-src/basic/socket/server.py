import socket
import time
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
        t = time.time()
        while self.good:
            try:
                buf = b''
                while True:
                    data_chunk = self._cltconn.recv(2048)  # Receive in chunks of 1024 bytes
                    if data_chunk == b'':
                        break
                    if data_chunk.replace(b'0000,', b'').replace(b'\0', b'') == b'':
                        continue
                    if data_chunk.endswith(b'\0'):
                        buf += data_chunk.rstrip(b'\0')
                        break  # No more data to receive
                    buf += data_chunk

                if len(buf) <= 0:
                    self.good = False
                else:
                    self.process(buf.decode("utf-8"))
            except Exception as e:
                self.good = False
        
        print(f"\nSession {self._cltnumber} ending")
        print(f"\nElapse Time: {(time.time() - t) * 1000} ms")


if __name__ == '__main__':
    address = str(input("Please Enter the Server Address (DEFAULT: 0.0.0.0): ").strip() or "0.0.0.0")
    port = int(input("Please Enter the Server Port Number (DEFAULT: 2000): ").strip() or "2000")
    svr = BasicServer(address, port)
    svr.run()
