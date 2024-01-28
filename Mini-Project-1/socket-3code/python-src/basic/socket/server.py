import socket
import threading

import sys
import os
sys.path.insert(1, os.path.abspath(
    os.path.join(os.path.join(os.path.dirname(__file__), '..'), '..')))

from basic.payload import builder

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
      elif self.port <=1024:
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
      addr = (self.ipaddr,self.port)
      if socket.has_dualstack_ipv6():
         self._svr = socket.create_server(addr, family=socket.AF_INET6, dualstack_ipv6=True)
      else:
         self._svr = socket.create_server(addr)
      self._svr.listen(10)

      print(f"server ({self.ipaddr}) is listening on {self.port}")

      while self.good:
        cltconn, caddr = self._svr.accept()
        print(f"Connection from {caddr[0]}")
        csession = SessionHandler(cltconn,caddr)
        csession.start()

# ----------------------------------------------

class SessionHandler(threading.Thread):
    def __init__(self,client_connection, client_addr):
        threading.Thread.__init__(self)
        self.daemon = False
        self._cltconn = client_connection
        self._cltaddr = client_addr
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

    def process(self,raw):
        try:
            bldr = builder.BasicBuilder()
            name,group,text = bldr.decode(raw)
            print(f"from {name}, to group: {group}, text: {text}")
        except Exception as e:
            pass

    def run(self):
        while self.good:
            try:
                buf = self._cltconn.recv(2048)
                if len(buf) <= 0:
                    self.good = False
                else:
                    self.process(buf.decode("utf-8"))
            except Exception as e:
                print(e)
                self.good = False

        print(f"clossing session {self._cltaddr}")


if __name__ == '__main__':
    svr = BasicServer("127.0.0.1", 2000)
    svr.run()
