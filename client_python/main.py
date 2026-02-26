import tkinter as tk
from network_manager import NetworkManager
from gui_manager import TrafficGUI

if __name__ == "__main__":
    root = tk.Tk()
    
    # Inițializăm rețeaua
    net = NetworkManager("127.0.0.1", 1111)
    
    # Inițializăm GUI-ul și îi dăm acces la rețea
    app = TrafficGUI(root, net)
    
    root.mainloop()