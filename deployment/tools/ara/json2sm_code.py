import sys
import os
import json
from datetime import datetime
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
out2_path = sys.argv[2]
input_file = sys.argv[3]
db_ = DB();
with open(input_file,"r") as input_f:
    json_o = json.load(input_f)
    SmParser.Read(json_o,db_)
    print(len(db_.states))
    print(out_path)
    CreateDir(out_path,db_.package.replace(".","/"))
    for name,state in db_.states.items():
        CppExtractor.CreateState(out_path,state,db_.package)
    with open(out2_path,"w") as init_file:
        include_list = ""
        init_list=""
        for name,state in db_.states.items():
            include_list+=f"""#include "{db_.package.replace(".","/")}/{name}_impl.h"\n"""
            init_list+=f"""        std::ignore = con->AddNewState(std::make_shared<{db_.package.replace(".","::")}::{name}Impl>());\n"""
        
        out = """/**
################################################################################
#
#   The file has been generated do not change its content
#
#   Copyright (c) """+str(datetime.now().year)+""" Bartosz Snieg.
#
################################################################################
#
*/
#include "ara/exec/sm/initialization.h"
#include "ara/exec/sm/state_controller.h"

"""+include_list+"""

namespace ara {
namespace exec {
namespace sm {
std::shared_ptr<IStateController> Initialize(std::unique_ptr<ISmComWrapper> com_wrapper) noexcept {
        auto con = std::make_shared<StateController>(std::move(com_wrapper),"""+str(db_.init_id)+"""U);
"""+init_list+"""
        return con;
    }
}  // namespace sm
}  // namespace exec
}  // namespace ara
"""
        init_file.write(out)