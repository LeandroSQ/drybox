#!/bin/sh
set -e

# Settings
SERIAL_PORT=/dev/tty.usbmodem11201
MAIN_FILE=firmware/firmware.ino
BOARD=arduino:avr:uno

# Compile and upload
arduino-cli compile --fqbn $BOARD $MAIN_FILE --upload --port $SERIAL_PORT