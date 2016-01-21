#include <Ubidots_fona.h>
#define APN "Your_apn_of_your_SIM_here" 
#define USER ""  // If your apn doesnt have username just put ""
#define PASS ""  // If your apn doesnt have password just put ""
#define TOKEN "CCN8GrVulRYGulPTk234R9Myx8qN2o"  // Remplace it with your token
#define ID "2345rVulRYGulPTk234R9Myx8qN2o" // Remplace it with your variable ID

Ubidots client(TOKEN);  
  
void setup() {
 //For FONA MODULE
  Serial.begin(115200);
  delay(2000);
  client.gprsNetwork(F(APN),F(USER),F(PASS));  //  First set your apn data
}

void loop() {
  float value = analogRead(A0);  // Reading analog pin A0
  client.flushSerial();
  client.checkFona();
  client.gprsOnFona();
  client.save_value(value, ID);  
  delay(600);  // 600 milliseconds 
}