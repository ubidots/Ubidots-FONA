#ifndef __Ubidots_FONA_H_
#define __Ubidots_FONA_H_
#define DEBUG_UBIDOTS

#include <SoftwareSerial.h>
#include <Adafruit_FONA.h>
#include <stdlib.h>
#include "Arduino.h"

#define FONA_RX 2
#define FONA_TX 3
#define FONA_RST 4
#define FONA_KEY 7
#define FONA_PS 8

class Ubidots{
private:
    SoftwareSerial fonaSS = SoftwareSerial(FONA_TX, FONA_RX);
    Adafruit_FONA fona = Adafruit_FONA(FONA_RST);
    char* _token;
    char* apn;
    
public:
    
    Ubidots(char* token);
    void gprsNetwork(const __FlashStringHelper *apn, const __FlashStringHelper *username, const __FlashStringHelper *password);
    void saveValue(char* myid, float value);
    float getValue(char* myid);
    void flushSerial();
    void gprsOnFona();
    void turnOffFona();
    void turnOnFona();
    void checkFona();
};
#endif

