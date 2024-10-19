#!/bin/sh
#bin/arduino-cli/bin/arduino-cli help compile

if [ $# -lt 1 ]; then
  port="/dev/cu.usbmodem1101"
#  port="/dev/cu.usbmodem211401"
  #port="/dev/cu.usbserial-21140"
else
  port=$1
fi

bin/arduino-cli/bin/arduino-cli upload\
    --additional-urls "https://raw.githubusercontent.com/espressif/arduino-esp32/gh-pages/package_esp32_index.json"\
    -b esp32:esp32:esp32c3:CDCOnBoot=cdc\
    --log-level info\
    -p $port\
    c1_v1_1_firmware.ino
bin/arduino-cli/bin/arduino-cli monitor\
  -c baudrate=115200\
  -b esp32:esp32:esp32c3\
  -p $port
