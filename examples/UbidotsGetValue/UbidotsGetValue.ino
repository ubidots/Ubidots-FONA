#include <UbidotsFONA.h>
#include <SoftwareSerial.h>


#define APN  ""  // The APN of your operator
#define USER ""  // if your apn doesnt have username just leave it ""
#define PASS ""  // if your apn doesnt have password just leave it ""
#define TOKEN "Your_token_here"  // Replace it with your Ubidots token
#define ID "Your_id_here" // Replace it with your Ubidots' variable ID


Ubidots client(TOKEN);  
  
void setup() {
  Serial.begin(115200);
  delay(2000);
  while(!client.setApn(APN, USER, PASS));
}

void loop() {
  float value = client.getValueWithDatasource("Data_Source_Label", "Variable_Label");
  delay(1000); 
  Serial.println(value);
}
