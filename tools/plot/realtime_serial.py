from serial import Serial
from threading import Thread

class RealtimeSerial:

    def __init__(self, port, baudrate=9600):
        self.connection = Serial(port, baudrate)
        self.buffer = []
        self.thread = Thread(target=self._read_input_callback, daemon=True).start()

    def write(self, message: str):
        if not self.connection.is_open:
            print("Connection closed")
            return

        self.connection.write(message.encode("utf-8"))

    def _read_input_callback(self):
        while True:
            try:
                if not self.connection.is_open:
                    print("Connection closed")
                    break

                if self.connection.in_waiting <= 0:
                    continue

                incoming = self.connection.readline().decode("utf-8").strip()

                # Validate data
                if not incoming or (not incoming[0].isdigit() and incoming[0] != "-"):
                    incoming = incoming.replace("\n", "")
                    print("RAW: ", incoming)
                    continue

                self.buffer.append(incoming)
            except:
                print("Error reading serial input")
                break