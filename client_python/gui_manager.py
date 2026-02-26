import tkinter as tk
from tkinter import messagebox
from models import UserProfile

class TrafficGUI:
    def __init__(self, root, network_manager):
        self.root = root
        self.net = network_manager
        self.current_user = UserProfile()

        self.net.on_message_received = self.display_message
        self.net.on_disconnection = self.handle_disconnection
        
        self.root.title("Monitorizare Trafic - Continental")
        self.root.geometry("400x500")
        self.root.configure(bg="#0099ff")

        self.setup_start_screen()

    def setup_start_screen(self):
        self.clear_screen()
        tk.Label(self.root, text="BINE AI VENIT", font=("Arial", 16, "bold"), bg="#0099ff", fg="white").pack(pady=30)
        
        tk.Button(self.root, text="LOGIN", width=20, command=self.setup_login_screen).pack(pady=10)
        tk.Button(self.root, text="SIGNUP", width=20, command=self.setup_signup_screen).pack(pady=10)

    def setup_signup_screen(self):
        self.clear_screen()

        tk.Label(self.root, text="ÎNREGISTRARE VEHICUL", font=("Arial", 16, "bold"), 
                 fg="white", bg="#0099ff").pack(pady=20)
        tk.Label(self.root, text="ID-ul tău de sistem (automat):", 
                 fg="white", bg="#0099ff").pack()
        tk.Label(self.root, text=self.current_user.user_id, font=("Courier", 14, "bold"), 
                 fg="#ff0000", bg="#0099ff").pack(pady=5)

        # Câmpul pentru NUME
        tk.Label(self.root, text="Nume Complet:", bg="#0099ff").pack()
        self.entry_nume = tk.Entry(self.root)
        self.entry_nume.pack()

        # Campul pentru email
        tk.Label(self.root, text="Email:", bg="#0099ff").pack()
        self.entry_nume = tk.Entry(self.root)
        self.entry_nume.pack()

        #Campul pentru parola
        tk.Label(self.root, text="Parola:", bg="#0099ff").pack()
        self.entry_nume = tk.Entry(self.root)
        self.entry_nume.pack()

        # Câmpul pentru MODEL MAȘINĂ
        tk.Label(self.root, text="Model Mașină:", bg="#0099ff").pack()
        self.entry_model = tk.Entry(self.root)
        self.entry_model.pack()

        # Câmpul pentru NUMĂR ÎNMATRICULARE
        tk.Label(self.root, text="Număr Înmatriculare:", bg="#0099ff").pack()
        self.entry_nr_auto = tk.Entry(self.root)
        self.entry_nr_auto.pack()

        tk.Button(self.root, text="Creează Cont", command=self.handle_signup).pack()

    def setup_login_screen(self):
        self.clear_screen()

        tk.Label(self.root, text="Email:").pack()
        self.entry_email = tk.Entry(self.root)
        self.entry_email.pack()
        
        tk.Label(self.root, text="Parolă:").pack()
        self.entry_pass = tk.Entry(self.root, show="*")
        self.entry_pass.pack()
    
        tk.Button(self.root, text="Intră în cont", command=self.handle_login_auth).pack()

    def handle_signup(self):
        nume_val = self.entry_nume.get()
        model_val = self.entry_model.get()
        nr_auto_val = self.entry_nr_auto.get()

        if not nume_val or not model_val or not nr_auto_val:
            messagebox.showwarning("Atenție", "Completează toate câmpurile!")
            return

        user = UserProfile(nume_val, model_val, nr_auto_val) # De luat din entries
        if self.net.connect():
            self.net.send_json(user.to_dict())
            self.show_main_screen()
        else:
            messagebox.showerror("Eroare", "Server indisponibil")

    def handle_disconnection(self):
        self.root.after(0, self._go_to_login_after_error)
    
    def _go_to_login_after_error(self):
        messagebox.showerror("Conexiune Pierdută", "Serverul s-a oprit sau conexiunea a fost întreruptă.")
        self.setup_signup_screen() # Revenim la pagina de login

    def display_message(self, raw_data):
        # Aici actualizezi UI-ul cu datele de la server
        print(f"Update primit în GUI: {raw_data}")

    def show_main_screen(self):
        self.clear_screen()
        pass

    def clear_screen(self):
        for widget in self.root.winfo_children():
            widget.destroy()