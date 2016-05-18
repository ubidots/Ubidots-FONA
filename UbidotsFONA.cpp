
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
 
Ubidots::Ubidots(char* token, char* server){
    _token = token;
    _server = server;
    _dsName = NULL;
    _dsTag = "FONA";
    currentValue = 0;
    val = (Value *)malloc(MAX_VALUES*sizeof(Value));
    fonaSS.begin(4800);
}
/*void Ubidots::begin() {
    pinMode(FONA_RST, OUTPUT);
    digitalWrite(FONA_RST, HIGH);
    delay(10);
    digitalWrite(FONA_RST, LOW);
    delay(100);
    digitalWrite(FONA_RST, HIGH);
    delay(500);
}*/

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
        Serial.println("------>AT");
        return false;
    }
    fonaSS.println("AT+CSQ");
    if (!waitForOK(6000)) {
        Serial.println("Error at CSQ");
        return false;
    }
    fonaSS.println("AT+CIPSHUT");
    if (!waitForOK(6000)) {
        Serial.println("Error at CIPSHUT");
        return false;
    }
    fonaSS.println("AT+CGATT?");
    if (!waitForOK(6000)) {
        Serial.println("GPRS is not attached");
        return false;
    }
    fonaSS.println("AT+SAPBR=3,1,\"CONTYPE\",\"GPRS\"");
    if (!waitForOK(6000)) {
        Serial.println("Error at setting up CONTYPE");
        return false;
    }
    fonaSS.print("AT+SAPBR=3,1,\"APN\",\"");
    fonaSS.print(apn);
    fonaSS.println("\"");
    if (!waitForOK(6000)) {
        Serial.println("Error at setting up APN");
        return false;
    }
    fonaSS.print("AT+SAPBR=3,1,\"USER\",\"");
    fonaSS.print(user);
    fonaSS.println("\"");
    if (!waitForOK(6000)) {
        Serial.println("Error at setting up apn user");
        return false;
    }
    fonaSS.print("AT+SAPBR=3,1,\"PWD\",\"");
    fonaSS.print(pwd);
    fonaSS.println("\"");
    if (!waitForOK(6000)) {
        Serial.println("Error at setting up apn pass");
        return false;
    }
    fonaSS.println("AT+SAPBR=1,1");
    if (!waitForOK(6000)) {
        Serial.println("Error with AT+SAPBR=1,1 Connection ip");
        return false;
    }
    fonaSS.println("AT+SAPBR=2,1");
    if (!waitForOK(6000)) {
        Serial.println("Error with AT+SAPBR=2,1 no IP to show");
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
    Serial.println(all.c_str());
    fonaSS.println("AT+CIPMUX=0");
    if (!waitForOK(6000)) {
        Serial.println("Error CIPMUX=0");
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
        Serial.println("Error at CIPSTART");
        return false;
    }
    fonaSS.println("AT+CIPSEND");
    if (!waitForMessage(">", 6000)) {
        Serial.println("Error at CIPSEND");
        return false;
    }
    fonaSS.write(all.c_str());
    fonaSS.write(0x1A);
    if (!waitForMessage("SEND OK", 6000)) {
        Serial.println("Error sending the message");
        return false;
    }
    fonaSS.println("AT+CIPCLOSE");
    if (!waitForMessage("CLOSE OK", 6000)) {
        Serial.println("Error closing TCP connection");
        return false;
    }
    return true;
}

float Ubidots::getValueWithDatasource(char* dsTag, char* idName) {
    float num;
    int i = 0;
    char allData[300];
    String response;
    uint8_t bodyPosinit;
    sprintf(allData, "%s%s|LV|%s|%s:%s|end", USER_AGENT, VERSION, _token, dsTag, idName);
    fonaSS.println("AT+CIPSEND");
    if (!waitForMessage(">", 6000)) {
        Serial.println("Error at CIPSEND");
        return false;
    }
    fonaSS.write(allData);
    fonaSS.write(0x1A);
    if (!waitForMessage("SEND OK", 6000)) {
        Serial.println("Error sending the message");
        return false;
    }
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

int Ubidots::readLine(uint32_t ts_max) {
    uint32_t ts_waitLF = 0;
    bool seenCR = false;
    int c;
    size_t bufcnt;
    bufcnt = 0;
    while (!isTimedOut(ts_max)) {
        wdt_reset();
        if (seenCR) {
            c = fonaSS.peek();
            // ts_waitLF is guaranteed to be non-zero
            if ((c == -1 && isTimedOut(ts_waitLF)) || (c != -1 && c != '\n')) {
                // Line ended with just <CR>. That's OK too.
                goto ok;
            }
      // Only \n should fall through
        }
        c = fonaSS.read();
        if (c < 0) {
            continue;
        }
        Serial.print((char)c);                 // echo the char
        seenCR = c == '\r';
        if (c == '\r') {
            ts_waitLF = millis() + 50;        // Wait another .05 sec for an optional LF
        } else if (c == '\n') {
            goto ok;
        } else {
            // Any other character is stored in the line buffer
            if (bufcnt < (DEFAULT_BUFFER_SIZE - 1)) {    // Leave room for the terminating NUL
                buffer[bufcnt++] = c;
            }
        }
    }
    Serial.println(F("readLine timed out"));
    return -1;            // This indicates: timed out
    ok:
    buffer[bufcnt] = 0;     // Terminate with NUL byte
    return bufcnt;
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