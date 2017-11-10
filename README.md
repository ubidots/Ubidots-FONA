# Ubidots FONA Library

**This library requires Arduino v1.6.0 or higher**
**This version is not compatible with 1.2 version of the library**

This is a library for the Adafruit FONA Cellular GSM Breakouts:

  * https://www.adafruit.com/products/1946
  * https://www.adafruit.com/products/1963
  * http://www.adafruit.com/products/2468
  * http://www.adafruit.com/products/2542

These modules use TTL Serial to communicate, so at least 2 pins are required to interface the FONA with your Arduino board.

## Required items

* Arduino UNO
* FONA
* An active SIM card **with a data plan**
* Ubidots_FONA library

## FONA - Arduino UNO connections

FONA | Arduino UNO
-----|------------
Vio | 5V (or 3V, if your Arduino has 3V logic)
GND | GND
FONA_RX | Pin 2
FONA_TX | Pin 3
FONA_RST | Pin 4
FONA_KEY | GND


## Run the examples!

1. Download the UbidotsFONA library [here](https://github.com/ubidots/Ubidots-FONA/archive/1.2.0.zip)
2. Go to the Arduino IDE, click on Sketch -> Include Library -> Add .ZIP Library
3. Select the UbidotsFONA .ZIP file and then "Accept" or "Choose"
4. Do the same with the [Adafruit_FONA](https://github.com/adafruit/Adafruit_FONA) library.
5. Close the Arduino IDE and open it again.Pin Connections FONA -> Arduino

**Before continuing, make sure your device can connect to the Internet! You can run Adafruit's example "FONAtest.ino" contained in [Adafruit's FONA](https://github.com/adafruit/Adafruit_FONA_Library/archive/1.3.0.zip) library to make sure your FONA is working properly and is able to connect to your mobile network.**
7. Now go to **File -> Examples -> UbidotsFONA** library and select one example.
6. Put your Ubidots token, device label and variable label where is indicated.
7. Put your mobile operator's APN settings (APN, USER, PASSWORD). You should be able to easily find your operator's APN settings on Google or in the operator's website.
8. Upload the code, open the Serial monitor to check the results. If no response is seen, try unplugging your Arduino and then plugging it again. Make sure the baud rate of the Serial monitor is set to the same one specified in your code.

Once you see some activity, go to your Ubidots account to check your data!

To manually install the libraries, copy the Ubidots_FONA folder contained in the ZIP file into your *arduinosketchfolder*/libraries/ folder. You may need to create the libraries subfolder if its your first library, then restart the IDE.

If you're not sure how to add a library to the Arduino IDE please click [here](https://www.arduino.cc/en/Guide/Libraries) to see the steps as explained by Arduino.

You can contact support@ubidots.com for any inquiries, please specify your Mobile operator and provide the dump of the Serial Monitor for debugging purposes.


# Documentation

## Methods

### Ubidots
`setDeviceName(char *device_name)`
> Assign a new device name. The device name by default is **"FONA"**

`void setDeviceLabel(char *device_label)`
> Assign a new device label. The device label by default is **"FONA"**

`add(char *variable_label, double value)`
> Add a variable with a value

`add(char *variable_label, double value, char *ctext)`
> Add a variable with a value and context [optional]

`add(char *variable_label, double value, unsigned long timestamp_val)`
> Add a variable with a value and timestamp [optional]

`add(char *variable_label, double value, char *ctext, unsigned long timestamp)`
> Add a variable with a value, context, and timestamp [optional]

`sendAll()`
> Send all data of all variables that you saved using the method **add(char *variable_label, double value)**

`getValue(char* device_label, char* variable_label)`
> Return the last value from a specific variable

## Included Examples

* [UbidotsGetValue](https://github.com/ubidots/Ubidots-FONA/blob/master/examples/UbidotsGetValue/UbidotsGetValue.ino)
* [UbidotsSendMultipleValues](https://github.com/ubidots/Ubidots-FONA/blob/master/examples/UbidotsSendMultipleValues/UbidotsSendMultipleValues.ino)
* [UbidotsSendValuesWithContext](https://github.com/ubidots/Ubidots-FONA/blob/master/examples/UbidotsSendValuesWithContext/UbidotsSendValuesWithContext.ino)
* [UbidotsSendValuesWithTimestamp](https://github.com/ubidots/Ubidots-FONA/blob/master/examples/UbidotsSendValuesWithTimestamp/UbidotsSendValuesWithTimestamp.ino)
