from __future__ import annotations
from deployment.tools.ara.someip.lib.someip_db import *
from deployment.tools.ara.common.data_structure import *

class SomeIPGenerator:
    def CreateDataStructure(main_path:str, structure: data_type):
        dir_list = structure.name.split(".")
        print(dir)