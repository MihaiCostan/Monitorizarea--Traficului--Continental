import json
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


    # UI SIGNUP
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
        self.entry_email = tk.Entry(self.root)
        self.entry_email.pack()

        #Campul pentru parola
        tk.Label(self.root, text="Parola:", bg="#0099ff").pack()
        self.entry_parola = tk.Entry(self.root)
        self.entry_parola.pack()

        # Câmpul pentru MODEL MAȘINĂ
        tk.Label(self.root, text="Model Mașină:", bg="#0099ff").pack()
        self.entry_model = tk.Entry(self.root)
        self.entry_model.pack()

        # Câmpul pentru NUMĂR ÎNMATRICULARE
        tk.Label(self.root, text="Număr Înmatriculare:", bg="#0099ff").pack()
        self.entry_nr_auto = tk.Entry(self.root)
        self.entry_nr_auto.pack()

        tk.Button(self.root, text="Creează Cont", command=self.handle_signup).pack()
        tk.Button(self.root, text="Înapoi", command=self.setup_start_screen).pack()

    def handle_signup(self):
        # Colectăm toate datele
        data = {
            "type": "signup",
            "email": self.entry_email.get(),
            "password": self.entry_parola.get(),
            "nume": self.entry_nume.get(),
            "masina": self.entry_model.get(),
            "numar_masina": self.entry_nr_auto.get(),
            "user_id": self.current_user.user_id # ID-ul generat automat
        }
        if not all(data.values()):
            messagebox.showwarning("Atenție", "Toate câmpurile sunt obligatorii!")
            return

        if self.net.connect():
            self.net.send_json(data)
        else:
            messagebox.showerror("Eroare", "Serverul nu poate fi contactat.")


    #UI LOGIN
    def setup_login_screen(self):
        self.clear_screen()

        tk.Label(self.root, text="Email:").pack()
        self.entry_email = tk.Entry(self.root)
        self.entry_email.pack()
        
        tk.Label(self.root, text="Parolă:").pack()
        self.entry_pass = tk.Entry(self.root, show="*")
        self.entry_pass.pack()
    
        tk.Button(self.root, text="Intră în cont", command=self.handle_login_auth).pack()
        tk.Button(self.root, text="Înapoi", command=self.setup_start_screen).pack()


    def handle_login_auth(self):
        # Luăm datele introduse de utilizator
        email = self.entry_email.get()
        password = self.entry_pass.get()

        if not email or not password:
            messagebox.showwarning("Atenție", "Introdu email-ul și parola!")
            return

        # Cream pachetul JSON de autentificare
        auth_packet = {
            "type": "login",
            "email": email,
            "password": password
        }

        # Trimitem prin NetworkManager
        if self.net.connect():
            self.net.send_json(auth_packet)
        else:
            messagebox.showerror("Eroare", "Serverul nu răspunde!")


    def display_message(self, raw_data):
        try:
            # Transformăm string-ul brut în obiect Python (dicționar)
            data = json.loads(raw_data)
            msg_type = data.get("type")

            # Rutăm mesajul către funcția potrivită în funcție de "type"
            if msg_type == "login_response":
                self._handle_login_response(data)
            elif msg_type == "signup_response":
                self._handle_signup_response(data)
            elif msg_type == "broadcast_incident":
                # Exemplu pentru viitor (notificări de accidente)
                self.root.after(0, lambda: messagebox.showinfo("ALERTĂ TRAFIC", data.get("detalii")))
            else:
                print(f"Tip mesaj necunoscut: {msg_type}")

        except json.JSONDecodeError:
            print(f"Eroare: Serverul a trimis un format non-JSON: {raw_data}")
        except Exception as e:
            print(f"Eroare generală în display_message: {e}")


    def _handle_login_response(self, data):
        if data.get("status") == "success":
            # Salvăm datele venite din DB în obiectul local
            self.current_user.user_id = data.get("user_id")
            self.current_user.nume = data.get("nume")
            self.current_user.masina = data.get("masina")
            self.current_user.nr_auto = data.get("numar_masina")

            self.root.after(0, lambda: messagebox.showinfo("Succes", f"Salut, {self.current_user.nume}!"))
            self.root.after(0, self.show_main_screen)
        else:
            self.root.after(0, lambda: messagebox.showerror("Eroare", "Email sau parolă incorectă!"))

    def _handle_signup_response(self, data):
        if data.get("status") == "success":
            self.root.after(0, lambda: messagebox.showinfo("Cont Nou", data.get("msg")))
            # După signup, îl trimitem la login
            self.root.after(0, self.setup_login_screen)
        else:
            self.root.after(0, lambda: messagebox.showerror("Eroare", data.get("msg")))

    def handle_disconnection(self):
        self.root.after(0, self._go_to_start_screen_after_error)
    
    def _go_to_start_screen_after_error(self):
        messagebox.showerror("Conexiune Pierdută", "Serverul s-a oprit sau conexiunea a fost întreruptă.")
        self.setup_start_screen() # Revenim la pagina principala

    def show_main_screen(self):
        self.clear_screen()
        pass

    def clear_screen(self):
        for widget in self.root.winfo_children():
            widget.destroy()