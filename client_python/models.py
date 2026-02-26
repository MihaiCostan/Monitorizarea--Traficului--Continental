import uuid

class UserProfile:
    def __init__(self, nume="", masina="", nr_auto="", email="", parola=""):
        # Generăm ID-ul doar dacă nu avem unul (ex: la signup)
        self.user_id = str(uuid.uuid4())[:8]
        self.nume = nume
        self.masina = masina
        self.nr_auto = nr_auto
        self.email = email
        self.parola = parola

    def clear(self):
        self.nume = ""
        self.masina = ""
        self.nr_auto = ""
        self.email = ""
        self.parola = ""