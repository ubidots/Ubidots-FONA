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

Made by Mateo Velez - Metavix for Ubidots Inc

*/

#ifndef __Ubidots_FONA_H_
#define __Ubidots_FONA_H_
#define DEBUG_UBIDOTS

#include <SoftwareSerial.h>
#include <stdlib.h>
#include "Arduino.h"

#define DEFAULT_BUFFER_SIZE      64
#define FONA_RX 2
#define FONA_TX 3
#define FONA_RST 4
#define FONA_KEY 7
#define FONA_PS 8
#define SERVER "translate.ubidots.com"
#define PORT "9010"
#define USER_AGENT "FONA"
#define VERSION "1.1"
#define MAX_VALUES 5


typedef struct Value {
    char  *idName;
    char  *contextOne;
    float idValue;
} Value;


class Ubidots{
private:
    SoftwareSerial fonaSS = SoftwareSerial(FONA_TX, FONA_RX);
    bool sendMessageAndwaitForOK(char* message, uint16_t timeout=4000);
    bool waitForMessage(const char *msg, uint32_t ts_max);
    bool isTimedOut(uint32_t ts) { return (long)(millis() - ts) >= 0; }
    int readLine(uint32_t ts_max);
    char* _token;
    char* _dsName;
    char* _dsTag;
    char* _server;
    char buffer[DEFAULT_BUFFER_SIZE];
    uint8_t currentValue;
    Value * val;

public:
    Ubidots(char* token, char* server = SERVER);
    void begin();
    void setDataSourceName(char *dsName);
    void setDataSourceTag(char *dsTag);
    void add(char *variable_id, float value, char *ctext1 = NULL);
    bool sendAll();
    float getValueWithDatasource(char* dsTag, char* idName);
    bool setApn(char* apn, char* user = "", char* pwd = "");
    void turnOffFona();
    void turnOnFona();
    // Deprecated functions
    void gprsNetwork(const __FlashStringHelper *apn, const __FlashStringHelper *username, const __FlashStringHelper *password);
    void saveValue(char* myid, float value);
    float getValue(char* myid);
    void flushSerial();
-   void gprsOnFona();
};
#endif

