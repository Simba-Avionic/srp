# Używamy obrazu Pythona
FROM python:3.9-slim

# Ustawiamy katalog roboczy
WORKDIR /app

# Kopiujemy pliki aplikacji do kontenera
COPY . /app

# Instalujemy wymagane pakiety
RUN pip install --no-cache-dir -r requirements.txt

# Uruchamiamy serwer Flask na zadanym IP
CMD ["python", "app.py"]
