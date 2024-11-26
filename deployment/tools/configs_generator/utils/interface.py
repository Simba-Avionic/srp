
import logging

from utils.validator import SimBaConfigsValidator
from utils.base_class import BaseClass


class Interface(BaseClass):
    """
    The `Interface` class is a subclass of `BaseClass` that loads data from a file, validates the data
    schema, and provides a method to retrieve interface information by name.
    """
    def __init__(self,filepath:str):
        super().__init__()
        self.data=self.load_data(filepath)
        self.data=self.data["interfaces"]
        if not SimBaConfigsValidator().validate_interface(filepath):
            logging.warning("Invalid Interface Schema")

    def get_by_interface_name(self,name:str)->dict|None:
        """
        The function `get_by_interface_name` takes in a name as input and returns a dictionary from the
        `self.data` list that has a matching "name" value, or returns None if no match is found.
        
        @param name The `name` parameter is a string that represents the interface name.
        
        @return a dictionary if a matching interface name is found in the data list. If no matching
        interface name is found, it returns None.
        """
        for i in self.data:
            if i["name"]==name:
                return i
        return None