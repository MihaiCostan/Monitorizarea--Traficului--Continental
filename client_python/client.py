import json
import socket
import select
import threading
import uuid
import tkinter as tk
from tkinter import messagebox, scrolledtext

SERVER_IP = "127.0.0.1"
PORT = 1111
BUFFER_SIZE = 2048

# def trimite_viteza(viteza):
#     data = {
#         "type": "speed_update",
#         "car_id": "IS-10-ABC",
#         "value": viteza
#     }
#     # Serializam in string si adaugam un terminator (ex: newline)
#     json_string = json.dumps(data) + "\n"
#     # Trimitem prin socket (presupunand ca socket-ul e deschis)
#     # client_socket.sendall(json_string.encode('utf-8'))
#     print(f"Am trimis: {json_string}")
# trimite_viteza(65)

class ClientGUI:
    def __init__(self, root):
        self.root = root
        self.user_id = str(uuid.uuid4())[:8]
        self.root.title("MonitorGUI")
        self.root.geometry("1920x1080")
        self.root.configure(bg="#0099ff") #albastru momentan

        self.client_socket = None
        self.is_running = False

        self.colors = {
            "Red": "#e21b3c", # Roșu
            "Blue": "#1368ce", # Albastru
            "Yellow": "#d89e00", # Galben
            "Green": "#26890c", # Verde
            "bg": "#0099ff", # Mov
            "text": "#ffffff"
        }

        self.setup_login_ui()

    def setup_login_ui(self):

        #Ecranul de login
        self.clear_screen()

        #afisare ID generat
        tk.Label(self.root, text=f"ID generat: {self.user_id}", fg="white", bg = self.colors["bg"]).pack()

        #introducere date personale
        tk.Label(self.root, text="Nume Complet:").pack()
        self.entry_nume = tk.Entry(self.root)
        self.entry_nume.pack()

        tk.Label(self.root, text="Model Mașină:").pack()
        self.entry_model = tk.Entry(self.root)
        self.entry_model.pack()

        tk.Label(self.root, text="Număr Înmatriculare:").pack()
        self.entry_nr_auto = tk.Entry(self.root)
        self.entry_nr_auto.pack()

        tk.Button(self.root, text="Conectare", command=self.connect_to_server).pack()

        #parte de design

    def connect_to_server(self):

        nume = self.entry_nume.get()
        model = self.entry_model.get()
        nr_auto = self.entry_nr_auto.pack()
        if not all([nume, model, nr_auto]):
            messagebox.showwarning("Atenție", "Te rugăm să completezi toate câmpurile!")
            return

        try:
            self.client_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
            self.client_socket.connect((SERVER_IP, PORT))

            #impachetez informatia JSON si o trmit
            login_packet = {
                "type": "login",
                "user_id": self.user_id,
                "nume": nume,
                "masina": model,
                "numar_masina": nr_auto
            }
            message = json.dumps(login_packet) + "\n"
            self.client_socket.sendall(message.encode('utf-8'))

            #pornire thread cu select pentru update-uri
            self.is_running = True
            threading.Thread(target=self.receive_messages_loop, daemon=True).start()

            self.setup_game_ui()
        except Exception as e:
            messagebox.showerror("Eroare", f"Nu m-am putut conecta la server: {e}")