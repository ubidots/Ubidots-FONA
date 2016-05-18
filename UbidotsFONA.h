#ifndef __UbidotsFONA_H_
#define __UbidotsFONA_H_
#define DEBUG_UBIDOTS

#include <SoftwareSerial.h>

#define FONA_RX 2
#define FONA_TX 3
#define FONA_RST 4
#define FONA_KEY 7
#define FONA_PS 8
#define SERVER "translate.ubidots.com"
#define PORT "9010"


typedef struct Value {
    char  *idName;
    char  *contextOne;
    float idValue;
} Value;


class Ubidots{
private:
    bool waitForOK(uint16_t timeout=4000);
    SoftwareSerial fonaSS = SoftwareSerial(FONA_TX, FONA_RX);
    char* _token;
    char* _dsName;
    char* _dsToken;
    char* _server;
    Value * val;

    
public:

    Ubidots(char* token, char* server = NULL);
    void setDataSourceName(char *dsName);
    void setDataSourceTag(char *dsTag);
    void add(char *variable_id, double value, char *ctext1 = NULL);
    void sendAll();
    bool setApn(char* apn, char* user = "", char* pwd = "");

    float getValue(char* myid);

    
    void gprsOnFona();
    void turnOffFona();
    void turnOnFona();
    void checkFona();
};
#endif

