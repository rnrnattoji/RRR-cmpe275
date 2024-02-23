import socket
import random
import string
import time

def generate_message():
    message_length = 6000
    message_content = ''.join(random.choices(string.ascii_letters + string.digits, k=message_length))
    message = f"{message_content}\0"
    total_length = len(message) + 4 + 6 + 11
    length_str = f"{total_length:04d}"
    final_message = f"{length_str},public,TEST_CLIENT,{message}"
    return final_message

def main():
    host = input("Enter the host address: ")
    port = int(input("Enter the port number: "))

    with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as sock:
        sock.connect((host, port))

        num_messages = 1000
        max_length = 0 

        for _ in range(num_messages):
            msg = generate_message()
            max_length = max(max_length, len(msg))
            sock.sendall(msg.encode('utf-8'))
            # print(f"Sent: {msg[:50]}...") 
        
        # print(f"Total messages sent: {num_messages}")

if __name__ == "__main__":
    main()
