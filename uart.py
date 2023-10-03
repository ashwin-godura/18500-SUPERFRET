'''
UART communication on Raspberry Pi using Pyhton
http://www.electronicwings.com
'''
import serial
from time import sleep

ser = serial.Serial ("/dev/ttyS0", 9600)    #Open port with baud rate
while True:
    sleep(1.03)
    print("sending: hi tushaar!")
    ser.write(("hi tushaar!").encode('utf-8'))    #transmit data serially 