.. _devices/fona:

.. This section will be included in devices.rst.
   Please: don't remove the next line.
   start_read_more_include

Adafruit FONA MiniGSM
=====================

In this tutorial we'll explain how to send a value to Ubidots with the FONA devices.

.. end_read_more_include

Introduction
^^^^^^^^^^^^
Adafruit FONA MiniGSM, an adorable all-in-one cellular phone module that lets you add voice, text, SMS and data to your project in an adorable little package.

Components
^^^^^^^^^^
*	`FONA <http://www.adafruit.com/product/1963>`_:

.. figure:: /_static/devices/fona.jpg
    :name: fona
    :align: center
    :alt: fona

* A micro-controller board to manage the FONA through an UART interface. We’ll use an Arduino in this case. You can also find a `basic ArduinoFONA tutorial in Adafruit's website <http://www.adafruit.com/product/1963>`_. The pin connection for an Arduino would look like this:

  * Vio ----> 5V (or 3V, with a 3V logic Arduino)
  * GND ----> GND
  * Key ----> digital 6
  * RX  ----> digital 1
  * TX  ----> digital 0
  * RST ----> digital 4
   
 
Preparing your Ubidots Account
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
Create a Data source called "FONA" and then a variable called "My Variable":

1. As a `logged in user <http://app.ubidots.com/accounts/signin/>`_
   navigate to the "Sources" tab.

.. figure:: /_static/devices/sources.png
    :name: Sources Tab - Ubidots
    :scale: 60%
    :align: center
    :alt: Sources Tab - Ubidots

2. Create a data source called "My Variable" by clicking
   on the orange button located in the upper right corner of the screen:

.. figure:: /_static/devices/new-source.png
    :name: New Data Source
    :scale: 60%
    :align: center
    :alt: New Data Source

3. Click on the created Data Source and then on "Add New Variable":

.. figure:: /_static/devices/fona_newvar.png
    :name: New Variable
    :scale: 60%
    :align: center
    :alt: New Variable

4. Take note of the variable's ID to which you want to send data. We'll need it later to include in our code:

.. figure:: /_static/devices/fona-id.png
    :name: Variable ID
    :scale: 60%
    :align: center
    :alt: Variable ID

5. Create a token under "My Profile" tab. We'll need it later for our code:

.. figure:: /_static/devices/electricimp_token.png
    :name: Token creation
    :scale: 60%
    :align: center
    :alt: Token creation

Coding
^^^^^^

Note the function used to send the data to Ubidots:

.. code-block:: cpp

   Void Send2ubidots();



Here's the code for your Arduino:

.. code-block:: cpp

      /* Chip McClelland - Cellular Data Logger
           BSD license, Please keep my name and the required Adafruit text in any redistribution

        This sketch will connect to Ubidots and periodically upload the current temperature and humidity as well as the number
        of times a PIR sensor has detected a person. The counts are then reset and you can access the data on your Ubidots account.


        IDE: Arduino 1.0 or later
        I had to add the following line to the Adafruit_FONA.cpp file: typedef 

        char PROGMEM prog_char;

        Otherwise this code would not compile

        Hardware - Adafruit Fona GSM Board
        Temperature Sensor - Dummy Code for now
        Person Counter - Dummy Code for now

        I made use of the Adafruit Fona library and parts of the example code

        /*************************************************** 
        This is an example for our Adafruit FONA Cellular Module

        Designed specifically to work with the Adafruit FONA 
        ----> http://www.adafruit.com/products/1946
        ----> http://www.adafruit.com/products/1963

        Adafruit invests time and resources providing this open source code, 
        please support Adafruit and open-source hardware by purchasing 
        products from Adafruit!

        Written by Limor Fried/Ladyada for Adafruit Industries.  
        BSD license, all text above must be included in any redistribution
          
        ****************************************************/
        
        /*************************************************** 
        This code has been modified Sep 15 2014
        by Mateo Vélez - Metavix - for Ubidots Inc.

        This code is in the public domain.
        ****************************************************/        

      

      #include <HardwareSerial.h>
      #include "Adafruit_FONA.h"
       
      #define FONA_RX 1
      #define FONA_TX 0
      #define FONA_RST 4
      #define FONA_KEY 6
      #define FONA_PS 7
       
      #define HWSERIAL Serial1


      char replybuffer[255];

      HardwareSerial fonaSS = Serial1;
      Adafruit_FONA fona = Adafruit_FONA(&fonaSS, FONA_RST);
       
      uint8_t readline(char *buff, uint8_t maxbuff, uint16_t timeout = 0);


      int Interval = 10000;             // Time between measurements in seconds
      int KeyTime = 2000;               // Time needed to turn on the Fona
      unsigned long Reporting = 30000;  // Time between uploads to Ubidots
      unsigned long TimeOut = 30000;    // How long we will give an AT command to comeplete
      unsigned long LastReading = 0;    // When did we last read the sensors - they are slow so will read between sends
      unsigned long LastReporting = 0;  // When did we last send data to Ubidots
      uint8_t n=0; 
      int f = 0;
      int PersonCount = 0;

      void setup() {
        delay(3000);
        HWSERIAL.begin(4800);
        Serial.begin(9600);

        Serial.println("Started setup");

        pinMode(FONA_KEY,OUTPUT);                                    // This is the pin used to turn on and off the Fona
        TurnOnFona();
        Serial.println(F("FONA basic test"));
        Serial.println(F("Initializing....(May take 3 seconds)"));   // See if the FONA is responding
        if (! fona.begin(4800)) {                                    // make it slow so its easy to read!
          Serial.println(F("Couldn't find FONA"));
          while (1);
        }
        GetConnected();
        Serial.println(F("FONA is OK"));
        TurnOffFona();  
       
      }


      void loop() {
        Serial.println("in Loop");
        delay(2000);
        int value = analogRead(A0); 
        if (LastReporting + Reporting <= millis()) {   // This checks to see if it is time to send to Ubidots
          TurnOnFona();                                // Turn on the module
          GetConnected();                              // Connect to network and start GPRS
          Send2Ubidots(String(value));                 // Send data to Ubidots
          GetDisconnected();                           // Disconnect from GPRS
          TurnOffFona();                               // Turn off the modeule
          LastReporting = millis();
        }
         
        if (LastReading + Interval <= millis()) {     // This checks to see if it is time to take a sensor reading
          f ++;                 //Do some math to convert the Celsius to Fahrenheit
          PersonCount ++;
          Serial.print(F("The current Temperature is: "));
          Serial.print(f);                             //Send the temperature in degrees F to the serial monitor
          Serial.print(F("F "));    
          LastReading = millis();                     // Record the time of the last sensor readings
        }
        
        
      }

      // This function is to send the sensor data to Ubidots - each step is commented in the serial terminal
      void Send2ubidots(String value)
      {

        int num;
        String le;
        String var;
        var = "{\"value\":"+ value + "}";           //value is the sensor value
        num = var.length();
        le = String(num);                           //this is to calcule the length of var

        Serial.print(F("Start the connection to Ubidots: "));
        if (SendATCommand("AT+CIPSTART=\"tcp\",\"things.ubidots.com",\"80\"",'C','T')) {
          Serial.println("Connected");
        }
        Serial.print(F("Begin to send data to the remote server: "));
        if (SendATCommand("AT+CIPSEND",'\n','>')) {
          Serial.println("Sending");
        }
       fona.println("POST /api/v1.6/variables/xxxxxxxxxxxxxxxx/values HTTP/1.1");           // Replace "xxx..." with your variable ID
       fona.println("Content-Type: application/json");                                      
       fona.println("Content-Length: "+le");                                                        
       fona.println("X-Auth-Token: xxxxxxxxxxxxxxxxxx");                                    // here you should replace "xxx..." with your Ubidots Token               
       fona.println("Host: things.ubidots.com");
       fona.println();
       fona.println(var);
       fona.println();
       fona.println((char)26);                                       //This ends the JSON SEND with a carriage return
       Serial.print(F("Send JSON Package: "));
       if (SendATCommand("",'2','0')) {                              // The 200 code from Ubidots means it was successfully uploaded
          Serial.println("Sent");
           PersonCount = 0;
        }
        else {
          Serial.println("Send Timed out, will retry at next interval");
        }
       // delay(2000);
       Serial.print(F("Close connection to Ubidots: "));              // Close the connection
       if (SendATCommand("AT+CIPCLOSE",'G','M')) {
          Serial.println("Closed");
        }
      }

      boolean SendATCommand(char Command[], char Value1, char Value2) {
       unsigned char buffer[64];                                  // buffer array for data recieve over serial port
       int count = 0;
       int complete = 0;
       unsigned long commandClock = millis();                      // Start the timeout clock
       fona.println(Command);
       while(!complete && commandClock <= millis()+TimeOut)         // Need to give the modem time to complete command 
       {
         while(!fona.available() && commandClock <= millis()+TimeOut);
         while(fona.available()) {                                 // reading data into char array 
           buffer[count++]=fona.read();                            // writing data into array
           if(count == 64) break;
         }
         Serial.write(buffer,count);                           // Uncomment if needed to debug
         for (int i=0; i <= count; i++) {
           if(buffer[i]==Value1 && buffer[i+1]==Value2) complete = 1; 
         }
       }
       if (complete ==1) return 1;                              // Returns "True"  - "False" sticks in the loop for now
       else return 0;
      }

      void TurnOnFona()
      {
        Serial.print("Turning on Fona: ");
        while(digitalRead(FONA_PS)) 
        {
          digitalWrite(FONA_KEY,LOW);
          unsigned long KeyPress = millis();
          while(KeyPress + KeyTime >= millis()) {}
          digitalWrite(FONA_KEY,HIGH);
        }
        fona.begin(4800);
        Serial.println("success!");
      }

      void GetConnected() 
      {
        do 
        {
          n = fona.getNetworkStatus();  // Read the Network / Cellular Status
          Serial.print(F("Network status ")); 
          Serial.print(n);
          Serial.print(F(": "));
            if (n == 0) Serial.println(F("Not registered"));
            if (n == 1) Serial.println(F("Registered (home)"));
            if (n == 2) Serial.println(F("Not registered (searching)"));
            if (n == 3) Serial.println(F("Denied"));
            if (n == 4) Serial.println(F("Unknown"));
            if (n == 5) Serial.println(F("Registered roaming"));
        } while (n != 1);
      }

      void GetDisconnected()
      {
        fona.enableGPRS(false);
        Serial.println(F("GPRS Serivces Started"));
      }

      void TurnOffFona()
      {
        Serial.print("Turning off Fona: ");
        while(digitalRead(FONA_PS))
        {
          digitalWrite(FONA_KEY,LOW);
          unsigned long KeyPress = millis();
          while(KeyPress + KeyTime >= millis()) {}
          digitalWrite(FONA_KEY,HIGH);
        }
        Serial.println("success!");
      }

Wrapping it up
^^^^^^^^^^^^^^
In this guide we learned how to read an analog input from the Arduino UNO and send this value through FONA to Ubidots. After getting familiar with it, you can modify your hardware setup to send readings from any other type of sensors attached to it.

More projects...
^^^^^^^^^^^^^^^^^^

Check out other cool projects using Ubidots:
 
* `Arduino GPRS Sim900 <http://ubidots.com/docs/devices/gprs.html>`_
* `Arduino WizNet WizFi 250 <http://ubidots.com/docs/devices/wizfi.html>`_
* `Dragino IoT Gateway <http://ubidots.com/docs/devices/Dragino.html>`_
    

