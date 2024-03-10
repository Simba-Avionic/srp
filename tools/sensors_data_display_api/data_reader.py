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
