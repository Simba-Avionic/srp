from __future__ import annotations
from deployment.tools.ara.common.singleton_lib import SingletonMeta
from deployment.tools.ara.app.adaptive_application import AdaptiveApplication

class AdaptiveApplicationDb(metaclass=SingletonMeta):
    def __init__(self) -> None:
        self.app_list: dict[str,AdaptiveApplication] = {}