#include <Adafruit_NeoPixel.h>

#include <SPI.h>
#include <LoRa.h>
#include<Arduino.h>
// #include <Wire.h>  // Only needed for Arduino 1.6.5 and earlier
#include "SSD1306.h" // alias for `#include "SSD1306Wire.h"`

#define AUTO_PIXEL_PIN 13
#define CONTROL_PIXEL_PIN 12

Adafruit_NeoPixel autostrip = Adafruit_NeoPixel(3, AUTO_PIXEL_PIN, NEO_GRB + NEO_KHZ800);
Adafruit_NeoPixel controlstrip = Adafruit_NeoPixel(3, CONTROL_PIXEL_PIN, NEO_GRB + NEO_KHZ800);

#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEServer.h>
#include <BLE2902.h>

// See the following for generating UUIDs:
// https://www.uuidgenerator.net/

#define SERIAL_SERVICE_UUID "6e400001-b5a3-f393-e0a9-e50e24dcca9e"

#define SERIAL_CHAR_UUID          "6e400002-b5a3-f393-e0a9-e50e24dcca9e"
#define IOTDF_RGB_CHAR_UUID       "bada5566-e91a-1337-a49b-8675309fb070"
#define IOTDF_WRITE_CHAR_UUID     "bada5566-e91b-1337-a49b-8675309fb070"
#define IOTDF_WRITECLR_CHAR_UUID  "bada5566-e91c-1337-a49b-8675309fb070"
#define IOTDF_MODE_CHAR_UUID      "bada5566-e91d-1337-a49b-8675309fb070"


char macString[] = "abcd";

char spaces[] = "                                                                                                                                ";

// GPIO5  -- SX1278's SCK
// GPIO19 -- SX1278's MISO
// GPIO27 -- SX1278's MOSI
// GPIO18 -- SX1278's CS
// GPIO14 -- SX1278's RESET
// GPIO26 -- SX1278's IRQ(Interrupt Request)

#define SS      18
#define RST     14
#define DI0     26
#define BAND    433E6  //915E6 -- 这里的模式选择中，检查一下是否可在中国实用915这个频段

SSD1306  display(0x3c, 4, 15);

byte mac[6];
int counter = 0;
int fontSize = 10;
int drawX = 0;
int drawY = 0;

int tick = 0;

volatile char message[255];
volatile bool messageIdle = true;


BLECharacteristic *pSerialChar;

void writeOled(String message) {

  display.setTextAlignment(TEXT_ALIGN_LEFT);
  if (fontSize == 10) {
    display.setFont(ArialMT_Plain_10);
  }
  else if (fontSize == 16) {
    display.setFont(ArialMT_Plain_16);
  }
  else if (fontSize == 24) {
    display.setFont(ArialMT_Plain_24);
  }

  display.drawStringMaxWidth(drawX, drawY, 128, message);
  display.display();
}


class SerialCallbacks: public BLECharacteristicCallbacks {
    void onWrite(BLECharacteristic *pCharacteristic) {
      std::string value = pCharacteristic->getValue();
      int len = value.length();


      if (value.length() > 0 && messageIdle) {
        for (int i = 0; i < len; i++) {
          //          Serial.print(value[i]);
          message[i] = value[i];
                       //          LoRa.print(value[i]);
        }

        message[len] = '\0';

        messageIdle = false;
      }

    }
};

class RGBPixelCallbacks: public BLECharacteristicCallbacks {
    void onWrite(BLECharacteristic *pCharacteristic) {
      std::string value = pCharacteristic->getValue();
      const char *cstr = value.c_str();

      if (value.length() > 0) {
        Serial.println("*********");
        Serial.print("New value: ");
        Serial.println(cstr);
        Serial.println("*********");

      }

      if (value.length() > 2) {
        controlstrip.setPixelColor(0, controlstrip.Color( value[0], value[1], value[2] ));
      }

      if (value.length() > 5) {
        controlstrip.setPixelColor(1, controlstrip.Color( value[3], value[4], value[5] ));
      }

      if (value.length() > 8) {
        controlstrip.setPixelColor(2, controlstrip.Color( value[6], value[7], value[8] ));
      }

      controlstrip.show();


    }
};

class ScreenDrawCallbacks: public BLECharacteristicCallbacks {
    void onWrite(BLECharacteristic *pCharacteristic) {
      std::string value = pCharacteristic->getValue();
      const char *cstr = value.c_str();

      if (value.length() > 0) {
        Serial.println("*********");
        Serial.print("New Screen Draw value: ");
        Serial.println(cstr);
        Serial.println("*********");

        writeOled(cstr);

      }

    }
};


class ScreenDrawClearCallbacks: public BLECharacteristicCallbacks {
    void onWrite(BLECharacteristic *pCharacteristic) {
      std::string value = pCharacteristic->getValue();
      const char *cstr = value.c_str();

      if (value.length() > 0) {
        Serial.println("*********");
        Serial.print("New Screen Draw Clear value: ");
        Serial.println(cstr);
        Serial.println("*********");

        display.clear();
        display.display();
        delay(100);
        writeOled(cstr);

      }

    }
};

class ScreenModeCallbacks: public BLECharacteristicCallbacks {
    void onWrite(BLECharacteristic *pCharacteristic) {
      std::string value = pCharacteristic->getValue();

      if (value.length() > 0) {
        Serial.println("*********");
        Serial.print("length: ");
        Serial.println(value.length());
        Serial.println("");
        Serial.print("New ScreenMode value: ");
        for (int i = 0; i < value.length(); i++) {
          Serial.print(value[i]);
        }


        Serial.println();
        Serial.println("*********");

        if (value[0] > 0) {
          display.clear();
          display.display();
        }
      }

      if (value.length() > 1) {
        if ((value[1] == 10) || (value[1] == 16) || (value[1] == 24)) {
          fontSize = value[1];
        }

      }

      if (value.length() > 2) {
        drawX = value[2];
      }

      if (value.length() > 3) {
        drawY = value[3];
      }


    }
};



void setup() {
  pinMode(25, OUTPUT); //Send success, LED will bright 1 second

  message[0] = '\0';


  Serial.begin(115200);
  while (!Serial); //If just the the basic function, must connect to a computer

  uint8_t chipid[6];
  esp_efuse_read_mac(chipid);
  sprintf(macString, "%02x%02x", chipid[4], chipid[5]);
  Serial.printf("%02x:%02x:%02x:%02x:%02x:%02x\n", chipid[0], chipid[1], chipid[2], chipid[3], chipid[4], chipid[5]);


  BLEDevice::init(macString);
  BLEServer *pServer = BLEDevice::createServer();

  BLEService *serialService = pServer->createService(SERIAL_SERVICE_UUID);

  pSerialChar = serialService->createCharacteristic(
                  SERIAL_CHAR_UUID,
                  BLECharacteristic::PROPERTY_READ   |
                  BLECharacteristic::PROPERTY_WRITE  |
                  BLECharacteristic::PROPERTY_NOTIFY |
                  BLECharacteristic::PROPERTY_INDICATE
                );
  pSerialChar->setCallbacks(new SerialCallbacks());
 BLECharacteristic *iotdfWriteChar = serialService->createCharacteristic(
                                       IOTDF_WRITE_CHAR_UUID,
                                       BLECharacteristic::PROPERTY_READ |
                                       BLECharacteristic::PROPERTY_WRITE
                                     );
 iotdfWriteChar->setCallbacks(new ScreenDrawCallbacks());


 BLECharacteristic *iotdfWriteClrChar = serialService->createCharacteristic(
     IOTDF_WRITECLR_CHAR_UUID,
     BLECharacteristic::PROPERTY_READ |
     BLECharacteristic::PROPERTY_WRITE
                                        );
 iotdfWriteClrChar->setCallbacks(new ScreenDrawClearCallbacks());


 BLECharacteristic *iotdfModeChar = serialService->createCharacteristic(
                                      IOTDF_MODE_CHAR_UUID,
                                      BLECharacteristic::PROPERTY_READ |
                                      BLECharacteristic::PROPERTY_WRITE
                                    );
 iotdfModeChar->setCallbacks(new ScreenModeCallbacks());


 BLECharacteristic *iotdfRGBChar = serialService->createCharacteristic(
                                     IOTDF_RGB_CHAR_UUID,
                                     BLECharacteristic::PROPERTY_READ |
                                     BLECharacteristic::PROPERTY_WRITE
                                   );
 iotdfRGBChar->setCallbacks(new RGBPixelCallbacks());


  pSerialChar->addDescriptor(new BLE2902());

//  pSerialChar->setValue("Hello World");
  // Start the service
  pServer->getAdvertising()->addServiceUUID(SERIAL_SERVICE_UUID);
  serialService->start();

  // Start advertising
  pServer->getAdvertising()->start();


  SPI.begin(5, 19, 27, 18);
  LoRa.setPins(SS, RST, DI0);
  //  Serial.println("LoRa Sender");

  if (!LoRa.begin(BAND)) {
    Serial.println("Starting LoRa failed!");
    while (1);
  }
  Serial.println("LoRa Initial OK!");
  pinMode(16, OUTPUT);
  digitalWrite(16, LOW);    // set GPIO16 low to reset OLED
  delay(50);
  digitalWrite(16, HIGH); // while OLED is running, must set GPIO16 in high
  display.init();
  display.flipScreenVertically();

  writeOled("chirp chrip !");

  display.clear();
  display.setTextAlignment(TEXT_ALIGN_LEFT);
  display.setFont(ArialMT_Plain_16);

  display.drawStringMaxWidth(0, 0, 128, "me: " + String(macString));
  display.drawStringMaxWidth(0, 18, 128, "chirp chrip!");

  display.display();

  autostrip.begin();
  controlstrip.begin();

  LoRa.beginPacket();
  LoRa.print(macString);
  LoRa.print("|chirp!");
  LoRa.endPacket();


}

void drawRed(int slot) {
  //  Serial.print("slot ");
  //  Serial.println(slot);
  switch (slot) {
    case 0:
      autostrip.setPixelColor(0, autostrip.Color( 50, 0, 0));
      autostrip.setPixelColor(1, autostrip.Color( 10, 0, 0));
      autostrip.setPixelColor(2, autostrip.Color( 0, 0, 0));
      break;
    case 1:
      autostrip.setPixelColor(0, autostrip.Color( 5, 0, 0));
      autostrip.setPixelColor(1, autostrip.Color( 70, 0, 0));
      autostrip.setPixelColor(2, autostrip.Color( 5, 0, 0));
      break;
    default:
      autostrip.setPixelColor(0, autostrip.Color( 0, 0, 0));
      autostrip.setPixelColor(1, autostrip.Color( 10, 0, 0));
      autostrip.setPixelColor(2, autostrip.Color( 50, 0, 0));
      break;
  }

  autostrip.show();
}

void loop() {

  int spot = tick % 6;


  if (messageIdle == false) {
    LoRa.beginPacket();
    LoRa.print(macString);
    LoRa.print("|");

    Serial.println("*********");
    Serial.print("New serial value: ");


    int len = 0;

    for (int i = 0; message[i] != '\0'; i++) {
      Serial.print(message[i]);
      LoRa.print(message[i]);

      len++;
    }

    LoRa.endPacket();

    Serial.println();
    Serial.println("length ");
    Serial.println(len);
    Serial.println("*********");

    message[0] = '\0';
    messageIdle = true;
  }

  // try to parse packet
  int packetSize = LoRa.parsePacket();
  if (packetSize) {
    char blah[packetSize + 1];
    // received a packet
    Serial.print("Received packet '");
    int count = 0;
    // read packet
    while (LoRa.available()) {
      char c = LoRa.read();
      Serial.print(c);
      blah[count] = c;
      count++;
    }

    blah[count] = '\0';

    pSerialChar->setValue(blah);
    pSerialChar->notify();
    // print RSSI of packet
    Serial.print("' with RSSI ");
    Serial.println(LoRa.packetRssi());
  }

  tick++;

  //  Serial.println(spot);

  // TODO, someone clean this up please
  switch (spot) {
    case 0:
      drawRed(0);
      break;
    case 1:
      drawRed(1);
      break;
    case 2:
      drawRed(2);
      break;
    case 3:
      drawRed(2);
      break;
    case 4:
      drawRed(1);
      break;
    case 5:
      drawRed(0);
      break;
    default:
      // if nothing else matches, do the default
      // default is optional
      break;
  }




  delay(250);
}
