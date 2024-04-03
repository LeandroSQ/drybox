# Read csv file and plot data
# The file has the following format:
# 2024-03-25 17:28:49: 0.00,40.79,45.00,67.42,0.00,0.00,0.00
# Parse first the timestamp and then the values

import pandas as pd
import matplotlib.pyplot as plt

# Read the data from the file
data = []
with open("test.txt", "r") as file:
    lines = file.readlines()

    # Parse the data
    for line in lines:
        parts = line.split(": ")
        timestamp = parts[0]
        values = parts[1].split(",")
        values = [float(value) for value in values]
        data.append([timestamp] + values)

# Convert the data to a pandas DataFrame
data = pd.DataFrame(data)

# it does not have a header, so we need to specify the column names
data.columns = [
    "timestamp",
    "Fan speed",
    "Hotend temperature",
    "Heater setpoint",
    "Heater output",
    "Box temperature",
    "Box humidity",
    "Box absolute humidity",
]

# Convert the timestamp to a datetime object
data["timestamp"] = pd.to_datetime(data["timestamp"])

# Plot the data
plt.figure(figsize=(10, 6))
plt.plot(data["timestamp"], data["Fan speed"], label="Fan speed")
plt.plot(data["timestamp"], data["Hotend temperature"], label="Hotend temperature")
plt.plot(data["timestamp"], data["Heater setpoint"], label="Heater setpoint")
plt.plot(data["timestamp"], data["Heater output"], label="Heater output")
plt.plot(data["timestamp"], data["Box temperature"], label="Box temperature")
plt.plot(data["timestamp"], data["Box humidity"], label="Box humidity")
plt.plot(data["timestamp"], data["Box absolute humidity"], label="Box absolute humidity")
plt.xlabel("Time")
plt.ylabel("Value")
plt.legend()
plt.show()
