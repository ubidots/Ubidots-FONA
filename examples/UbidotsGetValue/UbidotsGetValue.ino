#include <Ubidots_FONA.h>
#include <SoftwareSerial.h> 
#include <Adafruit_FONA.h>

#define APN  ""  // The APN of your operator
#define USER ""  // if your apn doesnt have username just leave it ""
#define PASS ""  // if your apn doesnt have password just leave it ""
#define TOKEN "Your_token_here"  // Replace it with your Ubidots token
#define ID "Your_id_here" // Replace it with your Ubidots' variable ID

Ubidots client(TOKEN);  
  
void setup() {
  Serial.begin(115200);
  delay(2000);
  client.gprsNetwork(F(APN),F(USER),F(PASS));
}

void loop() {
  client.flushSerial();
  client.checkFona();
  client.gprsOnFona();
  float value = client.getValue(ID);  
  delay(1000);  // 600 milliseconds 
  Serial.println(value);
}
