#include <UbidotsFONA.h>
#include <SoftwareSerial.h>

#define APN  "xxxxxxxxxx"  // The APN of your operator
#define USER ""  // if your apn doesnt have username just leave it ""
#define PASS ""  // if your apn doesnt have password just leave it ""
#define TOKEN "Your_token_here"  // Replace it with your Ubidots token
#define DEVICE_LABEL "Your_device_label" // Replace it with your Ubidots Device Label
#define VARIABLE_LABEL "Your_variable_label" // Replace it with your Ubidots Variable Label

Ubidots client(TOKEN);

void setup() {
  Serial.begin(115200);
  delay(2000);
  while(!client.setApn(APN, USER, PASS));
}

void loop() {
  float value = client.getValue(DEVICE_LABEL, VARIABLE_LABEL);
  delay(1000);
  Serial.print("The last value is: ");
  Serial.println(value);
}
