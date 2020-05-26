#!/bin/sh

echo "Content-type: text/html"
echo ""
echo "<html><body>"
echo "<h1>Esta es la prueba CGI</h1>"
echo "<p>"
echo "Date: `date`<br>"
echo "OS: `uname -a`<br>"
echo "Username: `whoami`<br>"
echo "From: `pwd`/`basename $0`<br>"
echo "</body></html>"
