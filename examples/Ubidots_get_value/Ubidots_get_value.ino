#include <Ubidots_fona.h>
#define APN "Your_apn_of_your_SIM_here" 
#define USER ""  // if your apn doesnt have username just put ""
#define PASS ""  // if your apn doesnt have password just put ""
#define TOKEN "CCN8GrVulRYGulPTk234R9Myx8qN2o"  // Remplace it with your token
#define ID "" // Remplace it with your variable ID

Ubidots client(TOKEN);  
  
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
  float value = client.get_value(ID);  
  delay(600);  // 600 milliseconds 
  Serial.println(value);
}