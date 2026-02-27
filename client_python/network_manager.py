import json
import socket
import select
import threading

BUFFER_SIZE = 2048

class NetworkManager:
    def __init__(self, host, port):
        self.host = host
        self.port = port
        self.socket = None
        self.is_running = False
        self.on_message_received = None
        self.on_disconnection = None

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
                    data = self.socket.recv(BUFFER_SIZE).decode('utf-8')
                    if not data: 
                        print(f"Deconectare bruscă de la server!")
                        self.is_running = False # Oprim loop-ul
                        if self.on_disconnection:
                            self.on_disconnection() # 2. Anunțăm deconectarea
                        break
                    messages = data.strip().split('\n')
                    for msg in messages:
                        if msg and self.on_message_received:
                            self.on_message_received(msg)
            except: 
                self.is_running = False
                if self.on_disconnection:
                    self.on_disconnection()
                break
            
    # În network_manager.py, în interiorul clasei NetworkManager
    def disconnect(self):
        self.is_running = False  # Oprește loop-ul _receive_loop
        if self.socket:
            try:
                self.socket.close()
                print("[NET] Socket închis manual.")
            except Exception as e:
                print(f"[NET] Eroare la închidere: {e}")
            finally:
                self.socket = None