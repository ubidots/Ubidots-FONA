#include <UbidotsFONA.h>
#include <SoftwareSerial.h>

#define APN  "xxxxxxxxxx"  // The APN of your operator
#define USER ""  // if your apn doesnt have username just leave it ""
#define PASS ""  // if your apn doesnt have password just leave it ""
#define TOKEN "Your_token_here"  // Replace it with your Ubidots token
#define VARIABLE_LABEL_1 "temperature" // Replace it with your Ubidots variable label
#define VARIABLE_LABEL_2 "humidity" // Replace it with your Ubidots variable label
#define VARIABLE_LABEL_3 "pressure" // Replace it with your Ubidots variable label

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
  float value1 = analogRead(A0);
  float value2 = analogRead(A1);
  float value3 = analogRead(A2);
  client.add(VARIABLE_LABEL_1, value1);
  client.add(VARIABLE_LABEL_2, value2);
  client.add(VARIABLE_LABEL_3, value3);
  if(client.sendAll()){
    Serial.println("values sent properly");
  }
}
