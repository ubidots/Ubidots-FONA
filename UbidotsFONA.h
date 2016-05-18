#ifndef __UbidotsFONA_H_
#define __UbidotsFONA_H_
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
#define VERSION "/1.1"
#define MAX_VALUES 5


typedef struct Value {
    char  *idName;
    char  *contextOne;
    float idValue;
} Value;


class Ubidots{
private:
    SoftwareSerial fonaSS = SoftwareSerial(FONA_TX, FONA_RX);
    bool waitForOK(uint16_t timeout=4000);
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

    Ubidots(char* token, char* server = NULL);
    void begin();
    void setDataSourceName(char *dsName);
    void setDataSourceTag(char *dsTag);
    void add(char *variable_id, float value, char *ctext1 = NULL);
    bool sendAll();
    float getValueWithDatasource(char* dsTag, char* idName);
    bool setApn(char* apn, char* user = "", char* pwd = "");

    void turnOffFona();
    void turnOnFona();
};
#endif

