import threading
import sys, select, termios

class RealtimeInput:

    def __init__(self):
        # Setup the input handling
        self.oldStdinMode = termios.tcgetattr(sys.stdin)
        _ = termios.tcgetattr(sys.stdin)
        # Disable ECHO and line-mode
        _[3] = _[3] & ~(termios.ECHO | termios.ICANON)
        # Don't block on stdin.read()
        _[6][termios.VMIN] = 0
        _[6][termios.VTIME] = 0
        termios.tcsetattr(sys.stdin, termios.TCSAFLUSH, _)

        termios.tcflush(sys.stdin, termios.TCIOFLUSH)

        self.input_buffer = ""
        self.buffer = []
        self.thread = threading.Thread(target=self._read_input_callback, daemon=True)
        self.thread.start()

    def _read_input_callback(self):
        while self.thread.is_alive():
            try:
                # line = input().lower()
                # self.buffer.append(line)
                peek = sys.stdin.read(1)
                print("Received (peek): " + peek)
                if len(peek) > 0 and peek not in ["\0", "\3", "\x1b", "\r", "\n", "\b", "\x1a", "\4"]:
                    self.input_buffer += peek
                    sys.stdout.write(peek)
                    sys.stdout.flush()
                else:
                    if peek == "\r" or peek == "\n":
                        print(f"Received command: {self.input_buffer}")
                        self.buffer.append(self.input_buffer)
                        self.input_buffer = ""
                    elif peek == "\x1b":
                        print("Exiting...")
                        sys.exit(0)

            except:
                print("Error reading input")
                break
            finally:
                termios.tcsetattr(sys.stdin, termios.TCSAFLUSH, self.oldStdinMode)