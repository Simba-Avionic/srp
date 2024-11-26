# The `SimBaConfigsValidator` class is a Python class that validates JSON files against predefined
# schemas for routers, interfaces, and services.
from cerberus import Validator
from json import load
from utils.schema import router_schema,interface_schema,services_schema

# The SimBaConfigsValidator class is used to validate JSON files for routers, interfaces, and
# services.
class SimBaConfigsValidator:
    r_validator:Validator
    i_validator:Validator
    s_validator:Validator
    def __init__(self):
        self.r_validator=Validator(router_schema)
        self.i_validator=Validator(interface_schema)
        self.s_validator=Validator(services_schema)

    def validate_routers(self,filepath:str)->bool:
        """
        The function `validate_routers` reads a file, validates its contents using a router validator, and
        returns a boolean value indicating whether the validation was successful.
        
        :param filepath: The `filepath` parameter is a string that represents the path to the file that
        contains the routers data
        :type filepath: str
        :return: the result of the `validate` method called on `self.r_validator` with the loaded content of
        the file as the argument.
        """
        with open(filepath,"r",encoding="UTF-8") as file:
            return self.r_validator.validate(load(file))
        
    def validate_interface(self,filepath:str)->bool:
        """
        The function `validate_interface` reads a file, validates its contents using a validator, and
        returns a boolean indicating whether the validation was successful.
        
        :param filepath: The `filepath` parameter is a string that represents the path to the file that
        needs to be validated
        :type filepath: str
        :return: the result of the `validate` method of the `i_validator` object, which is being called with
        the loaded content of the file specified by the `filepath` parameter.
        """
        with open(filepath,"r",encoding="UTF-8") as file:
            return self.i_validator.validate(load(file))

    def validate_services(self,filepath:str)->bool:
        """
        The function `validate_services` reads a file, validates its contents using a validator object, and
        returns a boolean value indicating whether the validation was successful.
        
        :param filepath: A string representing the file path of the file to be opened and read
        :type filepath: str
        :return: a boolean value.
        """
        with open(filepath,"r",encoding="UTF-8") as file:
            return self.s_validator.validate(load(file))

