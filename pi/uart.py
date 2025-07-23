import serial
import time

ser = serial.Serial(port='/dev/ttyAMA0', baudrate=115200, timeout=1)
time.sleep(2)

ser.write('hello world!\r\n'.encode('utf-8'));

ser.close()