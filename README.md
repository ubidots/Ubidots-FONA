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
* An active SIM card **with a data plan**
* Ubidots_FONA library

## Pin Connections FONA -> Arduino

* Vio –> 5V (or 3V, if your Arduino has 3V logic)
* GND –> GND
* FONA_RX –> Pin 2
* FONA_TX –> Pin 3
* FONA_RST –> Pin 4
* FONA_KEY –> Pin 7
* FONA_PS –> Pin 8

## Run the examples!

1. Download the Ubidots_FONA library [here](https://github.com/ubidots/Ubidots-FONA/archive/1.2.0.zip)
2. Go to the Arduino IDE, click on Sketch -> Include Library -> Add .ZIP Library
3. Select the Ubidots_FONA .ZIP file and then "Accept" or "Choose"
4. Do the same with the Adafruit_FONA library.
5. Close the Arduino IDE and open it again.
**Before continuing, make sure your device can connect to the Internet! You can run Adafruit's example "FONAtest.ino" contained in [Adafruit's FONA](https://github.com/adafruit/Adafruit_FONA_Library/archive/1.3.0.zip) library to make sure your FONA is working properly and is able to connect to your mobile network.**
7. Now go to File -> Examples -> Ubidots FONA library and select one example.
6. Put your Ubidots token and variable ID
7. Put your mobile operator's APN settings (APN, USER, PASSWORD). You should be able to easily find your operator's APN settings on Google or in the operator's website.
8. Upload the code, open the Serial monitor to check the results. If no response is seen, try unplugging your Arduino and then plugging it again. Make sure the baud rate of the Serial monitor is set to the same one specified in your code.

Once you see some activity, go to your Ubidots account to check your data! 

To manually install the libraries, copy the Ubidots_FONA folder contained in the ZIP file into your *arduinosketchfolder*/libraries/ folder. You may need to create the libraries subfolder if its your first library, then restart the IDE.

If you're not sure how to add a library to the Arduino IDE please click [here](https://www.arduino.cc/en/Guide/Libraries) to see the steps as explained by Arduino.

You can contact support@ubidots.com for any inquiries, please specify your Mobile operator and provide the dump of the Serial Monitor for debugging purposes.

## Included Examples

### Send a value with latitude and longitude

To send a value with latitude and longitude use the example **UbidotsSendValuesWithContext** or just copy the following code:

```c

#include <Ubidots_FONA.h>
#include <SoftwareSerial.h>


#define APN  ""  // The APN of your operator
#define USER ""  // if your apn doesnt have username just leave it ""
#define PASS ""  // if your apn doesnt have password just leave it ""
#define TOKEN "Your_Token_Here"  // Replace it with your Ubidots token

Ubidots client(TOKEN);

void setup() {
  Serial.begin(115200);
  delay(2000);
  client.setApn(APN, USER, PASS);
}
void loop() {
    float value1 = analogRead(A0);
    float value2 = analogRead(A1);
    char context[25];
    char context_2[45];
    sprintf(context, "lat=%s$lng=%s", char(1.1232), char(4323.123));
    // To send latitude and longitude to Ubidots and see it in a map widget
    // The format of the context is like this, you must send it like this example
    sprintf(context_2, "Temperature=Value_Of_Temperature$Position=Value_Of_Position$Color=Value_Of_Color");
    // You can send multiple context in one variable, to send it you must add a "$" symbol between every context
    client.add("Variable_Name_One", value1, context);  // Change for your variable name
    client.add("Variable_Name_Two", value2,  context_2);
    client.sendAll();
    delay(5000);
}
```


## Get value from a Ubidots variable

To get a value you could use the example inside the library or copy the next code:

```c
#include <Ubidots_FONA.h>
#include <SoftwareSerial.h>


#define APN  ""  // The APN of your operator
#define USER ""  // if your apn doesnt have username just leave it ""
#define PASS ""  // if your apn doesnt have password just leave it ""
#define TOKEN "Your_token_here"  // Replace it with your Ubidots token
#define ID "Your_id_here" // Replace it with your Ubidots' variable ID


Ubidots client(TOKEN);  
  
void setup() {
  Serial.begin(115200);
  delay(2000);
  client.setApn(APN, USER, PASS);
}

void loop() {
  float value = client.getValue(ID);  
  delay(1000); 
  Serial.println(value);
}
```