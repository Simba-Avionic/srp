import tkinter as tk
import threading
import time


class DataReader:
    def __init__(self, gui):
        self.gui = gui
        self.data = 0

    def read_data(self):
        while True:
            self.data += 1
            self.gui.update_data_label(self.data)
            time.sleep(1)


class GUI(tk.Tk):
    def __init__(self):
        super().__init__()

        self.title("Odczyt danych")
        self.geometry("200x100")

        self.label = tk.Label(self, text="Dane:")
        self.label.pack()

        self.data_label = tk.Label(self, text="")
        self.data_label.pack()

        self.data_reader = DataReader(self)
        self.start_reading()

    def start_reading(self):
        self.data_reader_thread = threading.Thread(target=self.data_reader.start_reading)
        self.data_reader_thread.start()

    def update_data_label(self, data):
        self.data_label.config(text=str(data))

    def stop_reading(self):
        self.data_reader.stop_reading()
        self.data_reader_thread.join()

if __name__ == "__main__":
    app = GUI()
    app.mainloop()
