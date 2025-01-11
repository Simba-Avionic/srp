from tkinter import *

class PopupWindow:
    def __init__(self):
        self.root = Toplevel()
        self.CreateView()
        
    def OnCloseEvent(self,event):
        self.c_event = event

    def Show(self):
        self.onShow()
        self.root.mainloop()
        

    def onShow(self):
        pass
    def CreateView(self):
        pass
