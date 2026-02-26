# gui_manager.py
import tkinter as tk
from tkinter import messagebox
from models import UserProfile

class TrafficGUI:
    def __init__(self, root, network_manager):
        self.root = root
        self.net = network_manager
        self.net.on_message_received = self.display_message
        
        self.root.title("Monitorizare Trafic - Continental")
        self.root.geometry("400x500")
        self.root.configure(bg="#0099ff")
        self.setup_login_ui()

    def setup_login_ui(self):
        # ... (Codul tău de UI pentru login) ...
        # ATENȚIE: am corectat bug-ul tău cu .pack() la entry!
        tk.Label(self.root, text="Nume:").pack()
        self.entry_nume = tk.Entry(self.root)
        self.entry_nume.pack()
        
        tk.Button(self.root, text="Conectare", command=self.handle_login).pack()

    def handle_login(self):
        user = UserProfile(self.entry_nume.get(), "Model", "Nr") # De luat din entries
        if self.net.connect():
            self.net.send_json(user.to_dict())
            self.show_main_screen()
        else:
            messagebox.showerror("Eroare", "Server indisponibil")

    def display_message(self, raw_data):
        # Aici actualizezi UI-ul cu datele de la server
        print(f"Update primit în GUI: {raw_data}")

    def show_main_screen(self):
        # Curăță login și arată bordul de trafic
        pass