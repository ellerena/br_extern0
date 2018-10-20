#!/usr/bin/python

import cgi
import platform
import getpass
import os
import datetime
import socket

print "Content-type:text/html\r\n"
t = datetime.datetime.now().strftime('%m/%d/%Y %H:%M:%S\n\r')

print """
<html>
<head>
<title>Llerena Family Server</title>
<style>img { border: 0px }</style> <!-- for IE -->
<meta name="viewport" content="width=device-width, initial-scale=1.0"/> <!-- smartphones -->
</head>
<body>
<div align="center">
<h1><img src="http://10.0.0.11/images/cherrypy.png"><br>
Llerena Family</h1>
</div>
<p>
"""
for i in  range(6):
  print '<a href="?power=' + str(i*20) + '">Led ' + str(i*20) + "%</a><br>"

print """
</p><p>
<a href="?switch=1"><img src="http://10.0.0.11/images/on.png"></a>
<a href="?switch=0"><img src="http://10.0.0.11/images/off.png"></a>
</p><p>
"""

print ''.join(platform.uname()), "<br>"
print getpass.getuser(), "<br>"
print os.getcwd(), "<br"
print t, "</p>"

form = cgi.FieldStorage()
message = form.getvalue("message", "(no message)")

print """
<p>Last msg: %s<br>
<form method="post" action="lili.py">
  message: <input type="text" name="message"/></p>
</form>
""" % cgi.escape(message)

s = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
s.sendto(t, ("localhost", 5005))

print "<p>(o)^(o)</p>"
print "</body>"
print "</html>"

