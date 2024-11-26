import json 
import sys

def main():
    
    with open(sys.argv[1],"w+") as out:
        out_obj = {}
        with open(sys.argv[2],"r") as src_a:
            obj = json.loads(src_a.read())
            obj = obj["platform"]
            out_text = """
#!/bin/sh
################################################################################
#
#   Copyright (c) 2024 Bartosz Snieg.
#
################################################################################
#
echo "Setting interface: """ + obj["interface_name"] + """ for """ + obj["name"]  + """ "
echo "ip: """ + obj["ip"] + """"
echo "net mask """ + obj["mask"] + """ "
ifconfig """ + obj["interface_name"] + """ """ + obj["ip"] + """ netmask """ + obj["mask"] + """
echo "Interface set [DONE]"
        """
        out.write(out_text)
if __name__ == "__main__":
    main()