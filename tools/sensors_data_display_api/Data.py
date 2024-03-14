class Data:
    def __init__(self):
        self.temperature_up = None
        self.temperature_middle = None
        self.temperature_down = None
        self.tank_pressure = None
        self.jet_pressure = None
        self.pressure_difference = None
        self.main_valve = None
        self.vent_frame = None

    def is_None(self):
        if self.temperature_up is None or self.temperature_middle is None or self.temperature_down is None or self.tank_pressure is None or self.main_valve is None:
            return True
        return False
