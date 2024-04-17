import unittest
from data_reader import DataReader
from unittest.mock import MagicMock, patch

import sys
from pathlib import Path

sys.path.append(str(Path(__file__).resolve().parent.parent))


class TestDataReader(unittest.TestCase):
    def test_init(self):
        dr = DataReader(None,"someip.json")
        self.assertEqual(dr.data,0)
        self.assertEqual(dr.ip,"127.0.0.1")
        self.assertEqual(dr.port,10101)
    def test_prepere_lookup_table(self):
        dr = DataReader(None,"someip.json")
        data = {(515, 32769): 'PC_APP/servoStatusEvent',
                (514, 32769): 'PC_APP/newTempEvent_1'}
        self.assertEqual(dr.lookup_table, data)