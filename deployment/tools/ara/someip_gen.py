from __future__ import annotations
import sys
import json
import os
from deployment.tools.ara.someip.lib.someip_parser import SomeIpParser
from deployment.tools.ara.common.common_parser import CommonParser

def LoadJson(path:str):
    CommonParser.LoadJson(path)

if __name__ == "__main__":
    LoadJson(sys.argv[1])