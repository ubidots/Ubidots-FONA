
#include <UbidotsFONA.h>
#include <SoftwareSerial.h>


#define APN  "xxxxxxxxxx"  // The APN of your operator
#define USER ""  // if your apn doesnt have username just leave it ""
#define PASS ""  // if your apn doesnt have password just leave it ""
#define TOKEN "Your_Token_Here"  // Replace it with your Ubidots token
#define VARIABLE_LABEL "position" // This variable will automatically position the position of your device

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
    char context[25];
    char lat[] = "37.773972";
    char lng[] = "-122.431297";
    // To send latitude and longitude to Ubidots and see it in a map widget
    sprintf(context, "lat=%s$lng=%s", lat, lng);
    client.add(VARIABLE_LABEL, 1.00 , context);
    if(client.sendAll()){
      Serial.println("values sent properly");
    }
}
