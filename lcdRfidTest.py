#/bin/python
#lcdRfidTest.py resides on ~/pi@vending raspberrypi in vending machine
#JDP 08-01-2016
#
import time
import serial

port = 'COM4'
#port = '/dev/ttyUSB1'

# configure the serial connections (the parameters differs on the device you are connecting to)
ser = serial.Serial(
    port=port,
    baudrate=115200,
    parity=serial.PARITY_NONE,
    stopbits=serial.STOPBITS_ONE,
    bytesize=serial.EIGHTBITS
)

ser.isOpen()

try:
   while True:
      line = ser.readline().strip()
      if line.startswith('Card:'):
         print(line)
         ser.write(line)

finally:   
   ser.close()
