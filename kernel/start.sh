#!/bin/sh

# On raspbian,  append the script to
# /etc/rc.local to launch it on boot 

echo "Starting disply..."

# install kernel driver
insmod /home/pi/Dev/exploringPi/kernel/indicator.ko

# run the userspace thread
# ampersand in the end forks the process
/home/pi/Dev/exploringPi/kernel/test &
