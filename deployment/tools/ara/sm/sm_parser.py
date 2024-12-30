import json
from deployment.tools.ara.sm.state import State, SwitchTransition, EmptyTransition, Transition, Mode, DtcTransition, MultipleTransition
from deployment.tools.ara.sm.db import DB
class SmParser:
    def ParseMultipleItem(json_o) -> Transition:
        list = []
        for n,v in json_o.items():
            list.append(SmParser.ParseItemValue(n,v))
        return MultipleTransition(list)
    def ParseDiDItem(json_o) -> dict[int,Transition]:
        list = {}
        for k,v in json_o.items():
            list[int(k,16)] = SmParser.ParseItem(v)
        return list
    def ParseItem(json_o) -> Transition:
        if len(json_o) == 0:
            raise Exception("Not supported")
        if len(json_o) > 1:
            return SmParser.ParseMultipleItem(json_o)
        if "switch" in json_o:
            return SwitchTransition(json_o["switch"])
        elif "dtc" in json_o:
            return DtcTransition(json["dtc"])
        else:
            return EmptyTransition()
    def ParseItemValue(name,value) -> Transition:
        if name == "switch":
            return SwitchTransition(value)
        elif name == "dtc":
            return DtcTransition(value)
    def ParseMode(json_o) -> Mode:
        return Mode(json_o["reset"],json_o["shutdown"])
    
    def Read(json_o, db_:DB):
        if "package" in json_o:
            db_.package = json_o["package"]
        for k, v in json_o["states"].items():
            after_finish = EmptyTransition()
            error = EmptyTransition()
            did = {}
            if "AfterFinish" in v:
                after_finish = SmParser.ParseItem(v["AfterFinish"])
            if "Error" in v:
                error = SmParser.ParseItem(v["Error"])
            if "DiD" in v:
                did = SmParser.ParseDiDItem(v["DiD"])
            mode = SmParser.ParseMode(v["mode"])
            print(k)
            db_.AddState(State(k,mode,after_finish,error,did))