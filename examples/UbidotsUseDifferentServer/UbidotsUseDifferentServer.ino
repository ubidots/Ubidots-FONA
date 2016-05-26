#include <Ubidots_FONA.h>
#include <SoftwareSerial.h>


#define APN  ""  // The APN of your operator
#define USER ""  // if your apn doesnt have username just leave it ""
#define PASS ""  // if your apn doesnt have password just leave it ""
#define TOKEN "Your_Token_Here"  // Replace it with your Ubidots token
#define SEVER "your.server.com"  // Replace it with you selected server

Ubidots client(TOKEN, server);

void setup() {
  Serial.begin(115200);
  delay(2000);
  client.setApn(APN, USER, PASS);
}

void loop() {
  float value1 = analogRead(A0);
  client.add("Variable_Label_1", value1);
  client.sendAll();
}
