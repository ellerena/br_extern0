#!/usr/bin/env lua

--[[
This is a simple demonstration of CGI (Common Gateway
Interface) programming in Lua.
]]

print ("Content-type: Text/html\n")

local info = os.getenv("QUERY_STRING")
local params = {}
local echo = {}

for name, value in string.gmatch(info .. '&', '(.-)%=(.-)%&') do
        value = string.gsub(value , '%+', ' ')
        value = string.gsub(value , '%%(%x%x)', function(dpc)
                return string.char(tonumber(dpc,16))
                end )
        params[name] = value

        value = string.gsub(value, "%&", "&amp;")
        value = string.gsub(value, "%<", "&lt;")
        value = string.gsub(value, '%"', "&quot;")
        echo[name] = value

        end

pagetop = [[<html><head><title>A Web page in Lua</title></head>
<body text=blue><h1>Calculate VAT at 15.0%</h1>
A Lua script with form fill ... <br><br>
<form>Net amount: <input name=net> and <input type=submit value="go!"></form>
<br><br>Result:<br><br>
]]
print (pagetop)

if params["net"] ~= ""  and params["net"] ~= nil then
        inval = tonumber(params["net"])
        if inval == nil then
                print ("Not a number - can't calculate")
        else
                print (inval * 1.15)
        end
        print ("<br><i>input value was</i><br>")
        print (echo["net"])
else
        print "<i>will appear here</i>"
end

pagebase = [[<br><br>
Sample Page in Lua by
<a href=http://www.wellho.net>Well House Consultants</a> &copy; 2008
</body></html>
]]

print (pagebase)
