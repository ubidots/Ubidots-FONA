# Ubidots FONA Library 

**This library requires Arduino v1.0.6 or higher**

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


Written by Mateo Velez - Metavix for Adafruit Industries. Based on Adafruit_FONA library  

To download. click the DOWNLOADS button in the top right corner, rename the uncompressed folder Ubidots_fona Check that the Ubidots_fona folder contains Ubidots_fona.cpp and Ubidots_fona.h

Place the Ubidots_fona library folder your *arduinosketchfolder*/libraries/ folder. 
You may need to create the libraries subfolder if its your first library. Restart the IDE.

## Install the Library

1. Download the library as a ZIP file
2. Open the Arduino IDE and go to "Sketch" --> "Import Library" --> "Add Library".
3. Select the downloaded ZIP file
4. Restart the Arduino IDE, you should then see Ubidots_fona examples under "File" --> "Examples" --> "Ubidots_fona"


