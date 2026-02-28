import serial
import socket

# Konfiguracja
COM_PORT = 'COM12'
BAUD_RATE = 115200  # Zmień na 57600 jeśli trzeba
UDP_IP = "127.0.0.1"
UDP_PORT = 14550

ser = serial.Serial(COM_PORT, BAUD_RATE, timeout=1)
sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)

print(f"Mostkowanie {COM_PORT} -> UDP {UDP_PORT}... Przerwij Ctrl+C")

try:
    while True:
        data = ser.read(1024) # Czytaj co wpadnie
        if data:
            sock.sendto(data, (UDP_IP, UDP_PORT))
            print(f"Wysłano {len(data)} bajtów", end='\r')
except KeyboardInterrupt:
    ser.close()