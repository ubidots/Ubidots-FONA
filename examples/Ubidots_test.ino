#include <Ubidots_fona.h>
#define APN "web.colombiamovil.com.co"
#define USER ""
#define PASS ""

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
  client.save_value(344.32,"569801a77625423a8d09dc3f");
  float n = client.get_value("568d8a0a76254218b18479ec");
  Serial.println(n);
  delay(600);
}