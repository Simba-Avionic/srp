

class FileReader:
    """Class to read data from file"""
    def read_data_in_parts(self,filename):
        """ odczytuje dane w porcjach po 251 Bajtów"""
        try:
            file_parts = []
            with open(filename, 'rb') as plik:
                while True:
                    part = plik.read(253)
                    if not part:
                        break
                    file_parts.append(part)
            return file_parts
        except FileNotFoundError:
            return "Plik nie znaleziony"
        except Exception as e:
            return f"Wystąpił błąd: {str(e)}"
