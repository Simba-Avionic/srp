from tkinter import *
from tkinter import messagebox 

from tools.uds.gui.console import Console
from tools.uds.gui.connect_window import ConnectWindow
from tools.uds.gui.machine_state_window import MachineStateWindow 
from tools.uds.gui.uds_con import UdsCon
global ECU
ECU = None
def ConnectView():
    v = ConnectWindow()
    v.OnCloseEvent(lambda v: UdsCon().ConnectToEcu(v))
    v.Show()
    
def SendCmd(val):
    UdsCon().SendCmd(val)
def onDisconnect():
    menu.entryconfig("ECU", state="disabled")
    menu.entryconfig("DTC", state="disabled")
    CON.entryconfig("Disconnect", state="disabled")
    CON.entryconfig("Connect", state="normal")
    CON.entryconfig("Connect to 192.168.10.101", state="normal")
    CON.entryconfig("Connect to 192.168.10.102", state="normal")
    
def onConnect():
    menu.entryconfig("ECU", state="normal")
    menu.entryconfig("DTC", state="normal")
    CON.entryconfig("Disconnect", state="normal")
    CON.entryconfig("Connect", state="disabled")
    CON.entryconfig("Connect to 192.168.10.101", state="disabled")
    CON.entryconfig("Connect to 192.168.10.102", state="disabled")
def ClearDtcStatus(res):
    if res[0] == 0x7F:
         messagebox.showerror("Error", f"An error occurred while cleaning the memory : {res[-1]}")
    else:
        messagebox.showinfo("Successful", "Memory cleared") 
root = Tk()
root.title("UDS APP")
menu = Menu(root)
root.config(menu=menu)
CON = Menu(menu)
menu.add_cascade(label='Connection', menu=CON)
scrollbar = Scrollbar(root)
scrollbar.pack(side=RIGHT, fill=Y)

txt = Text(root, wrap=WORD,width=200) # wrap=CHAR, wrap=NONE
txt.pack(expand=1, fill=BOTH)

txt.config(yscrollcommand=scrollbar.set)
scrollbar.config(command=txt.yview)
Console().SetContainer(txt)
CON.add_command(label="Connect",command=ConnectView )
CON.add_command(label="Connect to 192.168.10.101",command=lambda:UdsCon().ConnectToEcu("192.168.10.101") )
CON.add_command(label="Connect to 192.168.10.102",command=lambda:UdsCon().ConnectToEcu("192.168.10.102") )
CON.add_command(label="Disconnect",command=lambda:UdsCon().Disconnect() )
CON.entryconfig("Disconnect", state="disabled")

ECU = Menu(menu)
menu.add_cascade(label='ECU', menu=ECU)
ECU.add_command(label="Machine state", command=lambda: MachineStateWindow().Show())
ECU.add_command(label="Apps")
ECU.add_command(label="Enter to Engineering mode")
ECU.add_command(label="Soft restart",command=lambda: SendCmd("1103"))
ECU.add_command(label="Hard restart",command=lambda: SendCmd("1101"))
menu.entryconfig("ECU", state="disabled")

DTC = Menu(menu)
menu.add_cascade(label='DTC', menu=DTC)
DTC.add_command(label="Read Memory")
DTC.add_command(label="Clear Memory",command=lambda:UdsCon().SendCmd("14FFFFFF",lambda res:ClearDtcStatus(res)))
menu.entryconfig("DTC", state="disabled")

button1 = Button(root, text="Send",command=lambda: SendCmd(e1.get()))
button1.pack(side=RIGHT)
e1 = Entry(root,width=190)
e1.pack(side=LEFT)
UdsCon().AddCallback(onConnect,onDisconnect)

if __name__ == '__main__':
    mainloop()
    UdsCon().Disconnect(False)