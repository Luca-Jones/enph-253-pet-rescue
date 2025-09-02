import serial
import time
from enum import Enum

class tof_reading_e(Enum):
    TOF_READING_NONE            = 0
    TOF_READING_PET             = 1
    TOF_READING_PILLAR          = 2
    TOF_READING_PET_OFFCENTER   = 3

def ser_setup() -> serial.Serial:
    ser = serial.Serial(port='/dev/ttyAMA0', baudrate=115200, timeout=1)
    time.sleep(1);
    return ser

def send_reading(ser: serial.Serial, reading: tof_reading_e):
    ser.write(reading)


# example
ser = ser_setup()
send_reading(ser, reading=tof_reading_e.TOF_READING_NONE)