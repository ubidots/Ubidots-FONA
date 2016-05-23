
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

bool Ubidots::SendMessageAndwaitForOK(char* message, uint16_t timeout) {
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
        if (!SendMessageAndwaitForOK(message[i], 6000)) {
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
        if (!SendMessageAndwaitForOK(message[i],6000)) {
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
        Serial.println(message[i]);
        if (!waitForMessage(message[i+3],6000)) {
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
    sprintf(allData, "%s%s|LV|%s|%s:%s|end\n\x1A", USER_AGENT, VERSION, _token, dsTag, idName);
    fonaSS.println("AT+CIPSEND");
    if (!waitForMessage(">", 6000)) {
        Serial.println("Error at CIPSEND");
        return false;
    }
    fonaSS.write(allData);
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