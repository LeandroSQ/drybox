from realtime_input import RealtimeInput
from realtime_serial import RealtimeSerial
from realtime_plot import RealtimePlot
import sys
from time import time
from serial.tools.list_ports import comports

# Constants
config = {
    "Current Fan speed": {
        "color": "#227093",
        "suffix": " PWM",
        "line": "--"
    },
    "Fan speed": {
        "color": "#34ace0",
        "suffix": "%",
        "line": "-"
    },
    "Hotend temperature": {
        "color": "#ff793f",
        "suffix": "°C",
        "line": "-"
    },
    "Heater setpoint": {
        "color": "#cd6133",
        "suffix": "°C",
        "line": "--"
    },
    "Heater output": {
        "color": "#ff5252",
        "suffix": "%",
        "line": "-"
    },
    "Box temperature": {
        "color": "#706fd3",
        "suffix": "°C",
        "line": "-"
    },
    "Box humidity": {
        "color": "#d1ccc0",
        "suffix": "%",
        "line": "-"
    },
    "Box absolute humidity": {
        "color": "#84817a",
        "suffix": "g/m³",
        "line": "-"
    },
    "Box water vapor mass": {
        "color": "#2c3e50",
        "suffix": "g",
        "line": "-"
    },
}

# Validate args
if len(sys.argv) < 2:
    print("Usage: python3 plot.py <port>")

    print("Available ports:")
    for port in comports():
        print(port)

    sys.exit(1)



# Initialize the serial and input
serial = RealtimeSerial(sys.argv[1])
input = RealtimeInput()

def handle_serial_input():
    timestamp = time()
    data = serial.buffer.pop(0)

    # Parse the data
    values = data.split(",")
    values = [float(value) for value in values]

    # Convert it so the plot can understand it
    plot_data = { "time": timestamp }
    for i, column in enumerate(config.keys()):
        plot_data[column] = values[i]

    return plot_data

def handle_user_input():
    # Get the input
    command = input.buffer.pop(0)

    # Validate the input
    if command == "exit":
        sys.exit(0)

    # FAN <value>
    if command.startswith("fan"):
        parts = command.split(" ")
        if len(parts) == 2:
            # Ensure the value is between 0 and 255
            if int(parts[1]) != -1 and (int(parts[1]) < 0 or int(parts[1]) > 255):
                print("Value must be between 0 and 255")
                return
            serial.write(f"FAN {parts[1]}\n")
            print("Sent!")
        else:
            print("Usage: fan <VALUE>")

    # HEATER <HEATER OR FAN> <KP> <KI> <KD>
    if command.startswith("pid"):
        parts = command.split(" ")
        if len(parts) == 5:


            serial.write(f"PID {parts[1]} {parts[2]} {parts[3]} {parts[4]}\n")
            print("Sent!")
        else:
            print("Usage: PID <KP> <KI> <KD> <FF>")

    # SETPOINT <TEMP>
    if command.startswith("setpoint"):
        parts = command.split(" ")
        if len(parts) == 2:
            serial.write(f"SETPOINT {parts[1]}\n")
            print("Sent!")
        else:
            print("Usage: setpoint <TEMP>")

    # HEATER <VALUE>
    if command.startswith("heater"):
        parts = command.split(" ")
        if len(parts) == 2:
            # Ensure the value is between 0 and 255
            if int(parts[1]) != -1 and (int(parts[1]) < 0 or int(parts[1]) > 255):
                print("Value must be between 0 and 255")
                return
            serial.write(f"HEATER {parts[1]}\n")
            print("Sent!")
        else:
            print("Usage: heater <VALUE>")

def callback():
    # Check input
    if len(input.buffer) > 0:
        handle_user_input()

    # Check serial
    if len(serial.buffer) > 0:
        return handle_serial_input()

    return None

plot = RealtimePlot(callback, config, 2500, 120)
plot.show()