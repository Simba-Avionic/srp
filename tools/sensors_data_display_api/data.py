class Data:
    def __init__(self):
        self.temperature_up: float = None
        self.temperature_middle: float = None
        self.temperature_down: float = None
        self.tank_pressure: float = None
        self.pressure_difference: float = None
        self.main_valve: int = None
        # not given as for now
        self.jet_pressure: float = None
        self.vent: int = None

    def is_none(self):
        if self.temperature_up is None or self.temperature_middle is None or self.temperature_down is None or self.tank_pressure is None or self.main_valve is None or self.pressure_difference is None:
            return True
        return False
