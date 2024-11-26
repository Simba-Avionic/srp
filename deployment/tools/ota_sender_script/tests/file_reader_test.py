import unittest
import os

from file_reader import FileReader

print(os.getcwd())
reader=FileReader()
data=reader.read_data_in_parts("test_tar.tar")
print(data)

class FileReaderTest(unittest.TestCase):
    def test_file_reader(self):
        #reader=FileReader()
        #data=reader.read_data_in_parts("//deployment/tools/ota_sender_script:test_tar.tar")
        pass

unittest.main()