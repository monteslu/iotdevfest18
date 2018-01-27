# IoT DevFest 2018



## What is Bluetooth Low Energy?

![screenshot](BLE_diagram.png)



## Web Bluetooth

[https://chirpers.com](https://chirpers.com)


## ESP32 + LoRa

![screenshot](lora.png)

This firmware for today:  https://github.com/monteslu/iotdevfest18/blob/master/iotdevfest_lora/iotdevfest_lora.ino


## Badge Assembly

![screenshot](esp32.jpg)

* remove paper from acrylic
* attach batter holder
* addatch esp32 board & connect power
* solder neopixel strip
* attach neopixels




## Sending Bytes.

![screenshot](chirpers.png)

* service Id: `6e400001-b5a3-f393-e0a9-e50e24dcca9e`

* lora relay Characteristic `6e400002-b5a3-f393-e0a9-e50e24dcca9e`

* configure screen `bada5566-e91d-1337-a49b-8675309fb070`
  `[1, 16, 0, 0]`
  first byte = 1 to clear screen, 0 to leave as is
  second byte = font size 10, 16, or 24
  third byte = draw position X
  fourth byte = draw position Y

* write characters to screen `bada5566-e91b-1337-a49b-8675309fb070`

* write characters , but clear first `bada5566-e91c-1337-a49b-8675309fb070`

* Neopixels on pin 12 `bada5566-e91a-1337-a49b-8675309fb070`
  [255,0,0, 127,127,0, 0,0,50]
  9 bytes.  for RGB values for each of the 3 neopixels




## last resort...

If you're having trouble getting things going, you can import this flow into chirpers:

```javascript

```
