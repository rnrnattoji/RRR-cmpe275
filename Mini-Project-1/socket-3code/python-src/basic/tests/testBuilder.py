import socket
import time
import unittest

from ..payload import builder


class TestBuilder(unittest.TestCase):
    # def setUp(self):
    #     pass

    # def tearDown(self):
    #     pass

    # def testEncode(self):
    #     n = "fred"
    #     g = "dogs"
    #     t = "hello"

    #     b = builder.BasicBuilder()
    #     r = b.encode(n, g, t)
    #     print(f"encoded: {r}")

    #     parts = b.decode(r)

    #     self.assertEqual(n, parts[0])
    #     self.assertEqual(g, parts[1])
    #     self.assertEqual(t, parts[2])
    
    def testPerformance(self):
        try:
            bldr = builder.BasicBuilder()
            addr = ("0.0.0.0", 2000)
            clt = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
            clt.connect(addr)
            
            arr = []
            # for idx, msg_len in enumerate([10, 100, 1000, 10000, 100000, 1000000, 10000000]):
            for idx, msg_len in enumerate([10000]):
                m = "a" * msg_len
                m += 'b'
                msg = bytes(bldr.encode("PYTHON_CLIENT", "public", m), "utf-8")
                t = time.time()
                clt.sendall(msg)
                arr.append((time.time() - t) * 1000)
                print(idx)
            print(arr)
        except ConnectionRefusedError:
            print("\nUnable to connect to the server")


if __name__ == '__main__':
    unittest.main()
