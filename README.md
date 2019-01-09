# IoT DevFest 2018



## What is Bluetooth Low Energy?

![screenshot](BLE_diagram.png)



## Web Bluetooth


![screenshot](chirpers.png)

[https://chirpers.com](https://chirpers.com/browser)


## ESP32 + LoRa

![screenshot](lora.png)

This firmware for today:  https://github.com/monteslu/iotdevfest18/blob/master/iotdevfest_lora/iotdevfest_lora.ino


## Badge Assembly

![screenshot](esp32.jpg)

* remove paper from acrylic
* attach battery holder
* addatch esp32 board & connect power
* solder neopixel strip
* attach neopixels
* insert battery



## Sending Bytes.


Our main service Id for the badge: `6e400001-b5a3-f393-e0a9-e50e24dcca9e`

### characteristics

* lora relay Characteristic `6e400002-b5a3-f393-e0a9-e50e24dcca9e`

* configure screen `bada5566-e91d-1337-a49b-8675309fb070`
  * `[1, 16, 0, 0]`
  * first byte = 1 to clear screen, 0 to leave as is
  * second byte = font size 10, 16, or 24
  * third byte = draw position X
  * fourth byte = draw position Y

* write characters to screen `bada5566-e91b-1337-a49b-8675309fb070`

* write characters , but clear first `bada5566-e91c-1337-a49b-8675309fb070`

* Neopixels on pin 12 `bada5566-e91a-1337-a49b-8675309fb070`
  * [255,0,0, 127,127,0, 0,0,50]
  * 9 bytes.  for RGB values for each of the 3 neopixels




## last resort...

If you're having trouble getting things going, you can import this flow into chirpers:

```javascript
[{"id":"THlS0XtcvO0","type":"chirpers in","z":"mDRMOWxAyVw","name":"lora broadcasts","directToMe":false,"device":"f4a2b572-b33f-495d-a8f1-7271815d40a4","x":210,"y":466,"wires":[["AC6hzIu9jGQ"]]},{"id":"AC6hzIu9jGQ","type":"debug","z":"mDRMOWxAyVw","name":"","active":true,"console":"false","complete":"false","x":444,"y":559,"wires":[]},{"id":"a6rGzwzPHRk","type":"inject","z":"mDRMOWxAyVw","name":"pixel colors","topic":"","payload":"[255,0,0, 127,127,0, 0,0,50]","payloadType":"json","repeat":"","crontab":"","once":false,"allowDebugInput":false,"x":201,"y":69,"wires":[["snIWft_QO10"]]},{"id":"snIWft_QO10","type":"bluetooth out","z":"mDRMOWxAyVw","name":"pixel control","deviceName":"","characteristicId":"bada5566-e91a-1337-a49b-8675309fb070","bleServiceId":"6e400001-b5a3-f393-e0a9-e50e24dcca9e","x":460,"y":160,"wires":[]},{"id":"E2NNdB_APLE","type":"bluetooth out","z":"mDRMOWxAyVw","name":"lora out","deviceName":"","characteristicId":"6e400002-b5a3-f393-e0a9-e50e24dcca9e","bleServiceId":"6e400001-b5a3-f393-e0a9-e50e24dcca9e","x":472,"y":274,"wires":[]},{"id":"weqrjybYQ4g","type":"inject","z":"mDRMOWxAyVw","name":"","topic":"","payload":"hello world","payloadType":"str","repeat":"","crontab":"","once":false,"allowDebugInput":false,"x":214,"y":243,"wires":[["E2NNdB_APLE"]]},{"id":"hTYhRgiSRac","type":"bluetooth out","z":"mDRMOWxAyVw","name":"write to screen","deviceName":"","characteristicId":"bada5566-e91c-1337-a49b-8675309fb070","bleServiceId":"6e400001-b5a3-f393-e0a9-e50e24dcca9e","x":487,"y":394,"wires":[]},{"id":"g-W8-dXYkmo","type":"inject","z":"mDRMOWxAyVw","name":"","topic":"","payload":"I love this badge!","payloadType":"str","repeat":"","crontab":"","once":false,"allowDebugInput":false,"x":193,"y":373,"wires":[["hTYhRgiSRac"]]}]
```

#### Arduino libraries

- [LoRa](https://github.com/sandeepmistry/arduino-LoRa)
- [OLED SSD1306](https://github.com/ThingPulse/esp8266-oled-ssd1306)
