import sys
import os
import json
from deployment.tools.ara.sm.db import DB
from deployment.tools.ara.sm.sm_parser import SmParser
from deployment.tools.ara.sm.cpp_extractor import CppExtractor

def CreateDir(start:str,finish:str):
    for p in finish.split("/"):
            start+="/"+p
            try:
                os.makedirs(start)
            except:
                pass

out_path = sys.argv[1]
input_file = sys.argv[2]
db_ = DB();
with open(input_file,"r") as input_f:
    json_o = json.load(input_f)
    SmParser.Read(json_o,db_)
    print(len(db_.states))
    print(out_path)
    CreateDir(out_path,db_.package.replace(".","/"))
    for name,state in db_.states.items():
        CppExtractor.CreateState(out_path,state,db_.package)