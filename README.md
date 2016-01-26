# Ubidots FONA Library 

**This library requires Arduino v1.6.0 or higher**

This is a library for the Adafruit FONA Cellular GSM Breakouts:

  * https://www.adafruit.com/products/1946
  * https://www.adafruit.com/products/1963
  * http://www.adafruit.com/products/2468
  * http://www.adafruit.com/products/2542

These modules use TTL Serial to communicate, so at least 2 pins are required to interface the FONA with your Arduino board.

## Required items

* Arduino UNO
* FONA
* [Adafruit_FONA library](https://github.com/adafruit/Adafruit_FONA_Library/archive/1.3.0.zip) 
* Ubidots_FONA library

## Pin Connections FONA -> Arduino

* Vio –> 5V (or 3V, if your Arduino has 3V logic)
* GND –> GND
* FONA_RX –> Pin 2
* FONA_TX –> Pin 3
* FONA_RST –> Pin 4
* FONA_KEY –> Pin 7
* FONA_PS –> Pin 8

## Steps to do a simple test with FONA and Ubidots

1. Download the Adafruit_FONA library [here](https://github.com/adafruit/Adafruit_FONA_Library/archive/1.3.0.zip).
2. Download the Ubidots_FONA library [here](https://github.com/ubidots/ubidots-fona/archive/1.0.0.zip)
3. Go to the Arduino IDE, click on Sketch -> Include Library -> Add .ZIP Library
4. Select the .ZIP file of Ubidots_FONA and then "Accept" or "Choose"
5. Do the same to add the Adafruit_FONA library.
6. Close the Arduino IDE and open it again.
7. Now go to File -> Examples -> Ubidots FONA library and select get_value or save_value examples
6. Change your token, your variable ID and run it!  

To manually install the libraries, copy the Ubidots_FONA folder contained in the ZIP file into your *arduinosketchfolder*/libraries/ folder. You may need to create the libraries subfolder if its your first library, then restart the IDE.

If you're not sure how to add a library to the Arduino IDE please click [here](https://www.arduino.cc/en/Guide/Libraries) to see the steps as explained by Arduino.

Written by Mateo Velez - Metavix for Ubidots Inc. Based on Adafruit_FONA library  
