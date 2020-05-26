#!/bin/sh

echo 60 > /sys/class/gpio/export
echo out > /sys/class/gpio/gpio60/direction
echo 1  > /sys/class/gpio/gpio60/value
sleep 5
echo 0 > /sys/class/gpio/gpio60/value
echo 60 > /sys/class/gpio/unexport

