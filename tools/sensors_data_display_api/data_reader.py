import json
import logging
import struct
import time
import socket

from some_ip_header import SomeIPHeader


class DataReader:
    def __init__(self, gui, filename="someip.json"):
        self.gui = gui
        self.data = 0
        with open(filename, encoding='UTF-8') as f:
            self.config = json.load(f)
        self.lookup_table = self._prepare_lookup_table()
        self.ip = self.config["interface"][0].get('ip')
        self.port = self.config["interface"][0].get("port")

    def _prepare_lookup_table(self):
        lookup_table = {}
        for key, value in self.config['req_events'].items():
            service_id = value.get('service_id')
            method_id = value.get('event_id')
            lookup_table[(service_id, method_id)] = key
        return lookup_table
    
    def get_service_id(self, request_name):
        req_methods = self.config.get('req_methods', {})
        if request_name in req_methods:
            return req_methods[request_name]['service_id']
        return None, None
    
    def get_method_id(self, request_name):
        req_methods = self.config.get('req_methods', {})
        if request_name in req_methods:
            return req_methods[request_name]['method_id']
        return None, None

    def read_data(self):
        with socket.socket(socket.AF_INET, socket.SOCK_DGRAM) as sock:
            try:
                sock.bind((self.ip, self.port))
            except:
                logging.critical("Port is taken")
            while True:
                if self.gui.stop_reading:
                    break
                data = sock.recv(150)
                if len(data) == 0:
                    break
                some_ip_header = SomeIPHeader()
                some_ip_header.read(data)

                # get service_id, method_id and payload
                service_id = some_ip_header.service_id
                method_id = some_ip_header.methode_id
                payload = some_ip_header.raw_payload

                # get data type
                data_type = self.lookup_table.get((service_id, method_id))
                print(f'service_id: {service_id}, method_id: {method_id}, payload: {payload}, data_type: {data_type}')
                if data_type is not None:
                    if data_type == 'PC_APP/newTempEvent_1':
                        # temperature up
                        temperature_up = float(struct.unpack('<H', payload)[0]/10)
                        self.gui.data_to_save.temperature_up = temperature_up
                        self.gui.temperature_up.config(text=str(round(temperature_up, 2)))
                    elif data_type == 'PC_APP/newTempEvent_2':
                        # temperature down
                        temperature_down = float(struct.unpack('<H', payload)[0]/10)
                        self.gui.data_to_save.temperature_down = temperature_down
                        self.gui.temperature_down.config(text=str(round(temperature_down, 2)))
                    elif data_type == 'PC_APP/newTempEvent_3':
                        # temperature middle
                        temperature_middle = float(struct.unpack('<H', payload)[0]/10)
                        self.gui.data_to_save.temperature_middle = temperature_middle
                        self.gui.temperature_middle.config(text=str(round(temperature_middle, 2)))
                    elif data_type == 'PC_APP/newTankPressEvent':
                        # tank pressure
                        tank_pressure = float(struct.unpack('f', payload)[0])
                        self.gui.data_to_save.tank_pressure = tank_pressure
                        self.gui.tank_pressure.config(text=str(round(tank_pressure, 2)))
                    elif data_type == 'PC_APP/servoStatusEvent':
                        # main valve
                        main_valve = payload.decode('UTF-8', errors='ignore')
                        self.gui.data_to_save.main_valve = main_valve
                        self.gui.main_valve.config(text=main_valve)
                    elif data_type == 'PC_APP/newDPressEvent':
                        # pressure difference
                        pressure_difference = float(struct.unpack('f', payload)[0])
                        self.gui.data_to_save.pressure_difference = pressure_difference
                        self.gui.pressure_difference.config(text=str(round(pressure_difference, 2)))
                else:
                    print("No data")
                time.sleep(0.01)
