# try: make help
#

ifeq ($(OS),Windows_NT)
include Makefile-win32
else

ARDUINO_DIR = /usr/share/arduino
# list boards with: make show_boards
BOARD_TAG = mega
BOARD_SUB = atmega2560
ARDUINO_LIBS = EEPROM 
MONITOR_CMD = picocom -b 9600
#ARDUINO_PORT = /dev/serial/by-id/usb-Arduino_LLC_Arduino_Leonardo-if00
# ARDUINO_PORT = /dev/ttyACM0
# ARDUINO_PORT = /dev/ttyUSB0

OPTIMIZATION_LEVEL=0

CXXFLAGS += -DARDUINO_AVR_MEGA2560 -fexec-charset=windows-1251 -g3
include /usr/share/arduino/Arduino.mk

endif
