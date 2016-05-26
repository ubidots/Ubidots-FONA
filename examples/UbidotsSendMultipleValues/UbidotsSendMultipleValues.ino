#include <Ubidots_FONA.h>
#include <SoftwareSerial.h>


#define APN  ""  // The APN of your operator
#define USER ""  // if your apn doesnt have username just leave it ""
#define PASS ""  // if your apn doesnt have password just leave it ""
#define TOKEN "Your_token_here"  // Replace it with your Ubidots token

Ubidots client(TOKEN);  
  
void setup() {
  Serial.begin(115200);
  delay(2000);
  client.setApn(APN, USER, PASS);
}

void loop() {
  float value1 = analogRead(A0);
  float value2 = analogRead(A1);
  float value3 = analogRead(A2);
  client.add("Variable_Label_1", value1);
  client.add("Variable_Label_2", value2);
  client.add("Variable_Label_3", value3);
  client.sendAll();
}
