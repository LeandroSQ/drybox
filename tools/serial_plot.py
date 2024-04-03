import time
import matplotlib.pyplot as plt
import os
import pandas as pd

# Detect changes in file
FILENAME = "data.txt"
last_modified = 0
last_file_size = 0

data = pd.DataFrame(columns = [
    "timestamp",
    "Fan speed",
    "Hotend temperature",
    "Heater setpoint",
    "Heater output",
    "Box temperature",
    "Box humidity",
    "Box absolute humidity",
])

def getNewLines():
    global last_file_size
    with open(FILENAME, "r") as file:
        # Get the current file size
        file.seek(0, os.SEEK_END)
        file_size = file.tell()
        file.seek(last_file_size)

        # Read the new lines from the file after the last read
        lines = file.readlines()
        last_file_size = file_size
        return lines


def parseFile():
    global data
    lines = getNewLines()

    # Rotate the data array if it is too big
    if len(data) > 100:
        data = data.tail(100)

    # Parse the data
    for line in lines:
        if len(line) < 5:
            continue

        parts = line.split(": ")
        timestamp = pd.to_datetime(parts[0])
        values = parts[1].split(",")
        values = [float(value) for value in values]
        new_row = {"timestamp": timestamp}
        for i, col in enumerate(data.columns[1:]): # Fill values to corresponding columns
            new_row[col] = values[i]
        data = data.append(new_row, ignore_index=True)


    # Convert the data to a pandas DataFrame
    # data = pd.DataFrame(data)
    # return data

# The plot image will be updated every second
plt.ion()
fig = plt.figure(figsize=(10, 6))
ax = fig.add_subplot(111)

# Loop
while True:
    try:
        # Check if the file has been modified
        modified = os.path.getmtime(FILENAME)
        if modified > last_modified:
            last_modified = modified

            # Parse the file
            parseFile()

            # Plot the data
            ax.clear()
            ax.plot(data["timestamp"], data["Fan speed"], label="Fan speed")
            ax.plot(data["timestamp"], data["Hotend temperature"], label="Hotend temperature")
            ax.plot(data["timestamp"], data["Heater setpoint"], label="Heater setpoint")
            ax.plot(data["timestamp"], data["Heater output"], label="Heater output")
            ax.plot(data["timestamp"], data["Box temperature"], label="Box temperature")
            ax.plot(data["timestamp"], data["Box humidity"], label="Box humidity")
            ax.plot(data["timestamp"], data["Box absolute humidity"], label="Box absolute humidity")

            # Annotate last values
            for column in data.columns[1:]:
                ax.annotate(f'{data[column].iloc[-1]:.2f}',
                            (data["timestamp"].iloc[-1], data[column].iloc[-1]),
                            textcoords="offset points",
                            xytext=(0,10),
                            ha='center')

            ax.set_xlabel("Time")
            ax.set_ylabel("Value")
            ax.legend()
            fig.canvas.draw()
            fig.canvas.flush_events()

            time.sleep(0.1)
    except Exception as e:
        print("An error occurred")
        print(e)
        break

    time.sleep(1)