
#include <Stream.h>
#include <avr/pgmspace.h>
#ifdef ARDUINO_ARCH_AVR
#include <avr/wdt.h>
#else
#define wdt_reset()
#endif
#include <stdlib.h>

#include "Ubidots_FONA.h"
/**
 * Constructor.
 */
 
Ubidots::Ubidots(char* token, char* server){
    _token = token;
    _server = server;
    _dsName = NULL;
    _dsTag = "FONA";
    currentValue = 0;
    val = (Value *)malloc(MAX_VALUES*sizeof(Value));
}

bool Ubidots::begin() {
    pinMode(FONA_RST, OUTPUT);
    digitalWrite(FONA_RST, HIGH);
    delay(500);
    digitalWrite(FONA_RST, LOW);
    delay(500);
    digitalWrite(FONA_RST, HIGH);
    delay(500);
    Serial.println(F("Attempting to open comm with ATs"));
    int16_t timeout = 10000;
    while (timeout > 0) {
        while (fonaSS.available()) Serial.println(fonaSS.read());
        if (sendMessageAndwaitForOK("AT"))
            break;
        while (fonaSS.available()) Serial.println(fonaSS.read());
        fonaSS.println("AT");
        if (waitForMessage("AT", 5000)) 
            break;
        delay(500);
        timeout-=500;
    }
    if (timeout <= 0) {
        Serial.println(F("Timeout: No response to AT... last ditch attempt."));
        sendMessageAndwaitForOK("AT");
        delay(100);
        sendMessageAndwaitForOK("AT");
        delay(100);
        sendMessageAndwaitForOK("AT");
        delay(100);
    }
    sendMessageAndwaitForOK("ATE0");
    delay(100);
}

void Ubidots::setDataSourceName(char* dsName) {
    _dsName = dsName;
}

void Ubidots::setDataSourceTag(char* dsTag) {
    _dsTag = dsTag;
}

void Ubidots::add(char *variable_id, float value, char *ctext1) {
    (val+currentValue)->idName = variable_id;
    (val+currentValue)->contextOne = ctext1;
    (val+currentValue)->idValue = value;
    currentValue++;
    if (currentValue > MAX_VALUES) {
        currentValue = MAX_VALUES;
    }
}

bool Ubidots::sendMessageAndwaitForOK(char* message, uint16_t timeout) {
    int len;
    fonaSS.println(message);
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
    checkFona();
    int i = 0;
    char message[10][50];
    sprintf(message[0], "AT");
    sprintf(message[1], "AT+CSQ");
    sprintf(message[2], "AT+CIPSHUT");
    sprintf(message[3], "AT+CGATT?");
    sprintf(message[4], "AT+SAPBR=3,1,\"CONTYPE\",\"GPRS\"");
    sprintf(message[5], "AT+SAPBR=3,1,\"APN\",\"%s\"",apn);
    sprintf(message[6], "AT+SAPBR=3,1,\"USER\",\"%s\"",user);
    sprintf(message[7], "AT+SAPBR=3,1,\"PWD\",\"%s\"",pwd);
    sprintf(message[8], "AT+SAPBR=1,1");
    sprintf(message[9], "AT+SAPBR=2,1");
    for(i = 0; i < 10; i++) {
        if (!sendMessageAndwaitForOK(message[i], 6000)) {
            Serial.print("Error with ");
            Serial.println(message[i]);
            return false;
        }
    }
    return true;
}

bool Ubidots::sendAll() {
    int i;
    char message[8][50];
    String all;
    String str;
    all = USER_AGENT;
    all += "/";
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
        str = String(((val + i)->idValue), 2);
        all += String((val + i)->idName);
        all += ":";
        all += str;
        if ((val + i)->contextOne != NULL) {
            all += "$";
            all += String((val + i)->contextOne);
        }
        all += ","; 
        i++;
    }
    all += "|end";
    sprintf(message[0], "AT+CIPMUX=0");
    sprintf(message[1], "AT+CIPSTART=\"TCP\",\"%s\",\"%s\"", _server, PORT);
    sprintf(message[2], "AT+CIPSEND");
    sprintf(message[4], "AT+CIPCLOSE");
    sprintf(message[5], ">");
    sprintf(message[6], "SEND OK");
    sprintf(message[7], "CLOSE OK");
    for (i = 0; i < 2; i++) {
        if (!sendMessageAndwaitForOK(message[i], 6000)) {
            Serial.print("Error with ");
            Serial.println(message[i]);
            currentValue = 0;
            return false;
        }
    }
    for (i = 2; i < 8; i++) {
        if (i != 3) {
            fonaSS.println(message[i]);
        } else {
            fonaSS.write(all.c_str());
            fonaSS.write(0x1A);
        }
        if (!waitForMessage(message[i+3], 6000)) {
            Serial.print("Error with ");
            Serial.println(message[i]);
            currentValue = 0;
            return false;
        }
    }
    currentValue = 0;
    return true;
}

float Ubidots::getValueWithDatasource(char* dsTag, char* idName) {
    float num;
    int i = 0;
    char allData[300];
    String response;
    uint8_t bodyPosinit;
    sprintf(allData, "%s/%s|LV|%s|%s:%s|end\n\x1A", USER_AGENT, VERSION, _token, dsTag, idName);
    fonaSS.println("AT+CIPSEND");
    if (!waitForMessage(">", 6000)) {
        Serial.println("Error at CIPSEND");
        return false;
    }
    fonaSS.write(allData);
    if (!waitForMessage("|OK", 6000)) {
        Serial.println("Error sending the message");
        return false;
    }
    response = String(buffer);
    bodyPosinit = 3 + response.indexOf("OK|");
    response = response.substring(bodyPosinit);
    num = response.toFloat();
    currentValue = 0;
    free(allData);
    return num;
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
int Ubidots::readLine(uint32_t ts_max, bool multiline) {
    int replyidx = 0;
    while (ts_max--) {
        if (replyidx >= 254) {
            break;
        }
        while(fonaSS.available()) {
            char c =  fonaSS.read();
            Serial.print(c);
            if (c == '\r') continue;
            if (c == 0xA) {
            if (replyidx == 0)   // the first 0x0A is ignored
                continue;
            }
            if (!multiline) {
                ts_max = 0;         // the second 0x0A is the end of the line
                break;
            }
            buffer[replyidx] = c;
            replyidx++;
        }
        if (ts_max == 0) {
            break;
        }
        delay(1);
    }
    buffer[replyidx] = 0;  // null term
    Serial.println(buffer);
    return replyidx;
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


// ------------------------------------------------------------------
// -------------------------WARNING----------------------------------
// ------------------------------------------------------------------
/* Deprecated functions
* Next functions was deprecated at version 1.2 of Fona Library
*/

void Ubidots::gprsNetwork(const __FlashStringHelper *apn, const __FlashStringHelper *username, const __FlashStringHelper *password) {
    checkFona();
    _apn = reinterpret_cast<char*>(const_cast<__FlashStringHelper*>(apn));
    _user = reinterpret_cast<char*>(const_cast<__FlashStringHelper*>(username));
    _pwd = reinterpret_cast<char*>(const_cast<__FlashStringHelper*>(password));
}
bool Ubidots::saveValue(char* myid, float value) {
    char data[25];
    String val;
    val = String(value, 2);
    httpTerm();
    sprintf(data,"{\"value\":%s}", val.c_str());
    Serial.println(data);
    httpInit();
    Serial.println(_token);
    fonaSS.print(F("AT+HTTPPARA=\"URL\",\"things.ubidots.com/api/v1.6/variables/"));
    fonaSS.print(myid);
    fonaSS.print(F("/values?token="));
    fonaSS.print(_token);
    if (!sendMessageAndwaitForOK("\"", 4000)) {
        Serial.println(F("Error with AT+HTTPARA URL"));     
        httpTerm();
        return false;
    }
    if (!sendMessageAndwaitForOK("AT+HTTPPARA=\"CONTENT\",\"application/json\"", 4000)) {
#ifdef DEBUG_UBIDOTS
        Serial.println(F("Error with AT+HTTPARA CONTENT"));
#endif
        httpTerm();
        return false;
    }
    fonaSS.print(F("AT+HTTPDATA="));
    fonaSS.print(strlen(data));
    fonaSS.print(F(","));
    fonaSS.println(120000);
    if (!waitForMessage("DOWNLOAD",2000)) {
#ifdef DEBUG_UBIDOTS
        Serial.println(F("Error with AT+HTTPDATA"));
#endif
        httpTerm();
        return false;
    }
    fonaSS.write(data, strlen(data));
    if (!waitForMessage("OK",2000)) {
#ifdef DEBUG_UBIDOTS
        Serial.println(F("Error sending data and data length"));
#endif
        httpTerm();
        return false;
    }
    fonaSS.println(F("AT+HTTPACTION=1"));  // HTTPACTION=1 is a POST method
    delay(5000);
    if (!waitForMessage("+HTTPACTION:1,201", 2000)) {
#ifdef DEBUG_UBIDOTS
        Serial.println(F("Error with AT+HTTPACTION=1"));
        Serial.println(F("Status = 603 is DNS Error, maybe your SIM doesn't have mobile data"));
        Serial.println(F("Status = 601 Network Error, maybe your SIM doesn't have mobile data"));
        Serial.println(F("Status = 400 Bad Request, Your URL is wrong"));
        Serial.println(F("Status = 402 Payment Required, You exceed your dots in Ubidots"));
#endif
        httpTerm();
        return false;
    } else {
        fonaSS.println(F("AT+HTTPREAD"));
        if (!waitForMessage("+HTTPREAD:", 2000)) {
#ifdef DEBUG_UBIDOTS
            Serial.println(F("Error with AT+HTTPREAD. Closing HTTP Client"));
#endif
            httpTerm();
            return false;
        }
    }
    httpTerm();
    return true;
}
float Ubidots::getValue(char* myid) {
    float num;
    String raw;
    uint8_t bodyPosinit;
    uint8_t bodyPosend;
    flushInput();
    httpTerm();
    httpInit();
    fonaSS.print(F("AT+HTTPPARA=\"URL\",\"things.ubidots.com/api/v1.6/variables/"));
    fonaSS.print(myid);
    fonaSS.print(F("/values?token="));
    fonaSS.print(_token);
    if (!sendMessageAndwaitForOK("&page_size=1\"", 6000)) {
        httpTerm();
#ifdef DEBUG_UBIDOTS
        Serial.println(F("Error with AT+HTTPPARA URL. Closing HTTP Client"));
#endif      
        return 1;
    }
    fonaSS.println(F("AT+HTTPACTION=0"));
    if (!waitForMessage("+HTTPACTION:0,200", 4000)) {
#ifdef DEBUG_UBIDOTS
        Serial.println(F("Status = 603 is DNS Error, maybe your SIM doesn't have mobile data"));
        Serial.println(F("Status = 601 Network Error, maybe your SIM doesn't have mobile data"));
        Serial.println(F("Status = 400 Bad Request, Your URL is wrong"));
        Serial.println(F("Status = 402 Payment Required, You exceed your dots in Ubidots"));
#endif
        httpTerm();
        return 1; 
    } else {
        fonaSS.println(F("AT+HTTPREAD"));
        if (!waitForMessage("+HTTPREAD:", 3000)) {
#ifdef DEBUG_UBIDOTS
        Serial.println(F("Error with AT+HTTPREAD. Closing HTTP Client"));
#endif
            httpTerm();
            return 1;
        }
        char* pch = strstr(buffer,"\"value\":");
#ifdef DEBUG_UBIDOTS
        String raw(pch);
#endif
        int bodyPosinit =9 + raw.indexOf("\"value\":");
        int bodyPosend = raw.indexOf(", \"timestamp\"");
        raw.substring(bodyPosinit, bodyPosend).toCharArray(buffer, 10);
        num = atof(buffer);
        httpTerm();
        return num;
    }
}
void Ubidots::gprsOnFona() {
    checkFona();
    setApn(_apn, _user, _pwd);
}
void Ubidots::flushSerial() {
    while (Serial.available())
    Serial.read();
}
bool Ubidots::httpTerm() {
    if (!sendMessageAndwaitForOK("AT+HTTPTERM", 6000)) {
        Serial.print("Error with AT+HTTPTERM");
        return false;
    }
    return true;
}
bool Ubidots::httpInit(){
    uint8_t i = 0;
    char message[3][50];
    sprintf(message[0], "AT+HTTPINIT");
    sprintf(message[1], "AT+HTTPPARA=\"CID\",1");
    sprintf(message[2], "AT+HTTPPARA=\"UA\",\"%s /%s\"");
    for (i = 0; i < 3; i++) {
        if (!sendMessageAndwaitForOK(message[i], 6000)) {
            Serial.print("Error with ");
            Serial.println(message[i]);
            return false;
        }
    }
    return true;
}

void Ubidots::flushInput() {
    // Read all available serial input to flush pending data.
    uint16_t timeoutloop = 0;
    while (timeoutloop++ < 40) {
        while(Serial.available()) {
            Serial.read();
            timeoutloop = 0;  // If char was received reset the timer
        }
        delay(1);
    }
}

bool Ubidots::checkFona() {
    fonaSS.begin(4800);
    void begin();
    if (!sendMessageAndwaitForOK("ATE0", 6000)) {
        Serial.print("Couldn't find FONA");
        return false;
        while (1);
    }
    return true;
}