# gpio
GPIO interaction examples
It works with Raspberry Pi 3 on Linux

## Build 
```bash
make
```
## Run
Note: Most of the time work with GPIO requires root user privileges
```bash
sudo ./led_switch
```
Short press (~0,5 sec) turns the led on/off
Longer press (~1 sec) terminates the app

## Connection example
Button connect to GPIO17, LED to GPIO18 according to the pinout scheme https://pinout.xyz/pinout/

![Connection example](https://www.dropbox.com/s/jpj3t10l97xomui/Photo%2013-06-2018%2C%2008%2057%2047.jpg)
