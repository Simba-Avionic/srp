import threading
import tkinter as tk

from data_reader import DataReader


class App(tk.Tk):
    def __init__(self):
        super().__init__()

        # set properties of the window
        self.title("Engine")
        self.window_width = 600
        self.window_height = 600
        self.screen_width = self.winfo_screenwidth()
        self.screen_height = self.winfo_screenheight()
        self.padding = 10

        center_x = int(self.screen_width / 2 - self.window_width / 2)
        center_y = int(self.screen_height / 2 - self.window_height / 2)

        self.geometry(f'{self.window_width}x{self.window_height}+{center_x}+{center_y}')
        # self.configure(background='#B9B4C7')

        # variables
        label_width = 20
        text_width = 50
        text_height = 1

        # measurements
        temperature_up_frame = tk.Frame(self)
        temperature_up_label = tk.Label(temperature_up_frame, text='Temperature up:', width=label_width)
        self.temperature_up = tk.Label(temperature_up_frame, text='0')
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

        # buttons
        button_frame = tk.Frame(self)
        start_saving_button = tk.Button(button_frame, text="Start saving data", command=lambda: self.start_saving())
        stop_saving_button = tk.Button(button_frame, text="Stop saving data", command=lambda: self.stop_saving())
        launch_rocket_button = tk.Button(button_frame, text="Launch rocket", command=lambda: self.launch_rocket())
        exit_button = tk.Button(button_frame, text="Exit", command=lambda: self.exit())

        # pack on screen
        temperature_up_frame.pack(side=tk.TOP, pady=self.padding, fill='both')
        temperature_up_label.pack(side=tk.LEFT, pady=self.padding, fill='both')
        self.temperature_up.pack(side=tk.LEFT, pady=self.padding, fill='both')

        temperature_down_frame.pack(side=tk.TOP, pady=self.padding, fill='both')
        temperature_down_label.pack(side=tk.LEFT, pady=self.padding, fill='both')
        self.temperature_down.pack(side=tk.LEFT, pady=self.padding, fill='both')

        temperature_middle_frame.pack(side=tk.TOP, pady=self.padding, fill='both')
        temperature_middle_label.pack(side=tk.LEFT, pady=self.padding, fill='both')
        self.temperature_middle.pack(side=tk.LEFT, pady=self.padding, fill='both')

        tank_pressure_frame.pack(side=tk.TOP, pady=self.padding, fill='both')
        tank_pressure_label.pack(side=tk.LEFT, pady=self.padding, fill='both')
        self.tank_pressure.pack(side=tk.LEFT, pady=self.padding, fill='both')

        jet_pressure_frame.pack(side=tk.TOP, pady=self.padding, fill='both')
        jet_pressure_label.pack(side=tk.LEFT, pady=self.padding, fill='both')
        self.jet_pressure.pack(side=tk.LEFT, pady=self.padding, fill='both')

        pressure_difference_frame.pack(side=tk.TOP, pady=self.padding, fill='both')
        pressure_difference_label.pack(side=tk.LEFT, pady=self.padding, fill='both')
        self.pressure_difference.pack(side=tk.LEFT, pady=self.padding, fill='both')

        button_frame.pack(side=tk.TOP, pady=self.padding, fill='both')
        start_saving_button.pack(side=tk.TOP, pady=self.padding, fill='both')
        stop_saving_button.pack(side=tk.TOP, pady=self.padding, fill='both')
        launch_rocket_button.pack(side=tk.TOP, pady=self.padding, fill='both')
        exit_button.pack(side=tk.TOP, pady=self.padding, fill='both')

        self.data_reader = DataReader(gui=self)
        self.read_data()

    def read_data(self):
        data_reader_thread = threading.Thread(target=self.data_reader.read_data)
        data_reader_thread.start()

    def update_data_label(self, data):
        # self.temperature_up.delete(0, tk.END)
        # self.temperature_up.insert(tk.END, data)
        self.temperature_up.config(text=str(data))

    def start_saving(self):
        pass

    def stop_saving(self):
        pass

    def launch_rocket(self):
        pass

    def exit(self):
        self.destroy()
