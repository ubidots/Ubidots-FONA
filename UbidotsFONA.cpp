#include "UbidotsFONA.h"
#include <stdlib.h>
/**
 * Constructor.
 */
 
Ubidots::Ubidots(char* token, char* server = NULL){
    _token = token;
    _server = server;
    _dsName = NULL;
    _dsTag = "FONA";
    currentValue = 0;
    val = (Value *)malloc(MAX_VALUES*sizeof(Value));
    fonaSS.begin(4800);
}
void Ubidots::begin() {
    pinMode(FONA_RST, OUTPUT);
    digitalWrite(FONA_RST, HIGH);
    delay(10);
    digitalWrite(FONA_RST, LOW);
    delay(100);
    digitalWrite(FONA_RST, HIGH);
    delay(500);
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
    fonaSS.println("AT");
    if (!waitForOK(6000)) {
        SerialUSB.println("------>AT");
        return false;
    }
    fonaSS.println("AT+CSQ");
    if (!waitForOK(6000)) {
        SerialUSB.println("Error at CSQ");
        return false;
    }
    fonaSS.println("AT+CIPSHUT");
    if (!waitForOK(6000)) {
        SerialUSB.println("Error at CIPSHUT");
        return false;
    }
    fonaSS.println("AT+CGATT?");
    if (!waitForOK(6000)) {
        SerialUSB.println("GPRS is not attached");
        return false;
    }
    fonaSS.println("AT+SAPBR=3,1,\"CONTYPE\",\"GPRS\"");
    if (!waitForOK(6000)) {
        SerialUSB.println("Error at setting up CONTYPE");
        return false;
    }
    fonaSS.print("AT+SAPBR=3,1,\"APN\",\"");
    fonaSS.print(apn);
    fonaSS.println("\"");
    if (!waitForOK(6000)) {
        SerialUSB.println("Error at setting up APN");
        return false;
    }
    fonaSS.print("AT+SAPBR=3,1,\"USER\",\"");
    fonaSS.print(user);
    fonaSS.println("\"");
    if (!waitForOK(6000)) {
        SerialUSB.println("Error at setting up apn user");
        return false;
    }
    fonaSS.print("AT+SAPBR=3,1,\"PWD\",\"");
    fonaSS.print(pwd);
    fonaSS.println("\"");
    if (!waitForOK(6000)) {
        SerialUSB.println("Error at setting up apn pass");
        return false;
    }
    fonaSS.println("AT+SAPBR=1,1");
    if (!waitForOK(6000)) {
        SerialUSB.println("Error with AT+SAPBR=1,1 Connection ip");
        return false;
    }
    fonaSS.println("AT+SAPBR=2,1");
    if (!waitForOK(6000)) {
        SerialUSB.println("Error with AT+SAPBR=2,1 no IP to show");
        return false;
    }
    return true;
}

bool Ubidots::sendAll() {
    int i;
    String all;
    String str;
    all = USER_AGENT;
    all += VERSION;
    all += "|POST|";
    all += _token;
    all += "|";
    all += _dsTag;
    if (_dsName != NULL) {
        all += ":";
        all += _dsName;
    }
    all += "=>";
    for (i = 0; i < currentValue; ) {
        str = String(((val + i)->varValue), 2);
        all += String((val + i)->varName);
        all += ":";
        all += str;
        if ((val + i)->ctext != NULL) {
            all += "$";
            all += String((val + i)->ctext);
        }
        all += ","; 
        i++;
    }
    all += "|end";
    Serial.println(all.c_str());
    fonaSS.println("AT+CIPMUX=0");
    if (!waitForOK(6000)) {
        SerialUSB.println("Error CIPMUX=0");
        return false;
    }
    fonaSS.print("AT+CIPSTART=\"TCP\",\"");
    if (_server != NULL) {
        fonaSS.print(_server);    
    } else {
        fonaSS.print(SERVER);
    }
    fonaSS.print("\",\"");
    fonaSS.print(PORT);
    fonaSS.println("\"");
    if (!waitForOK(6000)) {
        SerialUSB.println("Error at CIPSTART");
        return false;
    }
    fonaSS.println("AT+CIPSEND");
    if (!waitForMessage(">", 6000)) {
        SerialUSB.println("Error at CIPSEND");
        return false;
    }
    fonaSS.write(all.c_str());
    fonaSS.write(0x1A);
    if (!waitForMessage("SEND OK", 6000)) {
        SerialUSB.println("Error sending the message");
        return false;
    }
    fonaSS.println("AT+CIPCLOSE");
    if (!waitForMessage("CLOSE OK", 6000)) {
        SerialUSB.println("Error closing TCP connection");
        return false;
    }
    return true;
}

bool Ubidots::waitForMessage(const char *msg, uint32_t ts_max) {
    int len;
    while ((len = readLine(ts_max)) >= 0) {
        if (len == 0) {
            // Skip empty lines
            continue;
        }
        if (strncmp(buffer, msg, strlen(msg)) == 0) {
            return true;
        }
    }
    return false;         // This indicates: timed out
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