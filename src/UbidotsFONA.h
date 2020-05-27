/*
Copyright (c) 2013-2016 Ubidots.

Permission is hereby granted, free of charge, to any person obtaining
a copy of this software and associated documentation files (the
"Software"), to deal in the Software without restriction, including
without limitation the rights to use, copy, modify, merge, publish,
distribute, sublicense, and/or sell copies of the Software, and to
permit persons to whom the Software is furnished to do so, subject to
the following conditions:

The above copyright notice and this permission notice shall be
included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE
LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION
OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

Original Maker: Mateo Velez - Metavix for Ubidots Inc
Modified and Maintened by: María Carlina Hernández ---- Developer at Ubidots Inc
                           https://github.com/mariacarlinahernandez
*/

#ifndef __UbidotsFONA_H_
#define __UbidotsFONA_H_

#include <SoftwareSerial.h>
#include <stdlib.h>
#include "Arduino.h"

#define DEFAULT_BUFFER_SIZE      64
#define FONA_RX 2
#define FONA_TX 3
#define FONA_RST 4
#define SERVER "industrial.api.ubidots.com"
#define PORT "9012"
#define USER_AGENT "FONA"
#define VERSION "3.0"
#define MAX_VALUES 5

typedef struct Value {
    char  *varName;
    char  *ctext;
    float varValue;
    unsigned long timestamp_val;
} Value;

class Ubidots {

 private:
    bool _debug = false;
    bool sendMessageAndwaitForOK(char* message, uint16_t timeout = 4000);
    bool waitForMessage(const char *msg, uint32_t ts_max);
    bool isTimedOut(uint32_t ts) { return (long)(millis() - ts) >= 0; }
    bool begin();
    char* readData(uint16_t timeout);
    char* _token;
    char* _dsName;
    char* _dsTag;
    char* _device_label;
    char* _device_name;
    char* _server;
    char buffer[DEFAULT_BUFFER_SIZE];
    int readLine(uint32_t ts_max, bool multiline = false);
    int dataLen(char* variable);
    SoftwareSerial fonaSS = SoftwareSerial(FONA_TX, FONA_RX);
    uint8_t _currentValue;
    Value * val;
    // Variables to retro-compatibility
    char* _apn;
    char* _user;
    char* _pwd;

 public:

    Ubidots(char* token, char* server = SERVER);
    bool checkFona();
    bool setApn(char* apn, char* user = "", char* pwd = "");
    bool sendAll();
    void add(char *variable_label, double value);
    void add(char *variable_label, double value, char *ctext);
    void add(char *variable_label, double value, unsigned long timestamp_val);
    void add(char *variable_label, double value, char *ctext, unsigned long timestamp);
    void setDebug(bool debug);
    void setDeviceName(char *device_name);
    void setDeviceLabel(char *device_label);
    float getValue(char* device_label, char* variable_label);
    // Deprecated functions
    void gprsOnFona();
};

#endif  // __Ubidots_FONA_H_
