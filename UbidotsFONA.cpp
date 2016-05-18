#include "UbidotsFONA.h"
#include <stdlib.h>
/**
 * Constructor.
 */
 
Ubidots::Ubidots(char* token, char* server = NULL){
    _token = token;
    _server = server;
    _dsName = "FONA";
    _dsTag = "FONA";
    currentValue = 0;
    val = (Value *)malloc(MAX_VALUES*sizeof(Value));
    fonaSS.begin(4800);
}

void Ubidots::setDataSourceName(char* dsName) {
    _dsName = dsName;
}

void Ubidots::setDataSourceTag(char* dsTag) {
    _dsTag = dsTag;
}

void Ubidots::add(char *variable_id, float value, char *ctext1) {
    (val+currentValue)->idName = variable_id;
    (val+currentValue)->ctext = contextOne;
    (val+currentValue)->varValue = idValue;
    currentValue++;
    if (currentValue > MAX_VALUE) {
        currentValue = MAX_VALUE;
    }
}

bool Ubidots::waitForOK(uint16_t timeout) {
    int len;
    uint32_t ts_max = millis() + timeout;
    while ((len = readLine(ts_max)) >= 0) {
        if (len == 0) {
            // Skip empty lines
            continue;
        }
        if (strcmp_P(buffer, PSTR("OK")) == 0) {
            return true;
        } else if (strcmp_P(buffer, PSTR("ERROR")) == 0) {
            return false;
        }
        // Other input is skipped.
    }
    return false;
}

bool Ubidots::setApn(char* apn, char* user, char* pwd) {
    Serial1.println("AT+CSQ");
    if (!waitForOK(6000)) {
        SerialUSB.println("Error at CSQ");
        return false;
    }
    Serial1.println("AT+CIPSHUT");
    if (!waitForOK(6000)) {
        SerialUSB.println("Error at CIPSHUT");
        return false;
    }
    Serial1.println("AT+CGATT?");
    if (!waitForOK(6000)) {
        SerialUSB.println("GPRS is not attached");
        return false;
    }
    Serial1.println("AT+SAPBR=3,1,\"CONTYPE\",\"GPRS\"");
    if (!waitForOK(6000)) {
        SerialUSB.println("Error at setting up CONTYPE");
        return false;
    }
    Serial1.print("AT+SAPBR=3,1,\"APN\",\"");
    Serial1.print(apn);
    Serial1.println("\"");
    if (!waitForOK(6000)) {
        SerialUSB.println("Error at setting up APN");
        return false;
    }
    Serial1.print("AT+SAPBR=3,1,\"USER\",\"");
    Serial1.print(user);
    Serial1.println("\"");
    if (!waitForOK(6000)) {
        SerialUSB.println("Error at setting up apn user");
        return false;
    }
    Serial1.print("AT+SAPBR=3,1,\"PWD\",\"");
    Serial1.print(pwd);
    Serial1.println("\"");
    if (!waitForOK(6000)) {
        SerialUSB.println("Error at setting up apn pass");
        return false;
    }
    Serial1.println("AT+SAPBR=1,1");
    if (!waitForOK(6000)) {
        SerialUSB.println("Error with AT+SAPBR=1,1 Connection ip");
        return false;
    }
    Serial1.println("AT+SAPBR=2,1");
    if (!waitForOK(6000)) {
        SerialUSB.println("Error with AT+SAPBR=2,1 no IP to show");
        return false;
    }
    return true;
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


// See this functions and change
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