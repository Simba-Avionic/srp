
import json
import logging
from utils.router import Router
from utils.service import Service
from utils.interface import Interface



class EndFileGenerator:
    router:Router
    service:Service
    interface:Interface
    file_path:str
    service_name:str

    def _generate_interface(self,interface_name:str):
        interface=self.interface.get_by_interface_name(interface_name)
        out={
            "ip":interface["ip"],
            "ipc":interface["ipc"],
            "port":interface["port"]
        }
        logging.info(f"_generate_interface {out}")
        return out

    def _generate_req_events(self,req_events_names:list)->list:
        out=[]
        for name in req_events_names:
            service_name=name.split('/')[0]
            event_name=name.split('/')[1]
            remote_service=self.service.get_by_name(service_name)
            event_id=self.service.get_event_id(service_name,event_name)
            if event_id is not None:
                out.append({"name":name,"service_id":remote_service["service_id"],"event_id":event_id})
            else:
                logging.critical("event not found")
                exit(1)
        return out



    def _generate_pub_events(self,pub_events:list):
        out={}
        for event in pub_events:
            string_name=self.service_name+'/'+event["name"]
            services_name=self.service.get_subscribers_by_event_name(string_name)
            sub_services=[]
            for s in services_name:
                sub_services.append(self.service.get_by_name(s))
            out[string_name]={
                "event_id":event.get("event_id"),
                "subscribers":[{"service_id":service.get("service_id")} for service in sub_services]
            }
        return out


    def _generate_req_methods(self,req_services:list)-> dict | None:
        db={}
        for endpoint in req_services:
            remote_service_name=endpoint.split("/")[0]
            remote_method_name=endpoint.split("/")[1]
            
            remote_service_id=self.service.get_by_name(remote_service_name).get("service_id","")

            remote_method_id=self.service.get_method_id_by_name(remote_method_name)

            db[endpoint]={
                "service_id":remote_service_id,
                "method_id":remote_method_id,
            }
            logging.info(db)
        return db

    def __init__(self,service_name:str,path:str):
        self.router=Router(f"{path}routers.json")
        self.interface=Interface(f"{path}interface.json")
        self.service=Service(f"{path}services.json")
        self.service_name=service_name
        if self.service.get_by_name(self.service_name) is None:
            logging.fatal(f"Service with name {self.service_name} dont exist")
    
    def save_to_file(self,path):
        with open(f"{path}/out.json","w") as file:
            json.dump(self.generate_output_json(),file,indent=4)

    def _generate_db(self):
        db={}
        my_interface_name=self.interface.get_by_interface_name(self.service.get_interface_name_by_service_name(self.service_name))["name"]
        for s in self.service.data:
            if s["name"]!=self.service_name:
                in_name=self.service.get_interface_name_by_service_name(s["name"])
                remote_service_interface=self.interface.get_by_interface_name(in_name)
                if self.router.interfaces_in_the_same_router(my_interface_name,in_name):
                    remote_service_interface["port"]=0
                    remote_service_interface["ip"]=""
                else:
                    remote_service_interface["ipc"]=""
                db[str(s["service_id"])]=remote_service_interface
        return db


    def generate_output_json(self):
        out_dict = {}
        work_service = self.service.get_by_name(self.service_name)

        if work_service is None:
            logging.warning(f"Service with name {self.service_name} does not exist")
            return out_dict

        interface_name = work_service.get("interface",None)
        out_dict["name"] = work_service.get("name",None)
        out_dict["service_id"] = work_service.get("service_id",None)
        out_dict["interface"] = self._generate_interface(interface_name)


        out_dict["req_events"]=self._generate_req_events(work_service.get("req_events",[]))

        out_dict["pub_events"]=self._generate_pub_events(self.service.get_by_name(self.service_name).get("pub_events",[]))

        req_services = work_service.get("req_methods", [])
        out_dict["req_methods"] = self._generate_req_methods(req_services)

        out_dict["pub_methods"]=self.service.get_by_id(out_dict["service_id"]).get("pub_methods",[])

        out_dict["db"]=self._generate_db()

        out_dict["conf"] = work_service.get("conf",{})

        return out_dict
