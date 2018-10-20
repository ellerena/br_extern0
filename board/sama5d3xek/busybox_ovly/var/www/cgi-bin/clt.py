#!/usr/bin/python

import socket
import datetime

t = datetime.datetime.now().strftime('%m/%d/%Y %H:%M:%S\n\r')
s = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
s.sendto(t, ("127.0.0.1", 5005))

