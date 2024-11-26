import logging
from utils.validator import SimBaConfigsValidator
from utils.base_class import BaseClass



# The `Router` class is a subclass of `BaseClass` that provides methods for retrieving router data
# based on name, interface, and implemented interfaces, as well as checking if two interfaces are
# present in the same router.
class Router(BaseClass):
    def __init__(self,filepath:str):
        super().__init__()
        self.data=self.load_data(filepath)
        self.data=self.data["routers"]
        if not SimBaConfigsValidator().validate_routers(filepath):
            logging.warning("Invalid Router Schema")

    def get_by_name(self,name:str)->dict|None:
        for r in self.data:
            if r["name"]==name:
                return r
        return None
    
    def get_by_own_interface(self,interface_name:str)->dict|None:
        for r in self.data:
            if interface_name==r["interface"]:
                return r
        return None
    
    def get_by_implemented_interfaces(self,interface_name:str)->dict|None:
        for r in self.data:
            if interface_name in r["interfaces"]:
                return r
        return None
    
    def interfaces_in_the_same_router(self,interface_name1:str,interface_name2:str)->bool:
        for r in self.data:
            if interface_name1 in r["interfaces"] and interface_name2 in r["interfaces"]:
                return True
        return False


