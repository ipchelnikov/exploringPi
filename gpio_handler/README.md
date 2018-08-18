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

![Connection example](https://downloader.disk.yandex.ru/preview/ae60a0c79a0cb5e866792e638979875fac0cb59598a6ca9703f66c23faabf91b/5b23968f/mD2IKnThOcx0abHbGzJeDzjP7HQ2q_LwQs6wOkQlgoJbsLx2Z1IjjPpXDzEfBPxGIA3XZw691RuDRyvJcYlBrw%3D%3D?uid=0&filename=IMG_1306.jpg&disposition=inline&hash=&limit=0&content_type=image%2Fjpeg&tknv=v2&size=1920x966)
