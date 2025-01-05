from __future__ import annotations

import copy

from deployment.tools.ara.sm.state import State
from deployment.tools.ara.common.singleton_lib import SingletonMeta

class DB(metaclass=SingletonMeta):
    def __init__(self):
        self.states: dict[str,State] = {}
        self.hash_list: list[int] = []
        self.package = ""
        self.init_id = 0;
    def AddState(self, state:State) -> bool:
        if state.name in self.states:
            return False
        if state.id in self.hash_list:
            raise Exception("Hash error")
        self.states[state.name] = copy.copy(state)
        self.hash_list.append(state.id)
        return True