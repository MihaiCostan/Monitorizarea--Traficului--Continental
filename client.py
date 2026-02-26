import json
import socket

def trimite_viteza(viteza):
    data = {
        "type": "speed_update",
        "car_id": "IS-10-ABC",
        "value": viteza
    }
    # Serializam in string si adaugam un terminator (ex: newline)
    json_string = json.dumps(data) + "\n"
    
    # Trimitem prin socket (presupunand ca socket-ul e deschis)
    # client_socket.sendall(json_string.encode('utf-8'))
    print(f"Am trimis: {json_string}")

trimite_viteza(65)