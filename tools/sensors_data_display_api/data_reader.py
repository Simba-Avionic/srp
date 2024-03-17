import json
import logging
import struct
import time
import socket

from some_ip_header import SomeIPHeader


class DataReader:
    def __init__(self, gui):
        self.gui = gui
        self.data = 0
        with open('someip.json') as f:
            self.config = json.load(f)
        self.lookup_table = self._prepare_lookup_table()

    def _prepare_lookup_table(self):
        lookup_table = {}
        for key, value in self.config['req_events'].items():
            service_id = value.get('service_id')
            method_id = value.get('event_id')
            lookup_table[(service_id, method_id)] = key
        return lookup_table

    def read_data(self):
        with socket.socket(socket.AF_INET, socket.SOCK_DGRAM) as sock:
            try:
                sock.bind(('127.0.0.1', 12345))
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
                print(f'service_id: {service_id}, method_id: {method_id}, payload: {payload}, data_ty[e: {data_type}')
                if data_type is not None:
                    if data_type == 'PC_APP/newTempEvent_1':
                        # temperature up
                        temperature_up = struct.unpack('f', payload)[0]
                        self.gui.data_to_save.temperature_up = temperature_up
                        self.gui.temperature_up.config(text=str(temperature_up))
                    elif data_type == 'PC_APP/newTempEvent_2':
                        # temperature down
                        temperature_down = struct.unpack('f', payload)[0]
                        self.gui.data_to_save.temperature_down = temperature_down
                        self.gui.temperature_down.config(text=str(temperature_down))
                    elif data_type == 'PC_APP/newTempEvent_3':
                        # temperature middle
                        temperature_middle = struct.unpack('f', payload)[0]
                        self.gui.data_to_save.temperature_middle = temperature_middle
                        self.gui.temperature_middle.config(text=str(temperature_middle))
                    elif data_type == 'PC_APP/newPressEvent':
                        # tank pressure
                        tank_pressure = struct.unpack('f', payload)[0]
                        self.gui.data_to_save.tank_pressure = tank_pressure
                        self.gui.tank_pressure.config(text=str(tank_pressure))
                    elif data_type == 'PC_APP/servoStatusEvent':
                        # main valve
                        main_valve = payload.decode('UTF-8', errors='ignore')
                        self.gui.data_to_save.main_valve = main_valve
                        self.gui.main_valve.config(text=main_valve)
                else:
                    print("No data")
                time.sleep(0.5)
