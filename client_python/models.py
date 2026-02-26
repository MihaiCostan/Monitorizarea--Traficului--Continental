import uuid

class UserProfile:
    def __init__(self, nume="", masina="", nr_auto="", email="", parola=""):
        self.user_id = str(uuid.uuid4())[:8]
        self.nume = nume
        self.masina = masina
        self.nr_auto = nr_auto
        self.email = email
        self.parola = parola

    def to_dict(self):
        return {
            "type": "login",
            "user_id": self.user_id,
            "nume": self.nume,
            "masina": self.masina,
            "numar_masina": self.nr_auto
        }