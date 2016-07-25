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


#include <Stream.h>
#include <avr/pgmspace.h>
#ifdef ARDUINO_ARCH_AVR
#include <avr/wdt.h>
#else
#define wdt_reset()
#endif
#include <stdlib.h>

#include "UbidotsFONA.h"

/**
 * Constructor.
 */

Ubidots::Ubidots(char* token, char* server) {
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
        if (sendMessageAndwaitForOK("AT"))
            break;
        delay(500);
        timeout -= 500;
    }
    if (timeout <= 0) {
        Serial.println(F("Timeout: No response to AT... last ditch attempt."));
        sendMessageAndwaitForOK("AT");
        delay(100);
        sendMessageAndwaitForOK("AT");
        delay(100);
        sendMessageAndwaitForOK("AT");
        delay(100);
        return false;
    }
    sendMessageAndwaitForOK("ATE0");
    delay(100);
    return true;
}

void Ubidots::setDataSourceName(char* dsName) {
    _dsName = dsName;
}

void Ubidots::setDataSourceTag(char* dsTag) {
    _dsTag = dsTag;
}

void Ubidots::add(char *variable_id, float value, char *ctext1) {
    (val+currentValue)->varName = variable_id;
    (val+currentValue)->ctext = ctext1;
    (val+currentValue)->varValue = value;
    currentValue++;
    if (currentValue > MAX_VALUES) {
        currentValue = MAX_VALUES;
    }
}

bool Ubidots::sendMessageAndwaitForOK(char* message, uint16_t timeout) {
    fonaSS.println(message);
    if (strstr(readData(timeout), "OK") == NULL) {
#ifdef DEBUG_UBIDOTS
            Serial.println(F("Error sending variables"));
#endif
            return false;
        }
    return true;
}

bool Ubidots::setApn(char* apn, char* user, char* pwd) {
    checkFona();
    fonaSS.println(F("AT"));
    if (strstr(readData(2000), "OK") == NULL) {
#ifdef DEBUG_UBIDOTS
        Serial.println(F("Error with AT"));
#endif
        return false;
    }
    fonaSS.println(F("AT+CSQ"));
    if (strstr(readData(2000), "OK") == NULL) {
#ifdef DEBUG_UBIDOTS
        Serial.println(F("Error with AT+CSQ"));
#endif
        return false;
    }
    fonaSS.println(F("AT+CGATT?"));
    if (strstr(readData(10000), "OK") == NULL) {
#ifdef DEBUG_UBIDOTS
        Serial.println(F("Error with AT+CGATT"));
#endif
        return false;
    }
    fonaSS.println(F("AT+SAPBR=3,1,\"CONTYPE\",\"GPRS\""));
    if (strstr(readData(10000), "OK") == NULL) {
#ifdef DEBUG_UBIDOTS
        Serial.println(F("Error with AT+SAPBR CONTYPE"));
#endif
        return false;
    }
    fonaSS.print(F("AT+SAPBR=3,1,\"APN\",\""));
    fonaSS.print(apn);
    fonaSS.println(F("\""));
    if (strstr(readData(3000), "OK") == NULL) {
#ifdef DEBUG_UBIDOTS
        Serial.println(F("Error with AT+SAPBR APN"));
#endif
        return false;
    }
    fonaSS.print(F("AT+SAPBR=3,1,\"USER\",\""));
    fonaSS.print(user);
    fonaSS.println(F("\""));
    if (strstr(readData(10000), "OK") == NULL) {
#ifdef DEBUG_UBIDOTS
        Serial.println(F("Error with AT+SAPBR USER"));
#endif
        return false;
    }
    fonaSS.print(F("AT+SAPBR=3,1,\"PWD\",\""));
    fonaSS.print(pwd);
    fonaSS.println("\"");
    if (strstr(readData(3000), "OK") == NULL) {
#ifdef DEBUG_UBIDOTS
        Serial.println(F("Error with AT+SAPBR PASSWORD"));
#endif
        return false;
    }
    fonaSS.println(F("AT+SAPBR=1,1"));
    if (strstr(readData(4000), "OK") == NULL) {
#ifdef DEBUG_UBIDOTS
        Serial.println(F("Error with AT+SAPBR=1,1 Connection ip"));
#endif
        return false;
    }
    fonaSS.println(F("AT+SAPBR=2,1"));
    if (strstr(readData(4000), "+SAPBR:") == NULL) {
#ifdef DEBUG_UBIDOTS
        Serial.println(F("Error with AT+SAPBR=2,1 no IP to show"));
#endif
        return false;
    }
    return true;
}

bool Ubidots::sendAll() {
    int i;
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
        str = String(((val + i)->varValue), 2);
        all += String((val + i)->varName);
        all += ":";
        all += str;
        if ((val + i)->ctext != NULL) {
            all += "$";
            all += String((val + i)->ctext);
        }
        i++;
        if (i >= currentValue) {
            break;
        } else {
            all += ",";
        }
    }
    all += "|end";
    Serial.println(all.c_str());
    fonaSS.println(F("AT+CIPMUX=0"));
    if (strstr(readData(4000), "OK") == NULL) {
#ifdef DEBUG_UBIDOTS
        Serial.println(F("Error with AT+CIPMUX"));
#endif
        currentValue = 0;
        return false;
    }
    fonaSS.print(F("AT+CIPSTART=\"TCP\",\""));
    fonaSS.print(_server);
    fonaSS.print(F("\",\""));
    fonaSS.print(PORT);
    fonaSS.println(F("\""));
    if (strstr(readData(4000), "CONNECT OK") == NULL) {
#ifdef DEBUG_UBIDOTS
        Serial.println(F("Error with AT+CIPSTART"));
#endif
        currentValue = 0;
        return false;
    }
    fonaSS.print(F("AT+CIPSEND="));
    fonaSS.println(all.length());
    if (strstr(readData(4000), ">") == NULL) {
#ifdef DEBUG_UBIDOTS
        Serial.println(F("Error with AT+CIPSEND"));
#endif
        currentValue = 0;
        return false;
    }
    fonaSS.write(all.c_str());
    if (strstr(readData(4000), "SEND OK") == NULL) {
#ifdef DEBUG_UBIDOTS
        Serial.println(F("Error sending variables"));
#endif
        currentValue = 0;
        return false;
    }
    fonaSS.println(F("AT+CIPCLOSE"));
    if (strstr(readData(4000), "CLOSE OK") == NULL) {
#ifdef DEBUG_UBIDOTS
        Serial.println(F("Error with AT+CIPCLOSE"));
#endif
        currentValue = 0;
        return false;
    }
    fonaSS.println(F("AT+CIPSHUT"));
    if (strstr(readData(4000), "SHUT OK") == NULL) {
#ifdef DEBUG_UBIDOTS
        Serial.println(F("Error with AT+CIPSHUT"));
#endif
        currentValue = 0;
        return false;
    }
    currentValue = 0;
    return true;
}

float Ubidots::getValueWithDatasource(char* dsTag, char* idName) {
    float num;
    int i = 0;
    String allData;
    allData = USER_AGENT;
    allData += "/";
    allData += VERSION;
    allData += "|LV|";
    allData += _token;
    allData += "|";
    allData += dsTag;
    allData += ":";
    allData += idName;
    allData += "|end";
    String response;
    uint8_t bodyPosinit;
    fonaSS.println(F("AT+CIPMUX=0"));
    if (strstr(readData(4000), "OK") == NULL) {
#ifdef DEBUG_UBIDOTS
        Serial.println(F("Error with AT+CIPMUX"));
#endif
        return false;
    }
    fonaSS.print(F("AT+CIPSTART=\"TCP\",\""));
    fonaSS.print(_server);
    fonaSS.print(F("\",\""));
    fonaSS.print(PORT);
    fonaSS.println(F("\""));
    if (strstr(readData(4000), "CONNECT OK") == NULL) {
#ifdef DEBUG_UBIDOTS
        Serial.println(F("Error with AT+CIPSTART"));
#endif
        return false;
    }
    fonaSS.print(F("AT+CIPSEND="));
    fonaSS.println(allData.length());
    if (strstr(readData(4000), ">") == NULL) {
#ifdef DEBUG_UBIDOTS
        Serial.println(F("Error with AT+CIPSEND"));
#endif
        return false;
    }
    fonaSS.write(allData.c_str());
    response = String(readData(4000));
    Serial.println(response);
    fonaSS.println(F("AT+CIPCLOSE"));
    if (strstr(readData(4000), "CLOSE OK") == NULL) {
#ifdef DEBUG_UBIDOTS
        Serial.println(F("Error sending data"));
#endif
        return false;
    }
    fonaSS.println(F("AT+CIPSHUT"));
    if (strstr(readData(4000), "SHUT OK") == NULL) {
#ifdef DEBUG_UBIDOTS
        Serial.println(F("Error with AT+CIPSHUT"));
#endif
        return false;
    }
    bodyPosinit = 3 + response.indexOf("OK|");
    response = response.substring(bodyPosinit);
    num = response.toFloat();
    return num;
}
int Ubidots::readLine(uint32_t ts_max, bool multiline) {
    int replyidx = 0;
    while (ts_max--) {
        if (replyidx >= 254) {
            break;
        }
        while (fonaSS.available()) {
            char c =  fonaSS.read();
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


// ------------------------------------------------------------------
// -------------------------WARNING----------------------------------
// ------------------------------------------------------------------
/* Deprecated functions
* Next functions was deprecated at version 1.2 of Fona Library
*/


void Ubidots::gprsOnFona() {
    checkFona();
    setApn(_apn, _user, _pwd);
}

bool Ubidots::checkFona() {
    fonaSS.begin(4800);
    Serial.println("Pase FONASss");
    delay(2000);
    begin();
    Serial.println("Pase begin");
    if (!sendMessageAndwaitForOK("ATE0", 6000)) {
        Serial.print("Couldn't find FONA");
        while (1) {
            Serial.println("ERROR");
        }
        return false;
    }
    Serial.println("Sali check");
    return true;
}

char* Ubidots::readData(uint16_t timeout) {
    uint16_t replyidx = 0;
    char replybuffer[254];
    while (timeout--) {
        if (replyidx >= 254) {
            break;
        }
        while (fonaSS.available()) {
            char c =  fonaSS.read();
            if (c == '\r') continue;
            if (c == 0xA) {
                if (replyidx == 0)   // the first 0x0A is ignored
                    continue;
            }
            replybuffer[replyidx] = c;
            replyidx++;
        }

        if (timeout == 0) {
            if (fonaSS.available()) {
                timeout = 5000;
            } else {
                break;
            }
        }
        delay(1);
    }
    replybuffer[replyidx] = '\0';  // null term
#ifdef DEBUG_UBIDOTS
    Serial.println("Response of FONA:");
    Serial.println(replybuffer);
#endif
    return replybuffer;
}
