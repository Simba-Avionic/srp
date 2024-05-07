import unittest

from app import App
import logging

import sys
from pathlib import Path
import tkinter as tk
from unittest.mock import MagicMock

sys.path.append(str(Path(__file__).resolve().parent.parent))

logging.disable(60)

class TestAppInitialization(unittest.TestCase):

    def setUp(self):
        self.app = App()

    def test_window_properties(self):
        self.assertEqual(self.app.title(), "Engine")
        self.assertEqual(self.app.window_width, 600)
        self.assertEqual(self.app.window_height, 600)

    def test_elements_existence(self):
        self.assertIsNotNone(self.app.temperature_up)
        self.assertIsNotNone(self.app.temperature_down)
        self.assertIsNotNone(self.app.temperature_middle)
        self.assertIsNotNone(self.app.tank_pressure)
        self.assertIsNotNone(self.app.jet_pressure)
        self.assertIsNotNone(self.app.pressure_difference)
        self.assertIsNotNone(self.app.main_valve)
        self.assertIsNotNone(self.app.vent)
        self.assertIsNotNone(self.app.start_saving_button)

    def test_start_to_save_file(self):
        self.app.start_save_to_file()
        self.assertTrue(self.app.saving_thread.is_alive())

    def test_stop_save_to_file(self):
        self.app.start_save_to_file()
        self.app.stop_save_to_file()
        self.assertFalse(self.app.saving_thread.is_alive())
