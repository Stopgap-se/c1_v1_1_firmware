#!/bin/sh
if [ $# -lt 1 ]; then
  port="/dev/cu.usbmodem1101"
  #port="/dev/cu.usbserial-21140"
else
  port=$1
fi
bin/arduino-cli/bin/arduino-cli monitor\
  -c baudrate=115200\
  -b esp32:esp32:esp32c3\
  -p $port
