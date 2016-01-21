#include <Ubidots_fona.h>
#define APN "web.colombiamovil.com.co"
#define USER ""
#define PASS ""

// Create a Ubidots client with your Ubidots TOKEN as an argument:
Ubidots client("CCN8FrVulRYGulPTk234R9Myx8qN2o");
  
void setup() {
 //For FONA MODULE
  Serial.begin(115200);
  delay(2000);
  client.gprsNetwork(F(APN),F(USER),F(PASS));
}

void loop() {
  client.flushSerial();
  client.checkFona();
  client.gprsOnFona();
  float value = client.get_value("568d8a0a76254218b18479ec");
  delay(600);
  Serial.println(value);
}
