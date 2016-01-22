# Ubidots FONA Library 

**This library requires Arduino v1.6.7 or higher**

This is a library for the Adafruit FONA Cellular GSM Breakouts etc

Designed specifically to work with the Adafruit FONA Breakout
  * https://www.adafruit.com/products/1946
  * https://www.adafruit.com/products/1963
  * http://www.adafruit.com/products/2468
  * http://www.adafruit.com/products/2542

These modules use TTL Serial to communicate, 2 pins are required to interface

## Components

* Arduino UNO
* FONA
* Adafruit_FONA library
* Ubidots_fona library

## Pin Connection

* Vio –> 5V (or 3V, with a 3V logic Arduino)
* GND –> GND
* FONA_RX –> Pin 2
* FONA_TX –> Pin 3
* FONA_RST –> Pin 4
* FONA_KEY –> Pin 7
* FONA_PS –> Pin 8


Written by Mateo Velez - Metavix for Ubidots Inc. Based on Adafruit_FONA library  

To download. click the DOWNLOADS button in the top right corner, rename the uncompressed folder Ubidots_fona Check that the Ubidots_fona folder contains Ubidots_fona.cpp and Ubidots_fona.h

Place the Ubidots_fona library folder your *arduinosketchfolder*/libraries/ folder. 
You may need to create the libraries subfolder if its your first library. Restart the IDE.

## Steps to do a simple text with fona and Ubidots

1. Download the Adafruit_FONA library [here](https://github.com/adafruit/Adafruit_FONA_Library/archive/1.3.0.zip).
2. Download the Ubidots_fona library [here](https://github.com/ubidots/ubidots-fona/archive/master.zip)
3. Go to Arduino IDE, click on Sketch -> Include Library -> Add .zip Library
4. Select .zip file of Adafruit_FONA and Ubidots_fona, and just add it
5. Now go to File -> Examples -> Ubidots FONA library and select get_value or save_value examples
6. Change your token, your id and run it!  

If you dont know how to do it press [here](https://www.arduino.cc/en/Guide/Libraries) and you can see the steps in Arduino page.
