import random
import socket
import string
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
    
    def generate_message(self):
        message_length = random.randint(100, 6000)
        message_content = ''.join(random.choices(string.ascii_letters + string.digits, k=message_length))
        message = f"{message_content}\0"
        total_length = len(message) + 4 + 6 + 11
        length_str = f"{total_length:04d}"
        final_message = f"{length_str},public,TEST_CLIENT,{message}"
        return final_message

    def testPerformance(self):
        bldr = builder.BasicBuilder()
        addr = ("0.0.0.0", 2000)
        clt = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        clt.connect(addr)

        num_messages = random.randint(1000, 10000)
        max_length = 0 

        for _ in range(num_messages):
            msg = self.generate_message()
            max_length = max(max_length, len(msg))
            clt.sendall(msg.encode('utf-8'))
            print(f"Sent: {msg[:50]}...") 
        
        print(f"Total messages sent: {num_messages}")

    # def testPerformance(self):
    #     try:
    #         bldr = builder.BasicBuilder()
    #         addr = ("0.0.0.0", 2000)
    #         clt = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    #         clt.connect(addr)
            
    #         arr = []
    #         for idx, msg_len in enumerate([10, 100, 1000, 10000, 100000, 1000000, 10000000]):
    #         # for idx, msg_len in enumerate([1000000]):
    #             m = "a" * msg_len
    #             msg = bytes(bldr.encode("PYTHON_CLIENT", "public", m), "utf-8")
    #             t = time.time()
    #             clt.sendall(msg)
    #             arr.append((time.time() - t) * 1000)
    #             print(idx)
    #         print(arr)
    #     except ConnectionRefusedError:
    #         print("\nUnable to connect to the server")


if __name__ == '__main__':
    unittest.main()
