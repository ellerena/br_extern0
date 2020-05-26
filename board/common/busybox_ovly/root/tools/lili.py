#!/usr/bin/python

import cgi
import platform
import getpass
import os
import datetime
import socket
import sys

print ('Argument List:', str(sys.argv))
tarAddr = "localhost" if len(sys.argv) < 2 else str(sys.argv[1])
tarPort = 5005 if len(sys.argv) < 3 else int(sys.argv[2])

print ("Content-type:text/html\r\n")
print ('<html><head><title>Hello from py/cgi</title></head>\
<body><p>')
for i in range(5):
  print ("Ti Amo Lili!<br>")
print ("</p><p>")
print (''.join(platform.uname()), "<br>")
print (getpass.getuser(), "<br>")
#print (os.environ['HOME'], "<br")
print (datetime.datetime.now().strftime('%c\n\r'), "</p>")

form = cgi.FieldStorage()
message = form.getvalue("message", "(no message)")

print ('"\
<p>Last msg: %s<br>\
<form method="post" action="lili.py">\
  message: <input type="text" name="message"/></p>\
</form>\
"' % html.escape(message))

s = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
s.sendto(message, (tarAddr, tarPort))

print ("<p>Process completed</p>")
print ("</body>")
print ("</html>")

os.environ
