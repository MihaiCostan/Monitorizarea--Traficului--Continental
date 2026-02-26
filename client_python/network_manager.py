import json
import socket
import select
import threading

class NetworkManager:
    def __init__(self, host, port):
        self.host = host
        self.port = port
        self.socket = None
        self.is_running = False
        self.on_message_received = None

    def connect(self):
        try:
            self.socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
            self.socket.connect((self.host, self.port))
            self.is_running = True
            threading.Thread(target=self._receive_loop, daemon=True).start()
            return True
        except Exception as e:
            print(f"Eroare conectare: {e}")
            return False
    
    def send_json(self, data):
        if self.socket:
            message = json.dumps(data) + "\n"
            self.socket.sendall(message.encode('utf-8'))
    
    def _receive_loop(self):
        while self.is_running:
            try:
                ready = select.select([self.socket], [], [], 0.5)
                if ready[0]:
                    data = self.socket.recv(2048).decode('utf-8')
                    if not data: 
                        break
                    if self.on_message_received:
                        self.on_message_received(data)
            except: 
                break