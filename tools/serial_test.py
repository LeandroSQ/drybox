# Simple python script to connect to an arduino at a given port and send a message to it
# And save all the data that the arduino sends back to a file

#pip3 install pyserial

import serial
import sys
import glob
import time

# Get the port from the command line
if len(sys.argv) < 2:
    print("Usage: python3 serial.py <port>")
    sys.exit(1)
port = sys.argv[1]
print("Connecting to port: " + port)

# Connect to the arduino
ser = serial.Serial(port, 9600)
print("Connected to arduino")

# Loop
while True:
    try:
        # Read the response from the arduino
        response = ser.readline().decode()
        print("Received: " + response)

        # Save the response to a file
        with open("data.txt", "a") as file:
            timestamp = time.strftime("%Y-%m-%d %H:%M:%S")
            file.write(timestamp + ": " + response)
    except Exception as e:
        print("An error occurred")
        print(e)
        break

# Close the connection
ser.close()