from tkinter import *

from tools.uds.gui.popup_window import PopupWindow

class ConnectWindow(PopupWindow):
    def __init__(self):
        super().__init__()
    def t(self,ip):
        self.c_event(ip)
        self.root.destroy()
        
    def CreateView(self):
        Label(self.root, text='IP').grid(row=0)
        self.e1 = Entry(self.root)
        self.e1.grid(row=0, column=1)
        self.b1 = Button(self.root, text="Connect",command=lambda:self.t(self.e1.get()))
        self.b1.grid(row=1, column=0,columnspan=2)