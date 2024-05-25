import csv
import datetime
import threading
import time
import tkinter as tk
from tkinter import messagebox
import logging

from data import Data
from data_reader import DataReader
from data_sender import DataSender


class App(tk.Tk):
    def __init__(self):
        super().__init__()

        # set properties of the window
        self.title("Engine")
        self.window_width = 600
        self.window_height = 600
        self.screen_width = self.winfo_screenwidth()
        self.screen_height = self.winfo_screenheight()
        self.padding = 5

        center_x = int(self.screen_width / 2 - self.window_width / 2)
        center_y = int(self.screen_height / 2 - self.window_height / 2)

        self.geometry(f'{self.window_width}x{self.window_height}+{center_x}+{center_y}')
        # self.configure(background='#B9B4C7')

        # variables
        label_width = 20
        button_padding = 2
        self.stop_reading = False
        self.saving = False
        self.data_reader_thread = None
        self.saving_thread = None
        self.data_to_save = Data()
        self.collection_time = 10

        # measurements
        temperature_up_frame = tk.Frame(self)
        temperature_up_label = tk.Label(temperature_up_frame, text='Temperature up:', width=label_width)
        self.temperature_up = tk.Label(temperature_up_frame, text='0', width=label_width)
        self.temperature_up.config(state=tk.DISABLED)

        temperature_down_frame = tk.Frame(self)
        temperature_down_label = tk.Label(temperature_down_frame, text='Temperature down:', width=label_width)
        self.temperature_down = tk.Label(temperature_down_frame, text='0', width=label_width)
        self.temperature_down.config(state=tk.DISABLED)

        temperature_middle_frame = tk.Frame(self)
        temperature_middle_label = tk.Label(temperature_middle_frame, text='Temperature middle:', width=label_width)
        self.temperature_middle = tk.Label(temperature_middle_frame, width=label_width, text='0')
        self.temperature_middle.config(state=tk.DISABLED)

        tank_pressure_frame = tk.Frame(self)
        tank_pressure_label = tk.Label(tank_pressure_frame, text='Tank pressure', width=label_width)
        self.tank_pressure = tk.Label(tank_pressure_frame, width=label_width, text='0')
        self.tank_pressure.config(state=tk.DISABLED)

        jet_pressure_frame = tk.Frame(self)
        jet_pressure_label = tk.Label(jet_pressure_frame, text='Jet pressure', width=label_width)
        self.jet_pressure = tk.Label(jet_pressure_frame, width=label_width, text='0')
        self.jet_pressure.config(state=tk.DISABLED)

        pressure_difference_frame = tk.Frame(self)
        pressure_difference_label = tk.Label(pressure_difference_frame, text='Pressure difference:', width=label_width)
        self.pressure_difference = tk.Label(pressure_difference_frame, width=label_width, text='0')
        self.pressure_difference.config(state=tk.DISABLED)

        main_valve_frame = tk.Frame(self)
        main_valve_label = tk.Label(main_valve_frame, text='Main valve:', width=label_width)
        self.main_valve = tk.Label(main_valve_frame, width=label_width, text='not given')
        self.main_valve.config(state=tk.DISABLED)

        vent_frame = tk.Frame(self)
        vent_label = tk.Label(vent_frame, text='Vent:', width=label_width)
        self.vent = tk.Label(vent_frame, width=label_width, text='not given')
        self.vent.config(state=tk.DISABLED)

        # buttons
        start_stop_saving_button_frame = tk.Frame(self)
        self.start_saving_button = tk.Button(start_stop_saving_button_frame, text="Start saving data", command=lambda: self.start_save_to_file())
        self.stop_saving_button = tk.Button(start_stop_saving_button_frame, text="Stop saving data", command=lambda: self.stop_save_to_file())

        main_valve_button_frame = tk.Frame(self)
        main_valve_set_1_button = tk.Button(main_valve_button_frame, text="Main valve: set 1", command=lambda: self.set_main_valve(value=1))
        main_valve_set_0_button = tk.Button(main_valve_button_frame, text="Main valve: set 0", command=lambda: self.set_main_valve(value=0))

        vent_button_frame = tk.Frame(self)
        vent_set_1_button = tk.Button(vent_button_frame, text="Vent: set 1", command=lambda: self.set_vent(self, value=1))
        vent_set_0_button = tk.Button(vent_button_frame, text="Vent: set 0", command=lambda: self.set_vent(self, value=0))

        launch_rocket_button_frame = tk.Frame(self)
        launch_rocket_button = tk.Button(launch_rocket_button_frame, text="Launch rocket", command=lambda: self.launch_rocket(value=1))
        stop_launch_rocket_button = tk.Button(launch_rocket_button_frame, text="Stop launch rocket", command=lambda: self.launch_rocket(value=0))

        exit_button = tk.Button(self, text="Exit", command=lambda: self.exit())

        # pack on screen
        # pack measurements
        temperature_up_frame.pack(side=tk.TOP, pady=self.padding, fill='both')
        temperature_up_label.pack(side=tk.LEFT, pady=self.padding, fill='both')
        self.temperature_up.pack(side=tk.LEFT, pady=self.padding, fill='both')

        temperature_middle_frame.pack(side=tk.TOP, pady=self.padding, fill='both')
        temperature_middle_label.pack(side=tk.LEFT, pady=self.padding, fill='both')
        self.temperature_middle.pack(side=tk.LEFT, pady=self.padding, fill='both')

        temperature_down_frame.pack(side=tk.TOP, pady=self.padding, fill='both')
        temperature_down_label.pack(side=tk.LEFT, pady=self.padding, fill='both')
        self.temperature_down.pack(side=tk.LEFT, pady=self.padding, fill='both')

        tank_pressure_frame.pack(side=tk.TOP, pady=self.padding, fill='both')
        tank_pressure_label.pack(side=tk.LEFT, pady=self.padding, fill='both')
        self.tank_pressure.pack(side=tk.LEFT, pady=self.padding, fill='both')

        jet_pressure_frame.pack(side=tk.TOP, pady=self.padding, fill='both')
        jet_pressure_label.pack(side=tk.LEFT, pady=self.padding, fill='both')
        self.jet_pressure.pack(side=tk.LEFT, pady=self.padding, fill='both')

        pressure_difference_frame.pack(side=tk.TOP, pady=self.padding, fill='both')
        pressure_difference_label.pack(side=tk.LEFT, pady=self.padding, fill='both')
        self.pressure_difference.pack(side=tk.LEFT, pady=self.padding, fill='both')

        main_valve_frame.pack(side=tk.TOP, pady=self.padding, fill='both')
        main_valve_label.pack(side=tk.LEFT, pady=self.padding, fill='both')
        self.main_valve.pack(side=tk.LEFT, pady=self.padding, fill='both')

        vent_frame.pack(side=tk.TOP, pady=self.padding, fill='both')
        vent_label.pack(side=tk.LEFT, pady=self.padding, fill='both')
        self.vent.pack(side=tk.LEFT, pady=self.padding, fill='both')

        # pack buttons
        main_valve_button_frame.pack(side=tk.TOP, pady=button_padding, fill='both')
        main_valve_set_1_button.pack(side=tk.LEFT, pady=button_padding, fill='both', expand=True)
        main_valve_set_0_button.pack(side=tk.LEFT, pady=button_padding, fill='both', expand=True)

        vent_button_frame.pack(side=tk.TOP, pady=button_padding, fill='both')
        vent_set_1_button.pack(side=tk.LEFT, pady=button_padding, fill='both', expand=True)
        vent_set_0_button.pack(side=tk.LEFT, pady=button_padding, fill='both', expand=True)

        start_stop_saving_button_frame.pack(side=tk.TOP, pady=button_padding, fill='both')
        self.start_saving_button.pack(side=tk.LEFT, pady=button_padding, fill='both', expand=True)
        self.stop_saving_button.pack(side=tk.LEFT, pady=button_padding, fill='both', expand=True)
        self.stop_saving_button.config(state=tk.DISABLED)

        launch_rocket_button_frame.pack(side=tk.TOP, pady=button_padding, fill='both')
        launch_rocket_button.pack(side=tk.LEFT, pady=button_padding, fill='both', expand=True)
        stop_launch_rocket_button.pack(side=tk.LEFT, pady=button_padding, fill='both', expand=True)

        exit_button.pack(side=tk.TOP, fill='both')

        self.data_reader = DataReader(gui=self)
        self.read_data()

        # initiate variable sender
        self.data_sender = DataSender(self.data_reader.ip, self.data_reader.port)

    def read_data(self):
        self.data_reader_thread = threading.Thread(target=self.data_reader.read_data, daemon=True)
        self.data_reader_thread.start()

    def start_save_to_file(self):
        self.saving = True
        self.start_saving_button.config(state=tk.DISABLED)
        self.stop_saving_button.config(state=tk.NORMAL)
        self.saving_thread = threading.Thread(target=self._save_to_file, daemon=True)
        self.saving_thread.start()

    def stop_save_to_file(self):
        self.saving = False
        self.start_saving_button.config(state=tk.NORMAL)
        self.stop_saving_button.config(state=tk.DISABLED)
        self.saving_thread.join()

    def _save_to_file(self):
        timestamp = datetime.datetime.now()
        filename = f"data_{timestamp.hour}_{timestamp.minute}_{timestamp.second}.csv"
        with open(file=filename, mode='w', encoding='UTF-8') as csvFile:
            writer = csv.writer(csvFile, delimiter=",")
            writer.writerow(['TIMESTAMP', 'TEMPERATURE_UP', 'TEMPERATURE_DOWN', 'TEMPERATURE_MIDDLE', 'TANK_PRESSURE', 'JET_PRESSURE', 'PRESSURE_DIFFERENCE', 'MAIN_VALVE', 'VENT'])
        start = time.time()
        while self.saving:
            logging.info("SAVING....")
            collection_time_difference = time.time() - start
            logging.info(f"Collection time difference: {collection_time_difference}")
            if (self.data_to_save.is_none() is False) or collection_time_difference > self.collection_time:
                logging.info("SAVED")
                timestamp = datetime.datetime.now()
                start = time.time()
                with open(filename, 'a', encoding='UTF-8', newline='') as csvFile:
                    writer = csv.writer(csvFile, delimiter=",")
                    writer.writerow([timestamp, self.data_to_save.temperature_up, self.data_to_save.temperature_down, self.data_to_save.temperature_middle,
                                     self.data_to_save.tank_pressure, self.data_to_save.jet_pressure, self.data_to_save.pressure_difference,
                                     self.data_to_save.main_valve, self.data_to_save.vent])
                self.data_to_save.temperature_up = None
                self.data_to_save.temperature_down = None
                self.data_to_save.temperature_middle = None
                self.data_to_save.tank_pressure = None
                self.data_to_save.jet_pressure = None
                self.data_to_save.pressure_difference = None
                self.data_to_save.main_valve = None
                self.data_to_save.vent = None
            else:
                logging.info("NOT SAVED")

    def launch_rocket(self, value):
        req_method="PC_APP/EngineStart"
        if value == 1:
            result = messagebox.askyesno('Launch rocket', 'Are you sure you want to launch rocket?')
            if result:
                self.data_sender.send_data(value=value, method_id=self.get_method_id(req_method), service_id=self.get_service_id(req_method))
        else:
            self.data_sender.send_data(value=value, method_id=self.get_method_id(req_method), service_id=self.get_service_id(req_method))

    def set_main_valve(self, value):
        req_method="PC_APP/setServoValue"
        self.data_sender.send_data(value=value, method_id=self.get_method_id(req_method), service_id=self.get_service_id(req_method))

    def set_vent(self, value):
        req_method="PC_APP/setVentValue"
        self.data_sender.send_data(value=value, method_id=self.get_method_id(req_method), service_id=self.get_service_id(req_method))

    def exit(self):
        self.stop_reading = True
        self.saving = False
        self.destroy()
