
#include <UbidotsFONA.h>
#include <SoftwareSerial.h>

#define APN  "xxxxxxxxxx"  // The APN of your operator
#define USER ""  // if your apn doesnt have username just leave it ""
#define PASS ""  // if your apn doesnt have password just leave it ""
#define TOKEN "Your_Token_Here"  // Replace it with your Ubidots token
#define VARIABLE_LABEL "temperature" // Replace it with your Ubidots variable label

unsigned long timestamp = 1509537600;

Ubidots client(TOKEN);

void setup() {
  Serial.begin(115200);
  delay(2000);
  client.setDebug(true); // comment this line to set DEBUG off
  //client.setDeviceName("new_device_name"); // uncomment this line to assign a different device name
  //client.setDeviceLabel("new_device_label"); // uncomment this line to assign a different device label
  while(!client.setApn(APN, USER, PASS));
}
void loop() {
    float value = analogRead(A0);
    client.add(VARIABLE_LABEL, 1.00 , timestamp);
    timestamp = timestamp + 5000;
    client.sendAll();
    delay(5000);
}
