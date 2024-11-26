# base_class.py
import json
import logging


class BaseClass:
    data: dict
    data_loaded: bool

    def load_data(self, file_path):
        logging.info("load data from file")
        try:
            with open(file_path, "r",encoding="UTF-8") as file:
                logging.info("Class initialized successfully")
                return json.load(file)
        except FileNotFoundError:
            logging.warning("Failed initialize class")
