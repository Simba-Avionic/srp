from  __future__ import annotations

from deployment.tools.ara.sm.state import State
from deployment.tools.ara.sm.db import DB
class CppExtractor:
    def CreateState(path:str,state:State,name_space:str = ""):
        with open(path+"/"+name_space.replace(".","/")+"/"+state.name+"_impl.h","w") as out:
            header_g = name_space.replace(".","_").upper()+"_"+state.name.upper()+"_IMPL_H_"
            kReset = ""
            kShutdown = ""
            if state.mode.restart:
                kReset = "true"
            else:
                kReset = "false"
            if state.mode.shutdown:
                kShutdown = "true"
            else:
                kShutdown = "false"
            did_c = ""
            for id,job in state.DiDDo.items():
                did_c+=f"""     case {id}:
            {job.GetCode(DB()).replace("\n","\n            ").rstrip()}
            break;"""
            out_s = f"""#ifndef {header_g}
#define {header_g}

#include "ara/exec/sm/state.h"

            """
            class_body = f"""
 public:
 
  {state.name}Impl() noexcept: ara::exec::sm::State{{kRestart, kShutdown}} {{}}
 
  void Finish() noexcept override {{
      {state.FinishDo.GetCode(DB()).replace("\n","\n      ").rstrip()}
  }}
  void Error() noexcept override {{
      {state.ErrorDo.GetCode(DB()).replace("\n","\n      ").rstrip()}
  }}
  void DiDHandler(const uint8_t val) noexcept override {{
    switch(val) {{
    {did_c}
        default:
            break;
    }};
  }}
"""
            
            out_s+=f"""
namespace {{
    constexpr bool kRestart{{{kReset}}};
    constexpr bool kShutdown{{{kShutdown}}};
}}  // namespace
class {state.name}Impl: public ara::exec::sm::State {{
{class_body}
}};
"""
            
            out_s+=f"""
#endif  // {header_g}
"""
            out.write(out_s)