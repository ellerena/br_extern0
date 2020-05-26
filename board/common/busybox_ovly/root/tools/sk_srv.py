#!/usr/bin/python

import socket
#import serial
#import Adafruit_BBIO.UART as UART

#UART.setup("UART4")
#ser = serial.Serial(port = "/dev/ttyO4", baudrate=115200)
#ser.close()
#ser.open()
#if ser.isOpen():
#  print ("Serial is ready...")
#  ser.write("BBGW UART Server Ready\n\r")
#else:
#  print ("Couldn't open serial port!")
#  quit()  

#ser.close()
s = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
s.bind(("0.0.0.0", 5005))

while 1:
  data, addr = s.recvfrom(50)
#  ser.open()
#  ser.write(data)
#  ser.close()
  print (data, addr)
  if data[0] == ord('.'):
    print("bye")
    break

