from tkinter import *
from tkinter import ttk
from tkinter import messagebox 
import binascii

from tools.uds.gui.popup_window import PopupWindow
from tools.uds.gui.uds_con import UdsCon
states = {"Startup":35698,"Startup2":11884,"Platform":43499,"Running":46617,"SafetyMode":46771}
states_inv = {}


class MachineStateWindow(PopupWindow):
    def __init__(self):
        super().__init__()
        for k,v in states.items():
            states_inv[v] = k

    def StatusUpdate(self,res):
        self.current.destroy()
        if res[0] == 0x7f:
            self.current = Label(self.root, text=f'Current states: The current machine state cannot be read')
        else:
            val = int.from_bytes(res[3:])
            self.current = Label(self.root, text=f'Current states: {states_inv[val]}')
        self.current.grid(row=0)
    def onShow(self):
        UdsCon().SendCmd("220003",lambda res: self.StatusUpdate(res))
    def ChangeRes(self,res):
        if res[0] == 0x7F:
            messagebox.showerror("Error","Failed to change machine state")
        else:
            messagebox.showinfo("Info", "Change of machine state successfully completed")
        self.root.destroy()
        # UdsCon().SendCmd("220003",lambda res: self.StatusUpdate(res))
    def Change(self):
        if(len(self.change.get()) == 0):
            return
        UdsCon().SendCmd("2E0003"+states[self.change.get()].to_bytes(2).hex(),lambda res: self.ChangeRes(res))
    def CreateView(self):
        self.current = Label(self.root, text=f'Current states: ...')
        self.current.grid(row=0)
        self.change = ttk.Combobox(self.root, values=list(states.keys()))
        self.change.grid(row=1)
        Button(self.root,text="Change", command= lambda: self.Change()).grid(row=1,column=1)