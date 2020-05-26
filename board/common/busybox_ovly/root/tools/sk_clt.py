#!/usr/bin/python

import socket
import datetime
import sys

print ('Argument List:', str(sys.argv))
tarAddr = "localhost" if len(sys.argv) < 2 else str(sys.argv[1])
tarPort = 5005 if len(sys.argv) < 3 else int(sys.argv[2])
t = datetime.datetime.now().strftime('clt: %c\n\r') if len(sys.argv) < 4 else sys.argv[3]
s = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
s.sendto(t.encode(), (tarAddr, tarPort))

