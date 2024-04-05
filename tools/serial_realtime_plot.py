import time
import matplotlib.pyplot as plt
import matplotlib.patches as mpatches
import pandas as pd
import serial
import sys
import traceback
import threading

# Read the input from stdin without blocking
def read_input_callback(buffer):
    while True:
        try:
            buffer.append(input().lower())
        except KeyboardInterrupt:
            break

def list_ports():
    # List all available ports
    ports = serial.tools.list_ports.comports()
    for port in ports:
        print(port.device)

def handle_commands(connection, command):
    # FAN <value>
    if command.startswith("fan"):
        parts = command.split(" ")
        if len(parts) == 2:
            # Ensure the value is between 0 and 255
            if int(parts[1]) != -1 and (int(parts[1]) < 0 or int(parts[1]) > 255):
                print("Value must be between 0 and 255")
                return
            connection.write(f"FAN {parts[1]}\n".encode())
            print("Sent!")
        else:
            print("Usage: fan <VALUE>")

    # HEATER <HEATER OR FAN> <KP> <KI> <KD>
    if command.startswith("pid"):
        parts = command.split(" ")
        if len(parts) == 5:
            target = parts[1].lower()
            if target != "heater" and target != "fan":
                print("Target must be either HEATER or FAN")
                return

            target = "0" if target == "heater" else "1"
            connection.write(f"PID {target} {parts[2]} {parts[3]} {parts[4]}\n".encode())
            print("Sent!")
        else:
            print("Usage: PID <TARGET> <KP> <KI> <KD>")

    # SETPOINT <TEMP>
    if command.startswith("setpoint"):
        parts = command.split(" ")
        if len(parts) == 2:
            connection.write(f"SETPOINT {parts[1]}\n".encode())
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
            connection.write(f"HEATER {parts[1]}\n".encode())
            print("Sent!")
        else:
            print("Usage: heater <VALUE>")

def loop(connection):
    max_entries = 2000

    data = pd.DataFrame(columns = [
        "timestamp",
        "Current Fan speed",
        "Fan speed",
        "Hotend temperature",
        "Heater setpoint",
        "Heater output",
        "Box temperature",
        "Box humidity",
        "Box absolute humidity",
        "Box water vapor mass"
    ])

    # Set colors for each column
    colors = {
        "Current Fan speed": "#227093",
        "Fan speed": "#34ace0",
        "Hotend temperature": "#ff793f",
        "Heater setpoint": "#cd6133",
        "Heater output": "#ff5252",
        "Box temperature": "#706fd3",
        "Box humidity": "#d1ccc0",
        "Box absolute humidity": "#84817a",
        "Box water vapor mass": "#2c3e50"
    }

    # Set data suffixes for each column
    suffixes = {
        "Current Fan speed": " PWM",
        "Fan speed": "%",
        "Hotend temperature": "°C",
        "Heater setpoint": "°C",
        "Heater output": "%",
        "Box temperature": "°C",
        "Box humidity": "%",
        "Box absolute humidity": "g/m³",
        "Box water vapor mass": "g"
    }

    plt.ion()
    fig = plt.figure(figsize=(10, 6), num="DRYBOX")
    fig.suptitle("Real time IO Plot console")
    ax = fig.add_subplot(111)

    # Create a thread to read the input from stdin
    buffer = []
    input_thread = threading.Thread(target=read_input_callback, args=(buffer,), daemon=True).start()

    while True:
        try:
            if not connection.is_open:
                break

            fig.canvas.flush_events()
            # Check if the plot window has been closed
            if not plt.fignum_exists(fig.number):
                break

            if connection.in_waiting == 0:
                continue

            # Check if there is any input from stdin
            if buffer:
                command = buffer.pop(0)
                if command == "exit":
                    break

                handle_commands(connection, command)

            # Read incoming data
            response = connection.readline().decode()
            if not response or (not response[0].isdigit() and response[0] != "-"):
                print("RAW: " + response)
                continue
            timestamp = time.strftime("%Y-%m-%d %H:%M:%S")

            # Parse data into pandas DataFrame
            values = response.split(",")
            values = [float(value) for value in values]
            new_row = { "timestamp": pd.to_datetime(timestamp) }
            for i, col in enumerate(data.columns[1:]):
                new_row[col] = values[i]
            # data = data.append(new_row, ignore_index=True)
                # Using concat
            data = pd.concat([data, pd.DataFrame(new_row, index=[0])], ignore_index=True)

            # Rotate the data array if it is too big
            if len(data) > max_entries:
                data = data.tail(max_entries)

            # Plot the data
            ax.clear()
            ax.plot(data["timestamp"], data["Current Fan speed"], label="Current Fan speed", color=colors["Current Fan speed"], linestyle="--")
            ax.plot(data["timestamp"], data["Fan speed"], label="Fan speed", color=colors["Fan speed"])
            ax.plot(data["timestamp"], data["Hotend temperature"], label="Hotend temperature", color=colors["Hotend temperature"])
            ax.plot(data["timestamp"], data["Heater setpoint"], label="Heater setpoint", color=colors["Heater setpoint"], linestyle="--")
            ax.plot(data["timestamp"], data["Heater output"], label="Heater output", color=colors["Heater output"])
            ax.plot(data["timestamp"], data["Box temperature"], label="Box temperature", color=colors["Box temperature"])
            ax.plot(data["timestamp"], data["Box humidity"], label="Box humidity", color=colors["Box humidity"])
            ax.plot(data["timestamp"], data["Box absolute humidity"], label="Box absolute humidity", color=colors["Box absolute humidity"])
            ax.plot(data["timestamp"], data["Box water vapor mass"], label="Box water vapor mass", color=colors["Box water vapor mass"])

            # Annotate last values
            # for column in data.columns[1:]:
            #     ax.annotate(
            #         f'{data[column].iloc[-1]:.2f}',
            #         (data["timestamp"].iloc[-1], data[column].iloc[-1]),
            #         textcoords="offset points",
            #         xytext=(0,10),
            #         ha='center'
            #     )

            # Annotate with the column name, at the beginning of the plot
            for column in data.columns[1:]:
                ax.annotate(
                    column,
                    (data["timestamp"].iloc[0], data[column].iloc[0]),
                    textcoords="offset points",
                    xytext=(0,10),
                    ha='center',
                    color=colors[column],
                    fontsize=8
                )

            # Annotate legend with last values
            patches = []
            for column in data.columns[1:]:
                patch = mpatches.Patch(color=colors[column], label=f'{column}: {data[column].iloc[-1]:.2f}{suffixes[column]}')
                patches.append(patch)
            ax.legend(handles=patches, loc="upper left")

            ax.set_xlabel("Time")
            ax.set_ylabel("Value")

            fig.canvas.draw()
            time.sleep(0.1)
        except KeyboardInterrupt:
            break
        except Exception as e:
            # Traceback error
            print("An error occurred")
            print(e)
            traceback.print_exc()
            break

def main():
    # Validate args
    if len(sys.argv) < 2:
        print("Usage: python3 serial_realtime_plot.py <port>")
        print("Available ports:")
        list_ports()
        sys.exit(1)

    # Connect to the arduino
    port = sys.argv[1]
    connection = serial.Serial(port, 9600)
    print("Connected to arduino!")

    loop(connection)

    connection.close()

if __name__ == "__main__":
    main()