import unittest
import sys
from pathlib import Path

sys.path.append(str(Path(__file__).resolve().parent.parent))

from data import Data

class TestDataClass(unittest.TestCase):
    def test_is_none(self):
        data = Data()
        self.assertTrue(data.is_none())
        data.jet_pressure = 12.2
        self.assertTrue(data.is_none())
        data.temperature_down=1
        data.temperature_middle=1
        data.temperature_up=1
        data.tank_pressure=1
        data.jet_pressure=1
        data.pressure_difference=1
        data.main_valve=1
        data.vent=1
        self.assertFalse(data.is_none())
