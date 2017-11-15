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

Original Maker: Mateo Velez - Metavix for Ubidots Inc
Modified and Maintened by: María Carlina Hernández ---- Developer at Ubidots Inc
                           https://github.com/mariacarlinahernandez

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
 * Default device label is "FONA"
 */
Ubidots::Ubidots(char* token, char* server) {
    _token = token;
    _server = server;
    _device_name = NULL;
    _device_label = "FONA";
    _currentValue = 0;
    val = (Value *)malloc(MAX_VALUES*sizeof(Value));
}

/***************************************************************************
FONA MODULE FUNCTIONS
***************************************************************************/

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

bool Ubidots::sendMessageAndwaitForOK(char* message, uint16_t timeout) {
    fonaSS.println(message);
    if (strstr(readData(timeout), "OK") == NULL) {
        if (_debug) {
            Serial.println(F("Error"));
        }
            return false;
        }
    return true;
}

bool Ubidots::setApn(char* apn, char* user, char* pwd) {
    checkFona();
    fonaSS.println(F("AT"));
    if (strstr(readData(2000), "OK") == NULL) {
        if (_debug) {
            Serial.println(F("Error with AT"));
        }
        return false;
    }
    fonaSS.println(F("AT+CREG?"));
    if (strstr(readData(2000), "+CREG:") == NULL) {
        if (_debug) {
            Serial.println(F("Error with AT"));
        }
        return false;
    }
    fonaSS.println(F("AT+CSQ"));
    if (strstr(readData(2000), "OK") == NULL) {
        if (_debug) {
            Serial.println(F("Error with AT+CSQ"));
        }
        return false;
    }
    fonaSS.println(F("AT+CGATT?"));
    if (strstr(readData(10000), "OK") == NULL) {
        if (_debug) {
            Serial.println(F("Error with AT+CGATT"));
        }
        return false;
    }
    fonaSS.println(F("AT+SAPBR=3,1,\"CONTYPE\",\"GPRS\""));
    if (strstr(readData(10000), "OK") == NULL) {
        if (_debug) {
            Serial.println(F("Error with AT+SAPBR CONTYPE"));
        }
        return false;
    }
    fonaSS.print(F("AT+SAPBR=3,1,\"APN\",\""));
    fonaSS.print(apn);
    fonaSS.println(F("\""));
    if (strstr(readData(3000), "OK") == NULL) {
        if (_debug) {
            Serial.println(F("Error with AT+SAPBR APN"));
        }
        return false;
    }
    fonaSS.print(F("AT+SAPBR=3,1,\"USER\",\""));
    fonaSS.print(user);
    fonaSS.println(F("\""));
    if (strstr(readData(10000), "OK") == NULL) {
        if (_debug) {
            Serial.println(F("Error with AT+SAPBR USER"));
        }
        return false;
    }
    fonaSS.print(F("AT+SAPBR=3,1,\"PWD\",\""));
    fonaSS.print(pwd);
    fonaSS.println("\"");
    if (strstr(readData(3000), "OK") == NULL) {
        if (_debug) {
            Serial.println(F("Error with AT+SAPBR PASSWORD"));
        }
        return false;
    }
    fonaSS.println(F("AT+SAPBR=1,1"));
    if (strstr(readData(4000), "OK") == NULL) {
        if (_debug) {
            Serial.println(F("Error with AT+SAPBR=1,1 Connection ip"));
        }
        return false;
    }
    fonaSS.println(F("AT+SAPBR=2,1"));
    if (strstr(readData(4000), "+SAPBR:") == NULL) {
        if (_debug) {
            Serial.println(F("Error with AT+SAPBR=2,1 no IP to show"));
        }
        return false;
    }
    return true;
}

/***************************************************************************
FUNCTIONS TO SEND DATA
***************************************************************************/

/**
 * Add a value of variable to save
 * @arg variable_label variable label to save in a struct
 * @arg value variable value to save in a struct
 * @arg ctext [optional] is the context that you will save, default
 * @arg timestamp_val [optional] is the timestamp for the actual value
 * is NULL
 */
void Ubidots::add(char *variable_label, double value) {
  return add(variable_label, value, NULL, NULL);
}

void Ubidots::add(char *variable_label, double value, char *ctext) {
  return add(variable_label, value, ctext, NULL);
}

void Ubidots::add(char *variable_label, double value, unsigned long timestamp_val) {
  return add(variable_label, value, NULL, timestamp_val);
}

void Ubidots::add(char *variable_label, double value, char *ctext, long unsigned timestamp_val) {
  (val+_currentValue)->varName = variable_label;
  (val+_currentValue)->varValue = value;
  (val+_currentValue)->ctext = ctext;
  (val+_currentValue)->timestamp_val = timestamp_val;
  _currentValue++;
  if (_currentValue > MAX_VALUES) {
        Serial.println(F("You are sending more than the maximum of consecutive variables"));
        _currentValue = MAX_VALUES;
  }
}

/**
 * Send all data of all variables that you saved
 * @reutrn true upon success, false upon error.
 */
bool Ubidots::sendAll() {
    int i;
    char* allData = (char *) malloc(sizeof(char) * 700);
    char str_values[10];

    if (_device_name != NULL) {
      sprintf(allData, "%s/%s|POST|%s|%s:%s=>", USER_AGENT, VERSION, _token, _device_label, _device_name);
    } else {
      sprintf(allData, "%s/%s|POST|%s|%s=>", USER_AGENT, VERSION, _token, _device_label);
    }

    for (i = 0; i < _currentValue; ) {
        dtostrf(((val+i)->varValue), 6, 2, str_values);
        sprintf(allData, "%s%s:%s", allData, (val + i)->varName, str_values);

        if ((val + i)->timestamp_val != NULL) {
            sprintf(allData, "%s@%lu%s", allData, (val + i)->timestamp_val, "000");
        }

        if ((val + i)->ctext != NULL) {
            sprintf(allData, "%s$%s", allData, (val + i)->ctext);
        }

        i++;
        if (i < _currentValue) {
            sprintf(allData, "%s,", allData);
        }
    }
    sprintf(allData, "%s|end", allData);
    Serial.println(allData);

    fonaSS.println(F("AT+CIPMUX=0"));
    if (strstr(readData(4000), "OK") == NULL) {
        if (_debug) {
            Serial.println(F("Error with AT+CIPMUX"));
        }
        _currentValue = 0;
        return false;
    }
    fonaSS.print(F("AT+CIPSTART=\"TCP\",\""));
    fonaSS.print(_server);
    fonaSS.print(F("\",\""));
    fonaSS.print(PORT);
    fonaSS.println(F("\""));
    if (strstr(readData(4000), "CONNECT OK") == NULL) {
        if (_debug) {
            Serial.println(F("Error with AT+CIPSTART"));
        }
        _currentValue = 0;
        return false;
    }
    fonaSS.print(F("AT+CIPSEND="));
    fonaSS.println(dataLen(allData));
    if (strstr(readData(4000), ">") == NULL) {
        if (_debug) {
            Serial.println(F("Error with AT+CIPSEND"));
        }
        _currentValue = 0;
        return false;
    }
    fonaSS.write(allData);
    if (strstr(readData(4000), "SEND OK") == NULL) {
        if (_debug) {
            Serial.println(F("Error sending variables"));
        }
        _currentValue = 0;
        return false;
    }
    fonaSS.println(F("AT+CIPCLOSE"));
    if (strstr(readData(4000), "CLOSE OK") == NULL) {
        if (_debug) {
            Serial.println(F("Error with AT+CIPCLOSE"));
        }
        _currentValue = 0;
        return false;
    }
    fonaSS.println(F("AT+CIPSHUT"));
    if (strstr(readData(4000), "SHUT OK") == NULL) {
        if (_debug) {
            Serial.println(F("Error with AT+CIPSHUT"));
        }
        _currentValue = 0;
        return false;
    }
    _currentValue = 0;
    free(allData);
    return true;
}

/***************************************************************************
FUNCTIONS TO RETRIEVE DATA
***************************************************************************/

/**
 * This function is to get value from the Ubidots API with the device label
 * and variable label
 * @arg device_label is the label of the device
 * @arg variable_label is the label of the variable
 * @return num the data that you get from the Ubidots API
 */
float Ubidots::getValue(char* device_label, char* variable_label) {
    char* serverResponse;
    char* response;
    float num;

    char* allData = (char *) malloc(sizeof(char) * 700);
    sprintf(allData, "%s/%s|LV|%s|%s:%s|end", USER_AGENT, VERSION, _token, device_label, variable_label);

    fonaSS.println(F("AT+CIPMUX=0"));
    if (strstr(readData(4000), "OK") == NULL) {
        if (_debug) {
            Serial.println(F("Error with AT+CIPMUX"));
        }
        return false;
    }
    fonaSS.print(F("AT+CIPSTART=\"TCP\",\""));
    fonaSS.print(_server);
    fonaSS.print(F("\",\""));
    fonaSS.print(PORT);
    fonaSS.println(F("\""));
    if (strstr(readData(4000), "CONNECT OK") == NULL) {
        if (_debug) {
            Serial.println(F("Error with AT+CIPSTART"));
        }
        return false;
    }
    fonaSS.print(F("AT+CIPSEND="));
    fonaSS.println(dataLen(allData));
    if (strstr(readData(4000), ">") == NULL) {
        if (_debug) {
            Serial.println(F("Error with AT+CIPSEND"));
        }
        return false;
    }
    fonaSS.println(allData);
    char* resp = readData(4000);
    response = strtok(resp, "|");
    serverResponse = response;
    while (response!=NULL) {
        if (_debug) {
            printf("%s\n", response);
        }
        response = strtok(NULL, "|");
        if (response != NULL) {
            serverResponse = response;
        }
    }
    num = atof(serverResponse);

    fonaSS.println(F("AT+CIPCLOSE"));
    if (strstr(readData(4000), "CLOSE OK") == NULL) {
        if (_debug) {
            Serial.println(F("Error sending data"));
        }
        return false;
    }
    fonaSS.println(F("AT+CIPSHUT"));
    if (strstr(readData(4000), "SHUT OK") == NULL) {
        if (_debug) {
            Serial.println(F("Error with AT+CIPSHUT"));
        }
        return false;
    }
    free(allData);
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


/***************************************************************************
AUXILIAR FUNCTIONS
***************************************************************************/

/**
 * Assigns a new device name
 * @arg device_name new name that you want to assign to your device
 */
void Ubidots::setDeviceName(char* device_name) {
    _device_name = device_name;
}

/**
 * Assigns a new device label
 * @arg device_label new label that you want to assign to your device
 */
void Ubidots::setDeviceLabel(char* device_label) {
    _device_label = device_label;
}

/**
 * Gets the length of a variable
 * @arg variable a variable of type char
 * @return dataLen the length of the variable
 */
int Ubidots::dataLen(char* variable) {
  uint8_t dataLen = 0;
  for (int i = 0; i <= 250; i++) {
    if (variable[i] != '\0') {
      dataLen++;
    } else {
      break;
    }
  }
  return dataLen;
}

/**
 * Turns on or off debug messages
 * @debug is a bool flag to activate or deactivate messages
 */
void Ubidots::setDebug(bool debug) {
  _debug = debug;
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
    delay(2000);
    begin();
    if (!sendMessageAndwaitForOK("ATE0", 6000)) {
        Serial.print("Couldn't find FONA");
        while (1) {
            Serial.println("ERROR");
        }
        return false;
    }
    return true;
}

char* Ubidots::readData(uint16_t timeout) {
    uint16_t replyidx = 0;
    char replybuffer[254];
    int secconds = 0;
    while (!fonaSS.available() && secconds < timeout) {
        secconds++;
    }
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
        while (!fonaSS.available() && timeout > 0) {
            timeout--;
            delay(1);
        }

        if (timeout == 0) {
            if (fonaSS.available()) {
                timeout = 1000;
            } else {
                break;
            }
        }
    }
    replybuffer[replyidx] = '\0';  // null term
    if (_debug) {
        Serial.println("Response of FONA:");
        Serial.println(replybuffer);
    }
    return replybuffer;
}
