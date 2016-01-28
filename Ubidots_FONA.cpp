#include "Ubidots_FONA.h"
#include <stdlib.h>
/**
 * Constructor.
 */
 
Ubidots::Ubidots(char* token){
    _token = token;
    fonaSS.begin(4800);
}
void Ubidots::gprsNetwork(const __FlashStringHelper *apn, const __FlashStringHelper *username, const __FlashStringHelper *password){
        fona.setGPRSNetworkSettings(apn, username, password);
}
void Ubidots::saveValue(char* myid, float value){
    uint16_t statuscode;
    int16_t length;
    char* url = (char *) malloc(sizeof(char) * 400);
    char* data = (char *) malloc(sizeof(char) * 50);
    char* val = (char *) malloc(sizeof(char) * 8);
    String(value).toCharArray(val,9);
    sprintf(url,"things.ubidots.com/api/v1.6/variables/%s/values?token=%s", myid, _token);
    sprintf(data,"{\"value\":%s}", val);  
    while (!fona.HTTP_POST_start(url, F("application/json"), (uint8_t *) data, strlen(data), &statuscode, (uint16_t *)&length)) {
        Serial.println("Failed!");
    }
    free(url);
    free(data);
    free(val);
    fona.HTTP_POST_end();
    flushSerial();
}
float Ubidots::getValue(char* myid){
    uint16_t statuscode;
    int16_t length;
    float num;
    char* url = (char *) malloc(sizeof(char) * 400);
    int i = 0;
    sprintf(url,"things.ubidots.com/api/v1.6/variables/%s/values?token=%s&page_size=1", myid, _token);
    while (!fona.HTTP_GET_start(url, &statuscode, (uint16_t *)&length)) {
        Serial.println("Failed!");
    }
    while(length>0){
        while(fonaSS.available()){
            url[i] = fonaSS.read();
            i++;
            length=length-1;
        }
        delay(10);
    }
    char* pch = strstr(url,"\"value\":");
    String raw(pch);
    int bodyPosinit =9+ raw.indexOf("\"value\":");
    int bodyPosend = raw.indexOf(", \"timestamp\"");
    raw.substring(bodyPosinit,bodyPosend).toCharArray(url, 10);
    num = atof(url);
    free(url);
    fona.HTTP_GET_end();
    flushSerial();
    return num;  
}
void Ubidots::flushSerial() {
    while (Serial.available())
    Serial.read();
}
void Ubidots::checkFona(){
    // See if the FONA is responding
    if (! fona.begin(fonaSS)) {           // can also try fona.begin(Serial1)
        Serial.println("Couldn't find FONA");
    }
    Serial.println("FONA is OK");
    //configure a GPRS APN
    
}
void Ubidots::gprsOnFona(){
    while(!fona.enableGPRS(true));
    Serial.println(F("Turn on"));
}
void Ubidots::turnOnFona(){
    Serial.println("Turning on Fona: ");
    while(digitalRead(FONA_PS)==LOW){
        digitalWrite(FONA_KEY, LOW);
    }
    digitalWrite(FONA_KEY, HIGH);
    delay(4000);
}

void Ubidots::turnOffFona(){
    Serial.println("Turning off Fona ");
    while(digitalRead(FONA_PS)==HIGH)
    {
        digitalWrite(FONA_KEY, LOW);
    }
    digitalWrite(FONA_KEY, HIGH);
    delay(4000);
}
