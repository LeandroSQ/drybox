#!/bin/sh
set -e

# Settings
SERIAL_PORT=/dev/tty.usbserial-1140
MAIN_FILE=firmware/firmware.ino
BOARD=arduino:avr:nano

# Compile and upload
arduino-cli compile --fqbn $BOARD $MAIN_FILE --upload --port $SERIAL_PORT