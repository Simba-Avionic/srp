from __future__ import annotations
from deployment.tools.ara.someip.lib.service import *

class SomeipExtractor:
    def ExtractService(base_path:str, service:Service):
        if service.dir == "kOut":
            SomeipExtractor.ExtractServiceSkeletonBindings(base_path, service)
            SomeipExtractor.ExtractServiceSkeleton(base_path, service)
        if service.dir == "kIn":
            SomeipExtractor.ExtractServiceHandler(base_path, service)
        for m in service.methods:
            if service.dir == "kIn":
                SomeipExtractor.ExtractMethodProxy(base_path,service.name.replace(".","/"),m)
        for e in service.events:
            if service.dir == "kIn":
                SomeipExtractor.ExtractEventProxy(base_path,service.name.replace(".","/"),e)
            elif service.dir == "kOut":
                SomeipExtractor.ExtractEventSkeleton(base_path,service.name.replace(".","/"),e)
            
    def ExtractMethodProxy(base_path:str,service_path:str,method:Method):
        with open(base_path+"/"+service_path+"/"+method.name+"MethodProxy.h","w") as out_file:
            file = """#ifndef """+(service_path+"/"+method.name).replace("/","_").upper()+"_PROXY_H_\n"
            file += """#define """+(service_path+"/"+method.name).replace("/","_").upper()+"_PROXY_H_\n"
            file+="\n"
            file+="""#include <vector>\n"""
            file+="""#include <functional>\n"""
            file+="""#include <future>\n"""
            file+="""#include <thread>\n"""
            file+="""#include "ara/core/instance_specifier.h"\n"""
            file+="""#include "ara/core/result.h"\n"""
            file+="""#include "ara/com/com_error_domain.h"\n"""
            file+="""#include "ara/com/type_converter.h"\n"""
            file+="""#include "ara/com/someip/method_proxy.h"\n\n"""
            temp = ""
            if method.in_parm.typ_str == "struct":
                    temp = """#include \""""+method.in_parm.name.replace(".","/")+""".h\""""
                    if temp not in file:
                        file+=temp+"\n"
            else:
                    for i in method.in_parm.GetInclude():
                        if i not in file:
                            file+=i+"\n"
            if method.out_parm.typ_str == "struct":
                    temp = """#include \""""+method.out_parm.name.replace(".","/")+""".h\""""
                    if temp not in file:
                        file+=temp+"\n"
            else:
                    for i in method.out_parm.GetInclude():
                        if i not in file:
                            file+=i+"\n"
            file+="\n"
            namespace_list = service_path.split("/")
            file+="\n"
            for i in range(len(namespace_list)):
                file+="""namespace """+namespace_list[i]+""" {\n"""
                
            file+="class "+method.name+"Proxy: public ara::com::someip::MethodProxy {"
            file+="\n private:\n"
            
            if method.out_parm.typ_str == "struct":
                file+="  ara::core::Result<"+method.out_parm.name.replace(".","::")+"> Exec("
            else:
                file+="  ara::core::Result<"+method.out_parm.typ_str.replace(".","::")+"> Exec("
            if method.in_parm.typ_str == "void":
                file+="){\n"
                file+="    const auto res_vec = this->HandleMethod(std::vector<std::uint8_t>{});\n"
            else:
                if method.in_parm.typ_str == "struct":
                    file+="const "+method.in_parm.name.replace(".","::")+"& in_value) {\n"
                    file+="    const auto in_vec = ara::com::Convert2Vector<"+method.in_parm.name.replace(".","::")+">::Conv(in_value);\n"
                    file+="    const auto res_vec = this->HandleMethod(in_vec);\n"
                else:
                    file+="const "+method.in_parm.typ_str.replace(".","::")+"& in_value) {\n"
                    file+="    const auto in_vec = ara::com::Convert2Vector<"+method.in_parm.typ_str.replace(".","::")+">::Conv(in_value);\n"
                    file+="    const auto res_vec = this->HandleMethod(in_vec);\n"
            file+="    if(!res_vec.HasValue()){\n"
            file+="      return res_vec.Error();\n"
            file+="    }\n"
            if method.out_parm.typ_str == "void":
                file+="    return {};\n"
            else:
                if method.out_parm.typ_str == "struct":
                    file+="    const auto res = ara::com::Convert<"+method.out_parm.name.replace(".","::")+">::Conv(res_vec.Value());\n"
                else:
                    file+="    const auto res = ara::com::Convert<"+method.out_parm.typ_str.replace(".","::")+">::Conv(res_vec.Value());\n"
                file+="    if(res.has_value()) {\n"
                file+="      return res.value();\n"
                file+="    }\n"
                file+="    return ara::com::MakeErrorCode(ara::com::ComErrc::kFieldValueIsNotValid, \"Value converting error!\");\n"
            file+="  }\n"
            file+="\n"
            
            
            
            file+=" public:\n"
            file+="  "+method.name+"Proxy(): ara::com::someip::MethodProxy{"+hex(method.id)+"} {}\n"
            file+="  ~"+method.name+"Proxy() = default;\n"
            file+="\n"
            if method.out_parm.typ_str == "struct":
                file+="  ara::core::Result<"+method.out_parm.name.replace(".","::")+"> operator() ("
            else:
                file+="  ara::core::Result<"+method.out_parm.typ_str.replace(".","::")+"> operator() ("
            if method.in_parm.typ_str == "void":
                file+="){\n"
                file+="    return this->Exec();\n"
            else:
                if method.in_parm.typ_str == "struct":
                    file+="const "+method.in_parm.name.replace(".","::")+"& in_value) {\n"
                else:
                    file+="const "+method.in_parm.typ_str.replace(".","::")+"& in_value) {\n"
                file+="    return this->Exec(in_value);\n"
            file+="  }\n"
            file+="\n"
            if method.out_parm.typ_str == "struct":
                file+="  ara::core::Result<"+method.out_parm.name.replace(".","::")+"> Call("
            else:
                file+="  ara::core::Result<"+method.out_parm.typ_str.replace(".","::")+"> Call("
            if method.in_parm.typ_str == "void":
                file+="){\n"
                file+="    return this->Exec();\n"
            else:
                if method.in_parm.typ_str == "struct":
                    file+="const "+method.in_parm.name.replace(".","::")+"& in_value) {\n"
                else:
                    file+="const "+method.in_parm.typ_str.replace(".","::")+"& in_value) {\n"
                file+="    return this->Exec(in_value);\n"
            file+="  }\n"
            file+="\n"
            if method.out_parm.typ_str == "struct":
                file+="  std::future<ara::core::Result<"+method.out_parm.name.replace(".","::")+">> CallAsync("
            else:
                file+="  std::future<ara::core::Result<"+method.out_parm.typ_str.replace(".","::")+">> CallAsync("
            if method.in_parm.typ_str == "void":
                file+="){\n"
                file+="    return std::async(std::launch::async, [this] ->"
                if method.out_parm.typ_str == "struct":
                    file+=" ara::core::Result<"+method.out_parm.name.replace(".","::")+"> "
                else:
                    file+=" ara::core::Result<"+method.out_parm.typ_str.replace(".","::")+"> "
                file+="{\n"
                file+="      return this->Exec();\n"
            else:
                if method.in_parm.typ_str == "struct":
                    file+="const "+method.in_parm.name.replace(".","::")+"& in_value) {\n"
                    file+="    return std::async(std::launch::async, [&in_value, this] ->"
                    if method.out_parm.typ_str == "struct":
                        file+=" ara::core::Result<"+method.out_parm.name.replace(".","::")+"> "
                    else:
                        file+=" ara::core::Result<"+method.out_parm.typ_str.replace(".","::")+"> "
                    file+="{\n"
                    file+="      return this->Exec(in_value);\n"
                else:
                    file+="const "+method.in_parm.typ_str.replace(".","::")+"& in_value) {\n"
                    file+="    return std::async(std::launch::async, [&in_value, this] ->"
                    if method.out_parm.typ_str == "struct":
                        file+=" ara::core::Result<"+method.out_parm.name.replace(".","::")+"> "
                    else:
                        file+=" ara::core::Result<"+method.out_parm.typ_str.replace(".","::")+"> "
                    file+="{\n"
                    file+="      return this->Exec(in_value);\n"
            file+="    });\n"
            file+="  }\n"
            file+="\n"    
            

            
            
            file+="};\n"
            
            for i in range(len(namespace_list)):
                    file+="""}  // namespace """+namespace_list[i]+"""\n"""
            file+="\n"
            file += """#endif  // """+(service_path+"/"+method.name).replace("/","_").upper()+"_PROXY_H_\n"
            out_file.write(file)

    def ExtractEventProxy(base_path:str,service_path:str,event:Event):
        with open(base_path+"/"+service_path+"/"+event.name+"EventProxy.h","w") as out_file:
            file = """#ifndef """+(service_path+"/"+event.name).replace("/","_").upper()+"_PROXY_H_\n"
            file += """#define """+(service_path+"/"+event.name).replace("/","_").upper()+"_PROXY_H_\n"
            file+="""#include "ara/com/com_error_domain.h"\n"""
            file+="""#include "ara/com/type_converter.h"\n"""
            file+="""#include "ara/com/someip/event_proxy.h"\n"""
            if event.out_parm.typ_str == "struct":
                    temp = """#include \""""+event.out_parm.name.replace(".","/")+""".h\""""
                    if temp not in file:
                        file+=temp+"\n"
            else:
                    for i in event.out_parm.GetInclude():
                        if i not in file:
                            file+=i+"\n"
            namespace_list = service_path.split("/")
            file+="\n"
            for i in range(len(namespace_list)):
                file+="""namespace """+namespace_list[i]+""" {\n"""

            file+="class "+event.name+"EventProxy final: public ara::com::someip::EventProxy {\n"
            file+=" private:\n"
            if event.out_parm.typ_str == "struct":
                file+="  "+event.out_parm.name.replace(".","::")+" value_;\n"
            else:
                file+="  "+event.out_parm.typ_str.replace(".","::")+" value_;\n"
            file+=" public:\n"
            file+="  "+event.name+"EventProxy(): ara::com::someip::EventProxy{"+hex(event.id)+"} {}\n"
            file+="  void HandleEvent(const std::vector<uint8_t>& payload) override {\n"
            if event.out_parm.typ_str == "struct":
                file+="    const auto val = ara::com::Convert<"+event.out_parm.name.replace(".","::")+">::Conv(payload);\n"
            else:
                file+="    const auto val = ara::com::Convert<"+event.out_parm.typ_str.replace(".","::")+">::Conv(payload);\n"
            file+="    if(!val.has_value()) {\n"
            file+="      msg_recived = false;\n"
            file+="      return;\n"
            file+="    }\n"
            file+="    value_ = val.value();\n"
            file+="    msg_recived = true;\n"
            file+="    if(event_receive_handler_) {\n"
            file+="      event_receive_handler_();\n"
            file+="    }\n"
            file+="  }\n"
            file+="\n"
            if event.out_parm.typ_str == "struct":
                file+="  ara::core::Result<"+event.out_parm.name.replace(".","::")+"> GetNewSamples() {\n"
            else:
                file+="  ara::core::Result<"+event.out_parm.typ_str.replace(".","::")+"> GetNewSamples() {\n"
            
            file+="    if(status_ != SubscribeStatus::kSubscribe) {\n"
            file+="      return ara::com::MakeErrorCode(ara::com::ComErrc::kSetHandlerNotSet, \"Event not subscribed\");\n"
            file+="    }\n"
            file+="    if(!msg_recived) {\n"
            file+="      return ara::com::MakeErrorCode(ara::com::ComErrc::kFieldValueIsNotValid, \"Value not set\");\n"
            file+="    }\n"
            file+="    msg_recived = false;\n"
            file+="    return value_;\n"
            file+="  }\n"
            file+="};\n"
            for i in range(len(namespace_list)):
                    file+="""}  // namespace """+namespace_list[i]+"""\n"""
            
            file += """#endif  // """+(service_path+"/"+event.name).replace("/","_").upper()+"_PROXY_H_\n"
            out_file.write(file)
    def ExtractEventSkeleton(base_path:str,service_path:str,event:Event):
        with open(base_path+"/"+service_path+"/"+event.name+"EventSkeleton.h","w") as out_file:
            file = """#ifndef """+(service_path+"/"+event.name).replace("/","_").upper()+"_SKELETON_H_\n"
            file += """#define """+(service_path+"/"+event.name).replace("/","_").upper()+"_SKELETON_H_\n"
            file+="""#include "ara/com/com_error_domain.h"\n"""
            file+="""#include "ara/com/type_converter.h"\n"""
            file+="""#include "ara/com/someip/event_skeleton.h"\n"""
            if event.out_parm.typ_str == "struct":
                    temp = """#include \""""+event.out_parm.name.replace(".","/")+""".h\""""
                    if temp not in file:
                        file+=temp+"\n"
            else:
                    for i in event.out_parm.GetInclude():
                        if i not in file:
                            file+=i+"\n"
            namespace_list = service_path.split("/")
            file+="\n"
            for i in range(len(namespace_list)):
                file+="""namespace """+namespace_list[i]+""" {\n"""

            file+="class "+event.name+"EventSkeleton: public ara::com::someip::EventSkeleton {\n"
            file+=" public:\n"
            file+="  "+event.name+"EventSkeleton(): ara::com::someip::EventSkeleton{"+hex(event.id)+"} {}\n"
            file+="  void Update(const "
            if event.out_parm.typ_str == "struct":
                file+=event.out_parm.name.replace(".","::")
            else:
                file+=event.out_parm.typ_str
            file +="& new_value) const {\n"
            if event.out_parm.typ_str == "struct":
                file+="    const auto vec = ara::com::Convert2Vector<"+event.out_parm.name.replace(".","::")+">::Conv(new_value);\n"
            else:
                file+="    const auto vec = ara::com::Convert2Vector<"+event.out_parm.typ_str.replace(".","::")+">::Conv(new_value);\n"
            file+="    if(send_callback_) {\n"
            file+="      send_callback_(event_id_, vec);\n"
            file+="    }\n"
            file+="  }\n"
            file+="};\n"
            for i in range(len(namespace_list)):
                    file+="""}  // namespace """+namespace_list[i]+"""\n"""
            
            file += """#endif  // """+(service_path+"/"+event.name).replace("/","_").upper()+"_SKELETON_H_\n"
            out_file.write(file)
    def ExtractServiceSkeleton(base_path:str, service:Service):
        with open(base_path+"/"+service.name.replace(".","/")+"Skeleton.h","w") as out_file:
            namespace_list = service.name.split(".")
            file = """#ifndef """+(service.name.upper().replace(".","_"))+"_SKELETON_H_\n"
            file += """#define """+(service.name.upper().replace(".","_"))+"_SKELETON_H_\n"
            file+="\n"
            file+="""#include "ara/core/instance_specifier.h"\n"""
            file+="""#include "ara/core/result.h"\n"""
            file+="""#include "ara/com/com_error_domain.h"\n"""
            
            file+="""#include \""""+service.name.replace(".","/")+"""SkeletonBindings.h\"\n"""
            for m in service.methods:
                if m.in_parm.typ_str == "struct":
                    temp = "#include \""+m.in_parm.name.replace(".","/")+".h\""
                    if temp not in file:
                        file+=temp+"\n"
                else:
                    for i in m.in_parm.GetInclude():
                        if i not in file:
                            file+=i+"\n"
                if m.out_parm.typ_str == "struct":
                    temp = "#include \""+m.out_parm.name.replace(".","/")+".h\""
                    if temp not in file:
                        file+=temp+"\n"
                else:
                    for i in m.out_parm.GetInclude():
                        if i not in file:
                            file+=i+"\n"
            for e in service.events:
                file+="#include \""+service.name.replace(".","/")+"/"+e.name+"EventSkeleton.h\"\n"
            file+="\n"
            for i in range(len(namespace_list)-1):
                file+="""namespace """+namespace_list[i]+""" {\n"""
                
            file+="""class """+namespace_list[-1]+"""Skeleton: public """+namespace_list[-1]+"""SkeletonBindings {\n"""
            file+=""" public:\n"""
            file+="""  """+namespace_list[-1]+"""Skeleton(const ara::core::InstanceSpecifier& instance): """+namespace_list[-1]+"""SkeletonBindings{instance} {\n"""
            for e in service.events:
                file+="    "+e.name+".SetCallback(std::bind(&"+namespace_list[-1]+"Skeleton::HandleEvent,this, std::placeholders::_1, std::placeholders::_2));\n"
            file+="""   }\n"""
            file+="""  virtual ~"""+namespace_list[-1]+"""Skeleton() = default;\n"""
            file+="\n"
            for e in service.events:
                file+="  "+service.name.replace(".","::")+"::"+e.name+"EventSkeleton "+e.name+"{};"
            file+="\n"
            file+=""" protected:\n"""
            for m in service.methods:
                
                
                if m.out_parm.typ_str == "struct":
                    file+="""  virtual ara::core::Result<"""+m.out_parm.name.replace(".","::")+"> "+m.name
                else:
                    file+="""  virtual ara::core::Result<"""+m.out_parm.typ_str.replace(".","::")+"> "+m.name
                if m.in_parm.typ_str == "struct":
                    file+="(const "+m.in_parm.name.replace(".","::")+"& in_parm)"
                elif m.in_parm.typ_str!="void":
                    file+="(const "+m.in_parm.typ_str.replace(".","::")+"& in_parm)"
                else:
                    file+="()"
                file+=" override {return ara::com::MakeErrorCode(ara::com::ComErrc::kSetHandlerNotSet, \"Method handler is not set\");}\n"
               
            file+="""};\n"""
            for i in range(len(namespace_list)-1):
                    file+="""}  // namespace """+namespace_list[i]+"""\n"""
            file += """\n#endif  // """+(service.name.upper().replace(".","_"))+"_SKELETON_H_\n"
            out_file.write(file)
    def ExtractMethodBindings(m:Method) -> str:
        method_call = ""
        in_parm_convert = "{\n"
        out_parm_convert = ""
        if(m.in_parm.typ_str == "void"):
            method_call += "         const auto res = "+m.name+"();\n"
        else:
            in_parm_convert +="         const auto in_p = ara::com::Convert<"
            if( m.in_parm.typ_str == "struct"):
                in_parm_convert += m.in_parm.name.replace(".","::")
            else:
                in_parm_convert += m.in_parm.typ_str
            in_parm_convert += ">::Conv(data);\n"
            method_call += "         if(!in_p.has_value()) {\n"
            method_call += "           return ara::com::MakeErrorCode(ara::com::ComErrc::kFieldValueIsNotValid, \"\");\n"
            method_call += "         }\n"
            method_call += "         const auto res = "+m.name+"(in_p.value());\n"
        out_parm_convert+="         if(!res.HasValue()) {\n"
        out_parm_convert+="           return res.Error();\n"
        out_parm_convert+="         }\n"
        if(m.out_parm.typ_str == "void"):
            out_parm_convert+="         return std::vector<uint8_t>{};\n"
        elif(m.out_parm.typ_str == "struct"):
            out_parm_convert+="         return ara::com::Convert2Vector<"+m.out_parm.name.replace(".","::")+">::Conv(res.Value());\n"
        else:
            out_parm_convert+="         return ara::com::Convert2Vector<"+m.out_parm.typ_str+">::Conv(res.Value());\n"
        return in_parm_convert+method_call+out_parm_convert+"       }"     
    def ExtractServiceSkeletonBindings(base_path:str, service:Service):
        with open(base_path+"/"+service.name.replace(".","/")+"SkeletonBindings.h","w") as out_file:
            namespace_list = service.name.split(".")
            file = """#ifndef """+(service.name.upper().replace(".","_"))+"_SKELETON_BINDINGS_H_\n"
            file += """#define """+(service.name.upper().replace(".","_"))+"_SKELETON_BINDINGS_H_\n"
            file+="\n"
            file+="""#include <vector>\n"""
            file+="""#include "ara/core/instance_specifier.h"\n"""
            file+="""#include "ara/core/result.h"\n"""
            file+="""#include "ara/com/com_error_domain.h"\n"""
            file+="""#include "ara/com/type_converter.h"\n"""
            file+="""#include "ara/com/someip/service_skeleton.h"\n\n"""
            file+="""#include "ara/com/someip/message_code.h"\n"""
            
            for m in service.methods:
                if m.in_parm.typ_str == "struct":
                    temp = """#include \""""+m.in_parm.name.replace(".","/")+""".h\""""
                    if temp not in file:
                        file+=temp+"\n"
                else:
                    for i in m.in_parm.GetInclude():
                        if i not in file:
                            file+=i+"\n"
                if m.out_parm.typ_str == "struct":
                    temp = """#include \""""+m.out_parm.name.replace(".","/")+""".h\""""
                    if temp not in file:
                        file+=temp+"\n"
                else:
                    for i in m.out_parm.GetInclude():
                        if i not in file:
                            file+=i+"\n"
            
            file+="\n"
            for i in range(len(namespace_list)-1):
                file+="""namespace """+namespace_list[i]+""" {\n"""
                
            file+="""class """+namespace_list[-1]+"""SkeletonBindings: public ara::com::someip::ServiceSkeleton {\n"""
            file+=""" public:\n"""
            file+="""  """+namespace_list[-1]+"""SkeletonBindings(const ara::core::InstanceSpecifier& instance): ara::com::someip::ServiceSkeleton{instance, """+hex(service.id)+"""} {}\n"""
            file+="""  virtual ~"""+namespace_list[-1]+"""SkeletonBindings() = default;\n"""
            file+="\n"
            file+=""" protected:\n"""
            
            file+="""  ara::core::Result<std::vector<uint8_t>> HandleMethod(const uint16_t method_id, const std::vector<uint8_t>& data) override {\n"""
            file+="""    switch(method_id) {\n"""
            for m in service.methods:
                file+="""     case """+hex(m.id)+""":\n"""
                file+="""       """ + SomeipExtractor.ExtractMethodBindings(m) + "\n"
                file+="""     break;\n"""
            file+="""     default:
       return ara::com::MakeErrorCode(ara::com::someip::MessageCode::kEUnknownMethod, \"Method handler is not set\");\n """
            file+="""    }\n"""
            file+="""  }\n"""
            for m in service.methods:
                if m.out_parm.typ_str == "struct":
                    file+="""  virtual ara::core::Result<"""+m.out_parm.name.replace(".","::")+"> "+m.name
                else:
                    file+="""  virtual ara::core::Result<"""+m.out_parm.typ_str.replace(".","::")+"> "+m.name
                if m.in_parm.typ_str == "struct":
                    file+="(const "+m.in_parm.name.replace(".","::")+"& in_parm) = 0;\n"
                elif m.in_parm.typ_str!="void":
                    file+="(const "+m.in_parm.typ_str.replace(".","::")+"& in_parm) = 0;\n"
                else:
                    file+="() = 0;\n"
            file+="""};\n"""
            for i in range(len(namespace_list)-1):
                    file+="""}  // namespace """+namespace_list[i]+"""\n"""
            file += """\n#endif  // """+(service.name.upper().replace(".","_"))+"_SKELETON_BINDINGS_H_\n"
            out_file.write(file)
    def ExtractServiceHandler(base_path:str, service:Service):
        namespace_list = service.name.split(".")
        print(base_path+"/"+service.name.replace(".","/")+"/"+namespace_list[-1]+"Handler.h")
        with open(base_path+"/"+service.name.replace(".","/")+"/"+namespace_list[-1]+"Handler.h","w") as out_file:
            
            file = """#ifndef """+(service.name.upper().replace(".","_"))+"_"+namespace_list[-1].upper()+"_HANDLER_H_\n"
            file += """#define """+(service.name.upper().replace(".","_"))+"_"+namespace_list[-1].upper()+"_HANDLER_H_\n"
            file+="\n"
            file+="""#include <functional>\n"""
            file+="""#include "ara/core/instance_specifier.h"\n"""
            file+="""#include "ara/core/result.h"\n"""
            file+="""#include "ara/com/com_error_domain.h"\n"""
            file+="""#include "ara/com/someip/someip_handler.h"\n"""
            file+="""#include "ara/com/someip/service_proxy.h"\n"""
            
            
            
            for m in service.methods:
                file+="""#include \""""+service.name.replace(".","/")+"/"+m.name+"""MethodProxy.h\"\n"""
            for e in service.events:
                file+="""#include \""""+service.name.replace(".","/")+"/"+e.name+"""EventProxy.h\"\n"""
            file+="\n"
            for i in range(len(namespace_list)-1):
                file+="""namespace """+namespace_list[i]+""" {\n"""
                
            file+="""class """+namespace_list[-1]+"""Handler final: public ara::com::someip::ServiceHandler {\n"""
            file+=""" public:\n"""
            file+="""  explicit """+namespace_list[-1]+"""Handler(const ara::core::InstanceSpecifier& instance): ara::com::someip::ServiceHandler(instance,"""+hex(service.id)+""")  {\n"""
            for e in service.events:
                file+="    "+e.name+"._SetSubscriptionCallback(std::bind(&"+namespace_list[-1]+"Handler::SubscribeEvent, this, std::placeholders::_1));\n"
            for m in service.methods:
                file+="    "+m.name+"._SetCallCallback(std::bind(&"+namespace_list[-1]+"Handler::HandleMethod, this, std::placeholders::_1, std::placeholders::_2));\n"
            file+="  }\n"
            file+="\n"
            file+="""  virtual ~"""+namespace_list[-1]+"""Handler() = default;\n"""
            file+="\n"
            file+="""  void HandleEvent(const uint16_t event_id,const std::vector<uint8_t>& data) override {\n"""
            file+="""    switch(event_id) {\n"""
            for e in service.events:
                file+="""      case """+hex(e.id)+""" :\n"""
                file+="""        """+e.name+".HandleEvent(data);\n"
                file+="""        break;\n"""
            file+="""    }\n"""
            file+="""  }\n"""
            file+="\n"
            file+="  void SubscribeNewStatus(const uint16_t event_id_, const uint8_t new_status) override {\n"
            file+="""    switch(event_id_) {\n"""
            for e in service.events:
                file+="""      case """+hex(e.id)+""" :\n"""
                file+="""        """+e.name+"._SetSubscriptionState(new_status);\n"
                file+="""        break;\n"""
            file+="""    }\n"""
            file+="""  }\n"""
            file+="// METHOD SECTION\n"
            for m in service.methods:
                file+="""  """+service.name.replace(".","::")+"::"+m.name+"""Proxy """+m.name+"""{};\n"""
            file+="// EVENT SECTION\n"
            for m in service.events:
                file+="""  """+service.name.replace(".","::")+"::"+m.name+"""EventProxy """+m.name+"""{};\n"""
            file+="\n"
            file+="""};\n"""
            file +="""using """+namespace_list[-1]+"""Proxy = ara::com::someip::ServiceProxy<"""+namespace_list[-1]+"""Handler>;\n"""
            for i in range(len(namespace_list)-1):
                    file+="""}  // namespace """+namespace_list[i]+"""\n"""
            file += """\n#endif  // """+(service.name.upper().replace(".","_"))+"_"+namespace_list[-1].upper()+"_HANDLER_H_\n"
            out_file.write(file)