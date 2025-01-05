import datetime
from tkinter import *

from tools.uds.gui.singleton_lib import SingletonMeta

class Console(metaclass=SingletonMeta):
    def SetContainer(self,container):
        self.container = container
        self.ok = True
    def AddLog(self,text):
       now = datetime.datetime.now()
       msg = f'[{now.hour}:{now.minute}:{now.second}]: {text}\n'
       if self.ok:
        self.container.insert(END,msg)
        self.container.see(END)
       