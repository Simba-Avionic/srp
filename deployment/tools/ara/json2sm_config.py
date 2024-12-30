import sys
import os
import json
from deployment.tools.ara.sm.db import DB
from deployment.tools.ara.sm.sm_parser import SmParser
from deployment.tools.ara.sm.config_extractor import ConfigExtractor

out_path = sys.argv[1]
input_file = sys.argv[2]
db_ = DB();
with open(input_file,"r") as input_f:
    json_o = json.load(input_f)
    SmParser.Read(json_o,db_)
    ConfigExtractor.ExtractConfigFile(out_path,db_)
